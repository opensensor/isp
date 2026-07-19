#!/usr/bin/env python3
"""Rebuild the T40 LSC real-parameter image and compare it to stock RAM."""

from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path


TUNING_HEADER_BYTES = 24
LSC_TOOL_OFFSET = 13036
LSC_TOOL_BYTES = 0xA324
LSC_REAL_BYTES = 27656

T_MODE = 0
T_GAIN0_LIN = 24
T_GAIN1_LIN = 46
T_WORDS = 120
T_CT_NODES = 122
T_LUT0 = 292
T_LUT1 = 14116
T_LUT2 = 27940

R_GAIN0 = 0
R_GAIN1 = 2
R_INTER = 4
R_NODES = 13828
R_PACKED = 13832


def u16(data: bytes | bytearray, offset: int) -> int:
    return struct.unpack_from("<H", data, offset)[0]


def u32(data: bytes | bytearray, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def intp_u16(gain: int, table: tuple[int, ...]) -> int:
    index = gain >> 16
    fraction = gain & 0xFFFF
    if index >= 10:
        return table[10]
    low, high = table[index : index + 2]
    if low == high:
        return high
    adjustment = abs(high - low) * fraction
    adjustment = (adjustment >> 16) + ((adjustment >> 15) & 1)
    return (low + adjustment if low < high else low - adjustment) & 0xFFFF


def lut_words(tool: bytes, offset: int, count: int) -> tuple[int, ...]:
    return struct.unpack_from(f"<{count}I", tool, offset)


def interpolate_ct(tool: bytes, ct: int, count: int) -> tuple[str, tuple[int, ...]]:
    breakpoints = struct.unpack_from("<4H", tool, T_CT_NODES)
    lut0 = lut_words(tool, T_LUT0, count)
    lut1 = lut_words(tool, T_LUT1, count)
    lut2 = lut_words(tool, T_LUT2, count)

    if ct < breakpoints[0]:
        return "lut0", lut0
    if ct < breakpoints[1]:
        lower, upper = lut0, lut1
        low_ct, high_ct = breakpoints[0:2]
        zone = "lut0/lut1"
    elif ct < breakpoints[2]:
        return "lut1", lut1
    elif ct < breakpoints[3]:
        lower, upper = lut1, lut2
        low_ct, high_ct = breakpoints[2:4]
        zone = "lut1/lut2"
    else:
        return "lut2", lut2

    fraction = ct - low_ct
    span = high_ct - low_ct
    lower_weight = span - fraction
    output = []
    for low, high in zip(lower, upper):
        upper_12 = ((((low >> 12) & 0xFFF) * lower_weight +
                     ((high >> 12) & 0xFFF) * fraction) // span) << 12
        lower_12 = (((low & 0xFFF) * lower_weight +
                     (high & 0xFFF) * fraction) // span) & 0xFFF
        output.append((upper_12 & 0xFFF000) | lower_12)
    return zone, tuple(output)


def build_real(tool: bytes, ct: int, gain: int) -> tuple[bytearray, str]:
    words = u16(tool, T_WORDS)
    if not words or words % 3:
        raise ValueError(f"invalid LSC LUT word count: {words}")

    gain0_table = struct.unpack_from("<11H", tool, T_GAIN0_LIN)
    gain1_table = struct.unpack_from("<11H", tool, T_GAIN1_LIN)
    zone, intermediate = interpolate_ct(tool, ct, words)
    real = bytearray(LSC_REAL_BYTES)
    struct.pack_into("<H", real, R_GAIN0, intp_u16(gain, gain0_table))
    struct.pack_into("<H", real, R_GAIN1, intp_u16(gain, gain1_table))
    struct.pack_into(f"<{words}I", real, R_INTER, *intermediate)
    struct.pack_into("<H", real, R_NODES, words // 3)

    destination = R_PACKED
    for index in range(0, words, 3):
        in0, in1, in2 = intermediate[index : index + 3]
        packed0 = ((in1 << 24) | (in0 & 0xFFFFFF)) & 0xFFFFFFFF
        packed1 = (((in1 >> 8) & 0xFFFF) | (in2 << 16)) & 0xFFFFFFFF
        packed2 = (in2 >> 16) & 0xFF
        struct.pack_into("<3I", real, destination, packed0, packed1, packed2)
        destination += 12
    return real, zone


def digest(data: bytes | bytearray) -> str:
    return hashlib.sha256(data).hexdigest()


def mismatch_summary(expected: bytes, actual: bytes) -> str:
    mismatches = [index for index, pair in enumerate(zip(expected, actual))
                  if pair[0] != pair[1]]
    if len(actual) != len(expected):
        mismatches.extend(range(min(len(actual), len(expected)),
                                max(len(actual), len(expected))))
    if not mismatches:
        return "exact"
    first = mismatches[:16]
    return (f"{len(mismatches)} differing bytes; first=" +
            ",".join(f"0x{offset:x}" for offset in first))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--blob", required=True, type=Path)
    parser.add_argument("--stock-real", required=True, type=Path)
    parser.add_argument("--stock-tool", type=Path)
    parser.add_argument("--ct", required=True, type=lambda value: int(value, 0))
    parser.add_argument("--gain", required=True, type=lambda value: int(value, 0))
    args = parser.parse_args()

    blob = args.blob.read_bytes()
    tool_start = TUNING_HEADER_BYTES + LSC_TOOL_OFFSET
    tool = blob[tool_start : tool_start + LSC_TOOL_BYTES]
    if len(tool) != LSC_TOOL_BYTES:
        raise ValueError("sensor blob is too short for the T40 LSC tool record")
    stock_real = args.stock_real.read_bytes()[:LSC_REAL_BYTES]
    if len(stock_real) != LSC_REAL_BYTES:
        raise ValueError("stock real-parameter dump is too short")

    expected, zone = build_real(tool, args.ct, args.gain)
    print(f"ct={args.ct} gain={args.gain} zone={zone} words={u16(tool, T_WORDS)}")
    print(f"gain_words=0x{u16(expected, R_GAIN0):04x}/0x{u16(expected, R_GAIN1):04x}")
    print(f"expected_sha256={digest(expected)}")
    print(f"stock_sha256={digest(stock_real)}")
    print(f"real_compare={mismatch_summary(expected, stock_real)}")
    print("intermediate_compare=" + mismatch_summary(
        expected[R_INTER:R_NODES], stock_real[R_INTER:R_NODES]))
    packed_bytes = u16(expected, R_NODES) * 12
    print("packed_compare=" + mismatch_summary(
        expected[R_PACKED:R_PACKED + packed_bytes],
        stock_real[R_PACKED:R_PACKED + packed_bytes]))

    if args.stock_tool:
        stock_tool = args.stock_tool.read_bytes()[:LSC_TOOL_BYTES]
        print(f"tool_compare={mismatch_summary(tool, stock_tool)}")
        print(f"tool_sha256={digest(tool)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
