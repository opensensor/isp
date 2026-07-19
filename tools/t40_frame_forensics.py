#!/usr/bin/env python3
"""Compare settled T40 stock and recovered RTSP frame bursts.

The gate deliberately measures more than frame-wide RGB means.  It excludes
the timestamp/Thingino overlays, summarizes temporal stability, compares a
spatial illumination and chroma-ratio grids, and reports full-reference
luma/color distances from averaged frames of the same fixed camera scene.
"""

from __future__ import annotations

import argparse
import glob
import json
import math
from pathlib import Path

import cv2
import numpy as np


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--stock", required=True, help="Glob for stock JPEG frames")
    parser.add_argument("--candidate", required=True, help="Glob for candidate JPEG frames")
    parser.add_argument("--output", type=Path, help="Optional JSON report path")
    parser.add_argument("--average-last", type=int, default=60)
    parser.add_argument("--grid-rows", type=int, default=6)
    parser.add_argument("--grid-cols", type=int, default=8)
    parser.add_argument(
        "--crop",
        default="72,48,48,48",
        help="Pixels to remove as top,bottom,left,right (default masks OSD)",
    )
    return parser.parse_args()


def frame_paths(pattern: str) -> list[Path]:
    paths = [Path(path) for path in sorted(glob.glob(pattern))]
    if not paths:
        raise ValueError(f"no frames matched {pattern!r}")
    return paths


def parse_crop(spec: str) -> tuple[int, int, int, int]:
    values = tuple(int(value, 0) for value in spec.split(","))
    if len(values) != 4 or any(value < 0 for value in values):
        raise ValueError("crop must be four non-negative integers: top,bottom,left,right")
    return values


def load_rgb(path: Path, crop: tuple[int, int, int, int]) -> np.ndarray:
    bgr = cv2.imread(str(path), cv2.IMREAD_COLOR)
    if bgr is None:
        raise ValueError(f"failed to decode {path}")
    top, bottom, left, right = crop
    height, width = bgr.shape[:2]
    if top + bottom >= height or left + right >= width:
        raise ValueError(f"crop {crop} consumes frame {width}x{height}")
    bgr = bgr[top : height - bottom, left : width - right]
    return cv2.cvtColor(bgr, cv2.COLOR_BGR2RGB)


def luma(rgb: np.ndarray) -> np.ndarray:
    pixels = rgb.astype(np.float32)
    return pixels[..., 0] * 0.299 + pixels[..., 1] * 0.587 + pixels[..., 2] * 0.114


def summarize(values: list[float]) -> dict[str, float]:
    data = np.asarray(values, dtype=np.float64)
    return {
        "mean": float(np.mean(data)),
        "std": float(np.std(data)),
        "min": float(np.min(data)),
        "max": float(np.max(data)),
    }


def normalized_grid(y: np.ndarray, rows: int, cols: int) -> np.ndarray:
    height, width = y.shape
    global_mean = max(float(np.mean(y)), 1.0)
    result = np.empty((rows, cols), dtype=np.float64)
    for row in range(rows):
        y0 = height * row // rows
        y1 = height * (row + 1) // rows
        for col in range(cols):
            x0 = width * col // cols
            x1 = width * (col + 1) // cols
            result[row, col] = float(np.mean(y[y0:y1, x0:x1])) / global_mean
    return result


def channel_ratio_grid(
    rgb: np.ndarray, numerator: int, denominator: int, rows: int, cols: int
) -> np.ndarray:
    """Cell-wise channel ratio; exposes radial color shading independent of AE."""
    height, width = rgb.shape[:2]
    result = np.empty((rows, cols), dtype=np.float64)
    for row in range(rows):
        y0 = height * row // rows
        y1 = height * (row + 1) // rows
        for col in range(cols):
            x0 = width * col // cols
            x1 = width * (col + 1) // cols
            cell = rgb[y0:y1, x0:x1]
            top = float(np.mean(cell[..., numerator]))
            bottom = max(float(np.mean(cell[..., denominator])), 1.0)
            result[row, col] = top / bottom
    return result


def edge_mask(shape: tuple[int, int]) -> np.ndarray:
    mask = np.zeros(shape, dtype=bool)
    mask[0, :] = True
    mask[-1, :] = True
    mask[:, 0] = True
    mask[:, -1] = True
    return mask


def histogram(image: np.ndarray) -> np.ndarray:
    values, _ = np.histogram(image, bins=256, range=(0.0, 256.0))
    values = values.astype(np.float64)
    return values / max(float(values.sum()), 1.0)


def histogram_hellinger(left: np.ndarray, right: np.ndarray) -> float:
    return float(np.sqrt(np.sum((np.sqrt(left) - np.sqrt(right)) ** 2)) / math.sqrt(2.0))


def global_ssim(left: np.ndarray, right: np.ndarray) -> float:
    left = left.astype(np.float64)
    right = right.astype(np.float64)
    mean_left = float(np.mean(left))
    mean_right = float(np.mean(right))
    var_left = float(np.var(left))
    var_right = float(np.var(right))
    covariance = float(np.mean((left - mean_left) * (right - mean_right)))
    c1 = (0.01 * 255.0) ** 2
    c2 = (0.03 * 255.0) ** 2
    numerator = (2.0 * mean_left * mean_right + c1) * (2.0 * covariance + c2)
    denominator = (mean_left**2 + mean_right**2 + c1) * (var_left + var_right + c2)
    return numerator / denominator if denominator else 1.0


def burst_metrics(
    paths: list[Path],
    crop: tuple[int, int, int, int],
    average_last: int,
    grid_rows: int,
    grid_cols: int,
) -> tuple[dict[str, object], np.ndarray, np.ndarray]:
    rgb_means: list[list[float]] = []
    rgb_stds: list[list[float]] = []
    luma_means: list[float] = []
    luma_stds: list[float] = []
    saturations: list[float] = []
    dark_clipping: list[float] = []
    bright_clipping: list[float] = []
    laplacian_variances: list[float] = []
    temporal_mads: list[float] = []
    temporal_p95s: list[float] = []
    previous_y: np.ndarray | None = None
    average_sum: np.ndarray | None = None
    average_count = 0

    average_start = max(0, len(paths) - average_last)
    for index, path in enumerate(paths):
        rgb = load_rgb(path, crop)
        y = luma(rgb)
        flat = rgb.reshape(-1, 3).astype(np.float64)
        rgb_means.append(np.mean(flat, axis=0).tolist())
        rgb_stds.append(np.std(flat, axis=0).tolist())
        luma_means.append(float(np.mean(y)))
        luma_stds.append(float(np.std(y)))
        hsv = cv2.cvtColor(rgb, cv2.COLOR_RGB2HSV)
        saturations.append(float(np.mean(hsv[..., 1])))
        dark_clipping.append(float(np.mean(y <= 4.0)))
        bright_clipping.append(float(np.mean(y >= 250.0)))
        laplacian = cv2.Laplacian(y, cv2.CV_32F)
        laplacian_variances.append(float(np.var(laplacian)))
        if previous_y is not None:
            delta = np.abs(y - previous_y)
            temporal_mads.append(float(np.mean(delta)))
            temporal_p95s.append(float(np.percentile(delta, 95)))
        previous_y = y
        if index >= average_start:
            if average_sum is None:
                average_sum = np.zeros(rgb.shape, dtype=np.float64)
            average_sum += rgb
            average_count += 1

    assert average_sum is not None and average_count
    average_rgb = (average_sum / average_count).astype(np.float32)
    average_y = luma(average_rgb)
    mean_array = np.asarray(rgb_means)
    std_array = np.asarray(rgb_stds)
    report: dict[str, object] = {
        "frame_count": len(paths),
        "first_frame": str(paths[0]),
        "last_frame": str(paths[-1]),
        "rgb_mean": {
            channel: summarize(mean_array[:, channel_index].tolist())
            for channel_index, channel in enumerate(("r", "g", "b"))
        },
        "rgb_spatial_std": {
            channel: summarize(std_array[:, channel_index].tolist())
            for channel_index, channel in enumerate(("r", "g", "b"))
        },
        "luma_mean": summarize(luma_means),
        "luma_spatial_std": summarize(luma_stds),
        "saturation_mean": summarize(saturations),
        "dark_clip_fraction": summarize(dark_clipping),
        "bright_clip_fraction": summarize(bright_clipping),
        "laplacian_variance": summarize(laplacian_variances),
        "temporal_luma_mad": summarize(temporal_mads) if temporal_mads else None,
        "temporal_luma_p95": summarize(temporal_p95s) if temporal_p95s else None,
        "normalized_luma_grid": normalized_grid(average_y, grid_rows, grid_cols).tolist(),
        "red_green_ratio_grid": channel_ratio_grid(
            average_rgb, 0, 1, grid_rows, grid_cols
        ).tolist(),
        "blue_green_ratio_grid": channel_ratio_grid(
            average_rgb, 2, 1, grid_rows, grid_cols
        ).tolist(),
    }
    return report, average_rgb, average_y


def comparison(
    stock_report: dict[str, object],
    candidate_report: dict[str, object],
    stock_rgb: np.ndarray,
    stock_y: np.ndarray,
    candidate_rgb: np.ndarray,
    candidate_y: np.ndarray,
) -> dict[str, object]:
    if stock_rgb.shape != candidate_rgb.shape:
        raise ValueError(f"frame shapes differ: {stock_rgb.shape} vs {candidate_rgb.shape}")

    delta_rgb = candidate_rgb.astype(np.float64) - stock_rgb.astype(np.float64)
    delta_y = candidate_y.astype(np.float64) - stock_y.astype(np.float64)
    mse_rgb = float(np.mean(delta_rgb**2))
    stock_grid = np.asarray(stock_report["normalized_luma_grid"], dtype=np.float64)
    candidate_grid = np.asarray(candidate_report["normalized_luma_grid"], dtype=np.float64)
    stock_rg_grid = np.asarray(stock_report["red_green_ratio_grid"], dtype=np.float64)
    candidate_rg_grid = np.asarray(
        candidate_report["red_green_ratio_grid"], dtype=np.float64
    )
    stock_bg_grid = np.asarray(stock_report["blue_green_ratio_grid"], dtype=np.float64)
    candidate_bg_grid = np.asarray(
        candidate_report["blue_green_ratio_grid"], dtype=np.float64
    )
    rg_delta = np.abs(candidate_rg_grid - stock_rg_grid)
    bg_delta = np.abs(candidate_bg_grid - stock_bg_grid)
    outer = edge_mask(stock_grid.shape)
    stock_temporal = stock_report["temporal_luma_mad"]
    candidate_temporal = candidate_report["temporal_luma_mad"]

    return {
        "rgb_mean_delta": {
            channel: float(
                candidate_report["rgb_mean"][channel]["mean"]
                - stock_report["rgb_mean"][channel]["mean"]
            )
            for channel in ("r", "g", "b")
        },
        "luma_mean_delta": float(
            candidate_report["luma_mean"]["mean"] - stock_report["luma_mean"]["mean"]
        ),
        "saturation_mean_delta": float(
            candidate_report["saturation_mean"]["mean"]
            - stock_report["saturation_mean"]["mean"]
        ),
        "rgb_mae": float(np.mean(np.abs(delta_rgb))),
        "luma_mae": float(np.mean(np.abs(delta_y))),
        "rgb_psnr_db": float(20.0 * math.log10(255.0 / math.sqrt(mse_rgb)))
        if mse_rgb
        else float("inf"),
        "luma_global_ssim": float(global_ssim(stock_y, candidate_y)),
        "luma_histogram_hellinger": histogram_hellinger(
            histogram(stock_y), histogram(candidate_y)
        ),
        "luma_grid_mae": float(np.mean(np.abs(candidate_grid - stock_grid))),
        "luma_grid_max_error": float(np.max(np.abs(candidate_grid - stock_grid))),
        "red_green_ratio_grid_mae": float(np.mean(rg_delta)),
        "red_green_ratio_grid_max_error": float(np.max(rg_delta)),
        "blue_green_ratio_grid_mae": float(np.mean(bg_delta)),
        "blue_green_ratio_grid_max_error": float(np.max(bg_delta)),
        "edge_red_green_ratio_mae": float(np.mean(rg_delta[outer])),
        "edge_blue_green_ratio_mae": float(np.mean(bg_delta[outer])),
        "temporal_luma_mad_ratio": float(
            candidate_temporal["mean"] / max(stock_temporal["mean"], 1.0e-9)
        )
        if stock_temporal and candidate_temporal
        else None,
        "laplacian_variance_ratio": float(
            candidate_report["laplacian_variance"]["mean"]
            / max(stock_report["laplacian_variance"]["mean"], 1.0e-9)
        ),
    }


def main() -> int:
    args = parse_args()
    crop = parse_crop(args.crop)
    stock_paths = frame_paths(args.stock)
    candidate_paths = frame_paths(args.candidate)
    stock_report, stock_rgb, stock_y = burst_metrics(
        stock_paths, crop, args.average_last, args.grid_rows, args.grid_cols
    )
    candidate_report, candidate_rgb, candidate_y = burst_metrics(
        candidate_paths, crop, args.average_last, args.grid_rows, args.grid_cols
    )
    report = {
        "schema": "t40-frame-forensics-v2",
        "crop_top_bottom_left_right": list(crop),
        "stock": stock_report,
        "candidate": candidate_report,
        "comparison": comparison(
            stock_report,
            candidate_report,
            stock_rgb,
            stock_y,
            candidate_rgb,
            candidate_y,
        ),
    }
    encoded = json.dumps(report, indent=2, sort_keys=True, allow_nan=False) + "\n"
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(encoded)
    print(encoded, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
