#!/usr/bin/env python3
"""Generate the exact T23 SC2336 BCSH startup register image."""

from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path


TUNING_HEADER_SIZE = 0x18
STARTUP_EV = 409600 >> 10
STARTUP_CT = 6000

OFFSETS = {
    "ccm_d2": (0x15158, 9),
    "ccm_d": (0x1517C, 9),
    "ccm_t": (0x151A0, 9),
    "ccm_a": (0x151C4, 9),
    "ccm_en": (0x151E8, 1),
    "ct_list": (0x151EC, 4),
    "hdp": (0x151FC, 3),
    "hbp": (0x15208, 3),
    "hlsp": (0x15214, 3),
    "sthres": (0x15220, 3),
    "ev_list": (0x1522C, 9),
    "smin_s": (0x15250, 9),
    "smax_s": (0x15274, 9),
    "smin_m": (0x15298, 9),
    "smax_m": (0x152BC, 9),
    "contrast": (0x152E0, 5),
    "cxl": (0x152F4, 9),
    "cxh": (0x15318, 9),
    "cyl": (0x1533C, 9),
    "cyh": (0x15360, 9),
    "brightness": (0x15384, 1),
    "offset_rgb": (0x15388, 3),
    "offset_yuv_y": (0x15394, 2),
    "clip0": (0x1539C, 4),
    "clip1": (0x153AC, 4),
    "matrix": (0x15518, 9),
    "matrix_inv": (0x1553C, 9),
    "clip2": (0x15560, 4),
}

RGB_TO_YUV = (1025, -508, -269, -2, 2309, 351, -2, 753, 2715)


def read_words(blob: bytes, parameter_offset: int, count: int) -> tuple[int, ...]:
    return struct.unpack_from(
        f"<{count}I", blob, TUNING_HEADER_SIZE + parameter_offset
    )


def signed32(value: int) -> int:
    return value - (1 << 32) if value & (1 << 31) else value


def signed14(value: int) -> int:
    return value - 0x4000 if value >= 0x2000 else value


def fixmul(a: int, b: int, shift: int) -> int:
    value = (abs(a) * abs(b)) >> shift
    return -value if (a < 0) != (b < 0) else value


def matmul(a: tuple[int, ...] | list[int], b: tuple[int, ...] | list[int], shift: int) -> list[int]:
    return [
        sum(fixmul(a[row * 3 + k], b[k * 3 + col], shift) for k in range(3))
        for row in range(3)
        for col in range(3)
    ]


def trunc_shift(value: int, shift: int) -> int:
    return -((-value) >> shift) if value < 0 else value >> shift


def interpolate(values: tuple[int, ...], ev_list: tuple[int, ...], ev: int) -> int:
    if ev < ev_list[0]:
        return values[0]
    if ev >= ev_list[-1]:
        return values[-1]

    for index, (ev_lo, ev_hi) in enumerate(zip(ev_list, ev_list[1:])):
        if ev_lo <= ev <= ev_hi:
            lo = values[index]
            hi = values[index + 1]
            delta = (ev - ev_lo) * abs(hi - lo) // (ev_hi - ev_lo)
            return lo + delta if hi >= lo else lo - delta
    raise ValueError(f"EV {ev} is outside the sorted interpolation table")


def pack16(high: int, low: int) -> int:
    return ((high & 0xFFFF) << 16) | (low & 0xFFFF)


def render(path: Path) -> str:
    blob = path.read_bytes()
    data = {
        name: read_words(blob, offset, count)
        for name, (offset, count) in OFFSETS.items()
    }

    ct_list = data["ct_list"]
    if tuple(sorted(ct_list)) != ct_list:
        raise ValueError(f"BCSH CT list is not sorted: {ct_list!r}")
    if STARTUP_CT < ct_list[3] - 200:
        raise ValueError(
            f"startup CT {STARTUP_CT} does not select the OEM D2 plateau"
        )
    if data["ccm_en"] != (1,):
        raise ValueError("SC2336 BCSH internal CCM is unexpectedly disabled")

    # At CT 6000 the OEM zone classifier selects the D2 plateau. Convert its
    # 14-bit register coefficients before applying M * CCM * Minv.
    ccm = tuple(signed14(value) for value in data["ccm_d2"])
    matrix = tuple(signed32(value) for value in data["matrix"])
    matrix_inv = tuple(signed32(value) for value in data["matrix_inv"])
    stage1 = matmul(matrix, ccm, 10)
    stage2 = matmul(stage1, matrix_inv, 16)
    hmatrix_signed = [
        trunc_shift(value, 6)
        for value in stage2
    ]
    hmatrix = [value & 0x3FFF if value < 0 else value for value in hmatrix_signed]
    if any(value > 0x3FFF for value in hmatrix):
        raise ValueError(f"BCSH HMatrix exceeds 14-bit register format: {hmatrix!r}")

    ev_list = data["ev_list"]
    if tuple(sorted(ev_list)) != ev_list:
        raise ValueError(f"BCSH EV list is not sorted: {ev_list!r}")
    svalue = [
        interpolate(data[name], ev_list, STARTUP_EV)
        for name in ("smin_s", "smax_s", "smin_m", "smax_m")
    ]
    contrast = [
        data["contrast"][0],
        interpolate(data["cxl"], ev_list, STARTUP_EV),
        interpolate(data["cxh"], ev_list, STARTUP_EV),
        interpolate(data["cyl"], ev_list, STARTUP_EV),
        interpolate(data["cyh"], ev_list, STARTUP_EV),
    ]

    if data["offset_rgb"] != (0x400, 0x400, 0x400):
        raise ValueError("generator currently requires the neutral SC2336 RGB offset")
    offset_rgb_yuv = (0x400, 0x400, 0x400)
    offset0 = (data["offset_yuv_y"][0], 0x400, 0x400)
    brightness = data["brightness"][0]
    offset1 = (
        data["offset_yuv_y"][1] + offset_rgb_yuv[0] - 0x800 + brightness,
        offset_rgb_yuv[1],
        offset_rgb_yuv[2],
    )

    clip0 = data["clip0"]
    clip1 = data["clip1"]
    clip2 = data["clip2"]
    hdp = data["hdp"]
    hbp = data["hbp"]
    hlsp = data["hlsp"]
    sthres = data["sthres"]

    cslope0 = (contrast[3] << 10) // contrast[1] if contrast[1] else 0
    cslope1 = (
        (abs(contrast[4] - contrast[3]) << 10)
        // (contrast[2] - contrast[1])
        if contrast[1] < contrast[2]
        else 0
    )
    cslope2 = (
        (abs(contrast[4] - clip0[1]) << 10)
        // (clip0[1] - contrast[2])
        if contrast[2] < clip0[1]
        else 0
    )
    sstep0 = abs(svalue[1] - svalue[0]) // (sthres[2] - sthres[1])
    sstep1 = abs(svalue[3] - svalue[2]) // (sthres[2] - sthres[1])
    hdp_slope = 0x400 // (hdp[2] - hdp[1])
    hbp_slope = 0x400 // (hbp[2] - hbp[1])
    hlsp_slope = 0x400 // (hlsp[2] - hlsp[1])

    registers = [
        (0x8000, pack16(clip0[0], clip0[1])),
        (0x8004, pack16(clip1[0], clip1[1])),
        (0x8008, pack16(clip2[0], clip2[1])),
        (0x800C, pack16(clip0[2], clip0[3])),
        (0x8010, pack16(clip1[2], clip1[3])),
        (0x8014, pack16(clip2[2], clip2[3])),
        (0x8018, pack16(offset1[0], offset0[0])),
        (0x801C, pack16(offset1[1], offset0[1])),
        (0x8020, pack16(offset1[2], offset0[2])),
        (0x8024, pack16(hmatrix[1], hmatrix[0])),
        (0x8028, hmatrix[2]),
        (0x802C, pack16(hmatrix[4], hmatrix[3])),
        (0x8030, hmatrix[5]),
        (0x8034, pack16(hmatrix[7], hmatrix[6])),
        (0x8038, hmatrix[8]),
        (0x803C, pack16(hdp_slope, hdp[0])),
        (0x8040, pack16(hdp[2], hdp[1])),
        (0x8044, pack16(hbp_slope, hbp[0])),
        (0x8048, pack16(hbp[2], hbp[1])),
        (0x804C, pack16(hlsp_slope, hlsp[0])),
        (0x8050, pack16(hlsp[2], hlsp[1])),
        (0x8054, pack16(cslope0, contrast[0])),
        (0x8058, pack16(cslope2, cslope1)),
        (0x805C, pack16(contrast[2], contrast[1])),
        (0x8060, pack16(contrast[4], contrast[3])),
        (0x8064, pack16(sstep1, (sstep0 << 3) | sthres[0])),
        (0x8068, pack16(sthres[2], sthres[1])),
        (0x806C, pack16(svalue[0], svalue[1])),
        (0x8070, pack16(svalue[2], svalue[3])),
    ]

    lines = [
        "/* Generated by tools/t23_bcsh_image.py. */",
        f"/* Source tuning SHA256: {hashlib.sha256(blob).hexdigest()} */",
        f"/* Startup EV {STARTUP_EV}; CT {STARTUP_CT} selects the D2 CCM plateau. */",
        "static const uint32_t regtrace_t23_bcsh_sc2336_startup[][2] = {",
    ]
    lines.extend(
        f"    {{ 0x{register:04x}U, 0x{value:08x}U }},"
        for register, value in registers
    )
    lines.append("};")
    return "\n".join(lines) + "\n"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("tuning_blob", type=Path)
    parser.add_argument("-o", "--output", type=Path)
    args = parser.parse_args()
    output = render(args.tuning_blob)
    if args.output:
        args.output.write_text(output, encoding="ascii")
    else:
        print(output, end="")


if __name__ == "__main__":
    main()
