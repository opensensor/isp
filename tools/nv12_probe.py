#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Render a dumped NV12/NV21 frame in several chroma interpretations."""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image


def clamp(v: int) -> int:
    return 0 if v < 0 else 255 if v > 255 else v


def yuv_to_rgb(y: int, u: int, v: int) -> tuple[int, int, int]:
    c = y - 16
    d = u - 128
    e = v - 128
    if c < 0:
        c = 0
    r = (298 * c + 409 * e + 128) >> 8
    g = (298 * c - 100 * d - 208 * e + 128) >> 8
    b = (298 * c + 516 * d + 128) >> 8
    return clamp(r), clamp(g), clamp(b)


def align(value: int, step: int) -> int:
    return ((value + step - 1) // step) * step


def render(data: bytes, width: int, height: int, stride: int,
           plane_height: int, order: str) -> Image.Image:
    y_len = stride * plane_height
    uv_len = y_len // 2
    if len(data) < y_len + uv_len:
        raise ValueError(f"need at least {y_len + uv_len} bytes, have {len(data)}")

    y_plane = data[:y_len]
    uv_plane = data[y_len:y_len + uv_len]
    out = bytearray(width * height * 3)

    neutral = order == "gray"
    swap = order == "nv21"
    for yy in range(height):
        y_row = yy * stride
        uv_row = (yy // 2) * stride
        out_row = yy * width * 3
        for xx in range(width):
            yv = y_plane[y_row + xx]
            if neutral:
                u = 128
                v = 128
            else:
                uv = uv_row + (xx & ~1)
                first = uv_plane[uv]
                second = uv_plane[uv + 1]
                if swap:
                    v, u = first, second
                else:
                    u, v = first, second
            r, g, b = yuv_to_rgb(yv, u, v)
            pos = out_row + xx * 3
            out[pos] = r
            out[pos + 1] = g
            out[pos + 2] = b

    return Image.frombytes("RGB", (width, height), bytes(out))


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("dump", type=Path)
    ap.add_argument("--width", type=int, default=1920)
    ap.add_argument("--height", type=int, default=1080)
    ap.add_argument("--stride", type=int, default=0)
    ap.add_argument("--plane-height", type=int, default=0)
    ap.add_argument("--out-dir", type=Path, default=None)
    args = ap.parse_args()

    data = args.dump.read_bytes()
    stride = args.stride or args.width
    plane_height = args.plane_height or align(args.height, 16)
    out_dir = args.out_dir or args.dump.with_suffix("")
    out_dir.mkdir(parents=True, exist_ok=True)

    for order in ("nv12", "nv21", "gray"):
        image = render(data, args.width, args.height, stride, plane_height,
                       order)
        out = out_dir / f"{args.dump.stem}-{order}.jpg"
        image.save(out, quality=92)
        print(out)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
