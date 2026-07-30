#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Offline forensics for a dumped T40 qbuf (NV12-layout) frame.

Given a raw qbuf dump (Y plane + interleaved UV), this reports the
diagnostics that distinguish the T40 failure classes without touching the
live camera:

  * Y-plane content strength (std) and whether the buffer is stale/black.
  * Chroma amplitude (U/V std).  A near-zero chroma std (~<5) means the
    frame is effectively monochrome -- "low channel spread" is then a
    desaturation artifact, NOT correct color.  Do not optimize for spread.
  * Chroma DC bias (U/V mean vs 128) and chroma-vs-luma correlation, which
    fingerprint demosaic / CFA-phase / CbCr-sign defects.
  * Dominant vertical band period via 1D FFT of a flat patch.  A strong
    ~64-row period is the VIC-MDMA stride/format banding signature (see
    docs/T40_TUNING_HURDLES.md): stock VIC stride is 0xF00, the recovered
    qbuf-ring path defaults to 0x780 (fmt=7).

It also writes luma/chroma JPEG renders next to the dump for eyeballing.
"""

from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
from PIL import Image


def align(v: int, step: int) -> int:
    return ((v + step - 1) // step) * step


def load_planes(path: Path, width: int, height: int):
    plane_h = align(height, 16)
    y_len = width * plane_h
    data = np.frombuffer(path.read_bytes(), dtype=np.uint8)
    if data.size < y_len + y_len // 2:
        raise SystemExit(f"{path}: have {data.size} bytes, "
                         f"need {y_len + y_len // 2} for {width}x{height}")
    y = data[:y_len].reshape(plane_h, width)[:height]
    uv = data[y_len:y_len + y_len // 2].reshape(plane_h // 2, width)[:height // 2]
    return y, uv[:, 0::2], uv[:, 1::2]


def band_period(y: np.ndarray) -> tuple[float, float]:
    """Dominant vertical period (rows) and its relative strength."""
    h, w = y.shape
    y0, x0 = h // 2 - 128, w // 2 - 128
    patch = y[y0:y0 + 256, x0:x0 + 256].astype(float)
    if patch.shape != (256, 256):
        return float("nan"), 0.0
    col = patch.mean(axis=1)
    col -= col.mean()
    spec = np.abs(np.fft.rfft(col * np.hanning(256)))
    spec[:2] = 0.0
    k = int(np.argmax(spec))
    period = 256.0 / k if k else float("inf")
    strength = float(spec[k] / (spec.mean() + 1e-9))
    return period, strength


def corr(a: np.ndarray, b: np.ndarray) -> float:
    a = a.astype(float).ravel() - a.mean()
    b = b.astype(float).ravel() - b.mean()
    return float((a * b).mean() / (a.std() * b.std() + 1e-9))


def yuv_to_rgb(y, U, V):
    h, w = y.shape
    Uu = np.repeat(np.repeat(U, 2, 0), 2, 1)[:h, :w].astype(np.int32)
    Vv = np.repeat(np.repeat(V, 2, 0), 2, 1)[:h, :w].astype(np.int32)
    c = np.clip(y.astype(np.int32) - 16, 0, None)
    d, e = Uu - 128, Vv - 128
    r = np.clip((298 * c + 409 * e + 128) >> 8, 0, 255)
    g = np.clip((298 * c - 100 * d - 208 * e + 128) >> 8, 0, 255)
    b = np.clip((298 * c + 516 * d + 128) >> 8, 0, 255)
    return np.dstack([r, g, b]).astype(np.uint8)


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("dump", type=Path)
    ap.add_argument("--width", type=int, default=1920)
    ap.add_argument("--height", type=int, default=1080)
    ap.add_argument("--render", action="store_true",
                    help="write luma/nv12 JPEGs next to the dump")
    args = ap.parse_args()

    y, U, V = load_planes(args.dump, args.width, args.height)
    Ydown = y[:args.height // 2 * 2].reshape(args.height // 2, 2,
                                             args.width // 2, 2).mean(axis=(1, 3))
    period, strength = band_period(y)

    print(f"file        {args.dump}")
    print(f"Y    mean {y.mean():6.1f}  std {y.std():5.1f}  "
          f"(<5 std => stale/flat)")
    print(f"U    mean {U.mean():6.1f}  std {U.std():5.1f}")
    print(f"V    mean {V.mean():6.1f}  std {V.std():5.1f}")
    chroma = max(U.std(), V.std())
    verdict = "MONOCHROME (desaturated)" if chroma < 5 else "has chroma"
    print(f"chroma std max {chroma:5.1f}  -> {verdict}")
    print(f"chroma DC bias  dU={U.mean()-128:+.1f}  dV={V.mean()-128:+.1f}  "
          f"(persistent +dV => red/magenta push)")
    print(f"corr(U,Y)={corr(U, Ydown):+.2f}  corr(V,Y)={corr(V, Ydown):+.2f}  "
          f"(strong negative => CFA/CbCr defect)")
    print(f"vertical band period {period:.1f} rows  strength {strength:.1f}x  "
          f"(~64/128 + high strength => VIC-MDMA stride/fmt banding)")

    if args.render:
        out = args.dump.with_suffix("")
        out.mkdir(parents=True, exist_ok=True)
        Image.fromarray(y).save(out / f"{args.dump.stem}-luma.jpg", quality=90)
        Image.fromarray(yuv_to_rgb(y, U.astype(np.int32), V.astype(np.int32))
                        ).save(out / f"{args.dump.stem}-nv12.jpg", quality=90)
        print(f"renders     {out}/")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
