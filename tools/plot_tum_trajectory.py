#!/usr/bin/env python3
"""Create a self-contained SVG plot from TUMTest trajectory diagnostics."""

import csv
import math
import sys
from pathlib import Path


WIDTH = 1000
HEIGHT = 700
MARGIN = 72


def aligned_estimates(samples):
    """Align accepted estimated X-Z points to ground truth with 2D similarity."""
    accepted = [row for row in samples if row["status"] == "accepted"]
    if len(accepted) < 2:
        return []

    source = [(float(row["estimated_x"]), float(row["estimated_z"])) for row in accepted]
    target = [(float(row["ground_truth_x"]), float(row["ground_truth_z"])) for row in accepted]
    source_center = tuple(sum(point[i] for point in source) / len(source) for i in range(2))
    target_center = tuple(sum(point[i] for point in target) / len(target) for i in range(2))

    dot = cross = source_energy = 0.0
    for src, dst in zip(source, target):
        sx, sz = src[0] - source_center[0], src[1] - source_center[1]
        tx, tz = dst[0] - target_center[0], dst[1] - target_center[1]
        dot += tx * sx + tz * sz
        cross += tz * sx - tx * sz
        source_energy += sx * sx + sz * sz

    if source_energy == 0.0:
        return []

    scale = math.hypot(dot, cross) / source_energy
    angle = math.atan2(cross, dot)
    cosine, sine = math.cos(angle), math.sin(angle)

    def transform(point):
        x, z = point[0] - source_center[0], point[1] - source_center[1]
        return (
            target_center[0] + scale * (cosine * x - sine * z),
            target_center[1] + scale * (sine * x + cosine * z),
        )

    return [transform(point) for point in source]


def points_attribute(points, project):
    return " ".join(f"{project(point)[0]:.1f},{project(point)[1]:.1f}" for point in points)


def write_pair_diagnostics(samples, output_path):
    """Plot parallax for all pairs and rotation error for accepted pairs."""
    width, height, margin = 1000, 540, 72
    plot_width = width - 2 * margin
    panel_height = 160
    frames = [int(row["frame"]) for row in samples]
    parallax = [float(row["median_parallax_px"]) for row in samples]
    rotation = [
        (int(row["frame"]), float(row["rotation_error_deg"]))
        for row in samples if row["rotation_error_deg"] != "nan"
    ]

    min_frame, max_frame = min(frames), max(frames)

    def x(frame):
        return margin + (frame - min_frame) / max(max_frame - min_frame, 1) * plot_width

    def y(value, values, top):
        maximum = max(max(values), 1e-6) * 1.08
        return top + panel_height - value / maximum * panel_height

    parallax_top, rotation_top = 105, 335
    parallax_path = " ".join(
        f"{x(frame):.1f},{y(value, parallax, parallax_top):.1f}"
        for frame, value in zip(frames, parallax)
    )
    rotation_path = " ".join(
        f"{x(frame):.1f},{y(value, [item[1] for item in rotation], rotation_top):.1f}"
        for frame, value in rotation
    )
    rejected_marks = "".join(
        f'<circle cx="{x(int(row["frame"])):.1f}" cy="{y(float(row["median_parallax_px"]), parallax, parallax_top):.1f}" r="3" class="rejected"/>'
        for row in samples if row["status"] == "rejected"
    )

    output_path.write_text(f'''<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">
  <style>
    text {{ font-family: -apple-system, BlinkMacSystemFont, sans-serif; fill: #1f2937; }}
    .frame {{ fill: #f8fafc; stroke: #64748b; }}
    .parallax {{ fill: none; stroke: #7c3aed; stroke-width: 2; }}
    .rotation {{ fill: none; stroke: #059669; stroke-width: 2; }}
    .rejected {{ fill: #dc2626; }}
  </style>
  <rect width="100%" height="100%" fill="white"/>
  <text x="{margin}" y="34" font-size="22" font-weight="700">TUM pair diagnostics</text>
  <rect x="{margin}" y="{parallax_top}" width="{plot_width}" height="{panel_height}" class="frame"/>
  <rect x="{margin}" y="{rotation_top}" width="{plot_width}" height="{panel_height}" class="frame"/>
  <polyline points="{parallax_path}" class="parallax"/>
  <g>{rejected_marks}</g>
  <polyline points="{rotation_path}" class="rotation"/>
  <text x="{margin}" y="{parallax_top - 12}" font-size="14">Median parallax (px); red markers are rejected pairs</text>
  <text x="{margin}" y="{rotation_top - 12}" font-size="14">Accepted-pair rotation error (degrees)</text>
  <text x="{margin}" y="{height - 18}" font-size="14">Sampled frame index</text>
</svg>''')


def main():
    if len(sys.argv) != 3:
        raise SystemExit("Usage: plot_tum_trajectory.py INPUT.csv OUTPUT.svg")

    input_path, output_path = map(Path, sys.argv[1:])
    with input_path.open(newline="") as csv_file:
        samples = list(csv.DictReader(csv_file))

    if not samples:
        raise SystemExit("No trajectory samples found.")

    ground_truth = [
        (float(row["ground_truth_x"]), float(row["ground_truth_z"]))
        for row in samples
    ]
    estimated = aligned_estimates(samples)
    rejected = [
        (float(row["ground_truth_x"]), float(row["ground_truth_z"]))
        for row in samples if row["status"] == "rejected"
    ]

    all_points = ground_truth + estimated
    min_x, max_x = min(point[0] for point in all_points), max(point[0] for point in all_points)
    min_z, max_z = min(point[1] for point in all_points), max(point[1] for point in all_points)
    span = max(max_x - min_x, max_z - min_z, 1e-6)
    padding = span * 0.08
    min_x, max_x = min_x - padding, max_x + padding
    min_z, max_z = min_z - padding, max_z + padding

    plot_width, plot_height = WIDTH - 2 * MARGIN, HEIGHT - 2 * MARGIN

    def project(point):
        x = MARGIN + (point[0] - min_x) / (max_x - min_x) * plot_width
        z = HEIGHT - MARGIN - (point[1] - min_z) / (max_z - min_z) * plot_height
        return x, z

    rotation_errors = [float(row["rotation_error_deg"]) for row in samples if row["rotation_error_deg"] != "nan"]
    mean_rotation_error = sum(rotation_errors) / len(rotation_errors) if rotation_errors else 0.0

    def crosses(points):
        parts = []
        for point in points:
            x, y = project(point)
            parts.append(f'<path d="M {x - 4:.1f} {y - 4:.1f} L {x + 4:.1f} {y + 4:.1f} M {x + 4:.1f} {y - 4:.1f} L {x - 4:.1f} {y + 4:.1f}"/>')
        return "".join(parts)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(f'''<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" height="{HEIGHT}" viewBox="0 0 {WIDTH} {HEIGHT}">
  <style>
    text {{ font-family: -apple-system, BlinkMacSystemFont, sans-serif; fill: #1f2937; }}
    .axis {{ stroke: #64748b; stroke-width: 1; }}
    .grid {{ stroke: #e2e8f0; stroke-width: 1; }}
    .ground {{ fill: none; stroke: #2563eb; stroke-width: 3; }}
    .estimate {{ fill: none; stroke: #f97316; stroke-width: 3; stroke-dasharray: 8 5; }}
    .rejected {{ stroke: #dc2626; stroke-width: 2; fill: none; }}
  </style>
  <rect width="100%" height="100%" fill="white"/>
  <text x="{MARGIN}" y="34" font-size="22" font-weight="700">TUM monocular VO trajectory</text>
  <text x="{MARGIN}" y="58" font-size="14">Estimated path uses a best-fit 2D similarity alignment to ground truth (X–Z view).</text>
  <rect x="{MARGIN}" y="{MARGIN}" width="{plot_width}" height="{plot_height}" fill="#f8fafc" class="axis"/>
  <line x1="{MARGIN}" y1="{HEIGHT - MARGIN}" x2="{WIDTH - MARGIN}" y2="{HEIGHT - MARGIN}" class="axis"/>
  <line x1="{MARGIN}" y1="{MARGIN}" x2="{MARGIN}" y2="{HEIGHT - MARGIN}" class="axis"/>
  <polyline points="{points_attribute(ground_truth, project)}" class="ground"/>
  <polyline points="{points_attribute(estimated, project)}" class="estimate"/>
  <g class="rejected">{crosses(rejected)}</g>
  <text x="{MARGIN}" y="{HEIGHT - 24}" font-size="14">X (meters, ground-truth frame)</text>
  <text x="18" y="{MARGIN + plot_height / 2}" font-size="14" transform="rotate(-90 18 {MARGIN + plot_height / 2})">Z (meters, ground-truth frame)</text>
  <line x1="{WIDTH - 420}" y1="38" x2="{WIDTH - 390}" y2="38" class="ground"/><text x="{WIDTH - 382}" y="43" font-size="13">Ground truth</text>
  <line x1="{WIDTH - 280}" y1="38" x2="{WIDTH - 250}" y2="38" class="estimate"/><text x="{WIDTH - 242}" y="43" font-size="13">Aligned estimate</text>
  <text x="{MARGIN}" y="{HEIGHT - 4}" font-size="13">Accepted pairs: {len(estimated)} · Rejected pairs: {len(rejected)} · Mean rotation error: {mean_rotation_error:.2f}°</text>
</svg>''')

    write_pair_diagnostics(
        samples,
        output_path.with_name(output_path.stem + "_diagnostics.svg")
    )


if __name__ == "__main__":
    main()
