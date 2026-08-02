#!/usr/bin/env python3

import argparse
import json
import sys
from pathlib import Path


# World string glyph -> JSON object type
GLYPH_TO_TYPE = {
    "#": "Wall",
    "R": "Robot",
}

WORLD_FILE_VERSION = 1


def world_to_json(input_path: Path, output_path: Path) -> None:
    try:
        with input_path.open("r", encoding="utf-8") as file:
            world_string = file.read()
    except FileNotFoundError:
        raise RuntimeError(f"Input file does not exist: {input_path}")
    except OSError as e:
        raise RuntimeError(f"Could not read input file: {e}")

    # Remove only trailing newlines.
    world_string = world_string.rstrip("\r\n")

    if not world_string:
        raise RuntimeError("World file is empty")

    rows = world_string.splitlines()

    if not rows:
        raise RuntimeError("World file contains no rows")

    row_size = len(rows[0])
    row_amount = len(rows)

    if row_size == 0:
        raise RuntimeError("World contains an empty row")

    # Make sure the world is rectangular.
    for y, row in enumerate(rows):
        if len(row) != row_size:
            raise RuntimeError(
                f"Invalid world: row {y} has length {len(row)}, "
                f"expected {row_size}"
            )

    objects = []

    for y, row in enumerate(rows):
        for x, glyph in enumerate(row):

            # Spaces represent Empty objects, which are not serialized
            # by World::serialize_().
            if glyph == " ":
                continue

            if glyph not in GLYPH_TO_TYPE:
                raise RuntimeError(
                    f"Unknown glyph '{glyph}' at position ({x}, {y})"
                )

            object_type = GLYPH_TO_TYPE[glyph]

            obj = {
                "type": object_type,
                "glyph": glyph,
                "transform": {
                    "x": x,
                    "y": y,
                    "rotation": 0
                }
            }

            # Deliberately do NOT add Robot data/devices.
            objects.append(obj)

    world = {
        "version": WORLD_FILE_VERSION,
        "ROW_SIZE": row_size,
        "ROW_AMOUNT": row_amount,
        "objects": objects
    }

    try:
        with output_path.open("w", encoding="utf-8", newline="\n") as file:
            json.dump(world, file, indent=4)
            file.write("\n")
    except OSError as e:
        raise RuntimeError(f"Could not write output file: {e}")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Convert a World text file into a World JSON file."
    )

    parser.add_argument(
        "input",
        type=Path,
        help="Input .txt file"
    )

    parser.add_argument(
        "output",
        type=Path,
        help="Output .json file"
    )

    args = parser.parse_args()

    if args.input.suffix.lower() != ".txt":
        print("Warning: input file does not have a .txt extension",
              file=sys.stderr)

    if args.output.suffix.lower() != ".json":
        print("Warning: output file does not have a .json extension",
              file=sys.stderr)

    try:
        world_to_json(args.input, args.output)
    except RuntimeError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

    print(f"Converted {args.input} -> {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())