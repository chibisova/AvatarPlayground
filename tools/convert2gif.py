from PIL import Image, ImageDraw
from pathlib import Path
import subprocess
import tempfile


FRAME_DIR = Path("assets/demo/trajectory_frames")
VIDEO_PATH = Path("assets/demo/spatial_mapping_demo.mp4")
OUTPUT_PATH = Path("assets/demo/spatial_mapping_demo.gif")

TARGET_HEIGHT = 360
FPS = 5


def resize_to_height(image, height):
    scale = height / image.height
    width = int(image.width * scale)

    return image.resize(
        (width, height),
        Image.Resampling.LANCZOS
    )


def add_label(image, text):
    draw = ImageDraw.Draw(image)

    draw.rectangle(
        (0, 0, image.width, 32),
        fill=(0, 0, 0)
    )

    draw.text(
        (10, 8),
        text,
        fill=(255, 255, 255)
    )

    return image


# --------------------------------------------------
# Read exact synchronization mapping
# --------------------------------------------------

mapping_path = (
    FRAME_DIR / "frame_mapping.txt"
)

if not mapping_path.exists():
    raise RuntimeError(
        f"Missing mapping file: {mapping_path}"
    )


mapping = {}

with open(mapping_path, "r") as file:
    for line in file:
        line = line.strip()

        if not line:
            continue

        gif_index, video_frame = map(
            int,
            line.split()
        )

        mapping[gif_index] = video_frame


print(
    f"Synchronization entries: {len(mapping)}"
)


# --------------------------------------------------
# Find reconstruction frames
# --------------------------------------------------

reconstruction_frames = sorted(
    FRAME_DIR.glob("frame_*.png")
)

if not reconstruction_frames:
    raise RuntimeError(
        "No reconstruction frames found."
    )


# --------------------------------------------------
# Extract exact source frames with FFmpeg
# --------------------------------------------------

with tempfile.TemporaryDirectory() as temp_dir:

    temp_dir = Path(temp_dir)

    print(
        "Extracting synchronized source frames..."
    )

    # Extract every frame from the source video.
    #
    # This preserves the original frame numbering,
    # which is important because frame_mapping.txt
    # stores actual video frame indices.
    subprocess.run(
        [
            "ffmpeg",
            "-y",
            "-i",
            str(VIDEO_PATH),
            "-vsync",
            "0",
            str(temp_dir / "source_%06d.png"),
        ],
        check=True
    )

    source_frames = sorted(
        temp_dir.glob("source_*.png")
    )

    print(
        f"Source frames extracted: "
        f"{len(source_frames)}"
    )


    # --------------------------------------------------
    # Build synchronized three-view frames
    # --------------------------------------------------

    combined_frames = []

    for reconstruction_path in reconstruction_frames:

        gif_index = int(
            reconstruction_path.stem.split("_")[-1]
        )

        if gif_index not in mapping:
            print(
                f"Skipping {reconstruction_path.name}: "
                f"no mapping entry"
            )
            continue


        overview_path = (
            FRAME_DIR /
            f"overview_{gif_index:05d}.png"
        )

        if not overview_path.exists():
            print(
                f"Skipping {reconstruction_path.name}: "
                f"missing {overview_path.name}"
            )
            continue


        # --------------------------------------------------
        # Exact source video frame
        # --------------------------------------------------

        video_frame_index = mapping[gif_index]

        # FFmpeg frame numbering starts at 1,
        # while OpenCV frameNumber starts at 0.
        source_path = (
            temp_dir /
            f"source_{video_frame_index + 1:06d}.png"
        )

        if not source_path.exists():
            print(
                f"Skipping GIF frame {gif_index}: "
                f"source frame {video_frame_index} "
                f"not found"
            )
            continue


        # --------------------------------------------------
        # Load images
        # --------------------------------------------------

        source_image = Image.open(
            source_path
        ).convert("RGB")

        trajectory_image = Image.open(
            reconstruction_path
        ).convert("RGB")

        overview_image = Image.open(
            overview_path
        ).convert("RGB")


        # --------------------------------------------------
        # Resize
        # --------------------------------------------------

        source_image = resize_to_height(
            source_image,
            TARGET_HEIGHT
        )

        trajectory_image = resize_to_height(
            trajectory_image,
            TARGET_HEIGHT
        )

        overview_image = resize_to_height(
            overview_image,
            TARGET_HEIGHT
        )


        # --------------------------------------------------
        # Labels
        # --------------------------------------------------

        source_image = add_label(
            source_image,
            "SOURCE VIDEO"
        )

        trajectory_image = add_label(
            trajectory_image,
            "TRAJECTORY VIEW"
        )

        overview_image = add_label(
            overview_image,
            "3D OVERVIEW"
        )


        # --------------------------------------------------
        # Combine horizontally
        # --------------------------------------------------

        total_width = (
            source_image.width +
            trajectory_image.width +
            overview_image.width
        )

        combined = Image.new(
            "RGB",
            (
                total_width,
                TARGET_HEIGHT
            )
        )

        x = 0

        combined.paste(
            source_image,
            (x, 0)
        )

        x += source_image.width

        combined.paste(
            trajectory_image,
            (x, 0)
        )

        x += trajectory_image.width

        combined.paste(
            overview_image,
            (x, 0)
        )

        combined_frames.append(
            combined
        )


# --------------------------------------------------
# Save GIF
# --------------------------------------------------

if not combined_frames:
    raise RuntimeError(
        "No synchronized frames were generated."
    )


OUTPUT_PATH.parent.mkdir(
    parents=True,
    exist_ok=True
)

combined_frames[0].save(
    OUTPUT_PATH,
    save_all=True,
    append_images=combined_frames[1:],
    duration=int(1000 / FPS),
    loop=0,
    optimize=True
)


print()
print(
    f"Created: {OUTPUT_PATH}"
)

print(
    f"Frames: {len(combined_frames)}"
)

print(
    f"FPS: {FPS}"
)
