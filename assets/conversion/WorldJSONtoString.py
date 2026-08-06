#!/usr/bin/env python3

import argparse
import json
import sys
from pathlib import Path

# JSON object type -> World string glyph
TYPE_TO_GLYPH = {
    "Wall": "#",
    "Robot": "R",
}


def json_to_world(input_path: Path, output_path: Path) -> None:
    try:
        with input_path.open("r", encoding="utf-8") as file:
            world = json.load(file)
    except FileNotFoundError:
        raise RuntimeError(f"Input file does not exist: {input_path}")
    except json.JSONDecodeError as e:
        raise RuntimeError(f"Invalid JSON: {e}")

    # Validate required fields.
    for field in ("version", "ROW_SIZE", "ROW_AMOUNT", "objects"):
        if field not in world:
            raise RuntimeError(f"JSON is missing required field: '{field}'")

    row_size = world["ROW_SIZE"]
    row_amount = world["ROW_AMOUNT"]

    if not isinstance(row_size, int) or row_size <= 0:
        raise RuntimeError("ROW_SIZE must be a positive integer")

    if not isinstance(row_amount, int) or row_amount <= 0:
        raise RuntimeError("ROW_AMOUNT must be a positive integer")

    # Start with an empty world.
    grid = [
        [" " for _ in range(row_size)]
        for _ in range(row_amount)
    ]

    for obj in world["objects"]:
        if not isinstance(obj, dict):
            raise RuntimeError("Every object must be a JSON object")

        if "type" not in obj:
            raise RuntimeError("Object is missing 'type'")

        if "transform" not in obj:
            raise RuntimeError(
                f"Object '{obj['type']}' is missing 'transform'"
            )

        object_type = obj["type"]
        transform = obj["transform"]

        if object_type not in TYPE_TO_GLYPH:
            raise RuntimeError(
                f"No glyph mapping exists for object type '{object_type}'"
            )

        # Validate the nested vector2 field.
        if not isinstance(transform, dict):
            raise RuntimeError(
                f"Object '{object_type}' has an invalid 'transform'"
            )

        if "vector2" not in transform:
            raise RuntimeError(
                f"Object '{object_type}' transform is missing 'vector2'"
            )

        vector2 = transform["vector2"]

        if not isinstance(vector2, dict):
            raise RuntimeError(
                f"Object '{object_type}' has an invalid 'vector2'"
            )

        for field in ("x", "y"):
            if field not in vector2:
                raise RuntimeError(
                    f"Object '{object_type}' transform.vector2 "
                    f"is missing '{field}'"
                )

        x = vector2["x"]
        y = vector2["y"]

        if not isinstance(x, int) or not isinstance(y, int):
            raise RuntimeError(
                f"Invalid position for '{object_type}': ({x}, {y})"
            )

        if not (0 <= x < row_size and 0 <= y < row_amount):
            raise RuntimeError(
                f"Object '{object_type}' is outside world bounds: "
                f"({x}, {y})"
            )

        if grid[y][x] != " ":
            raise RuntimeError(
                f"Multiple objects occupy position ({x}, {y})"
            )

        grid[y][x] = TYPE_TO_GLYPH[object_type]

    world_string = "\n".join("".join(row) for row in grid)

    try:
        with output_path.open(
            "w",
            encoding="utf-8",
            newline="\n"
        ) as file:
            file.write(world_string)
            file.write("\n")
    except OSError as e:
        raise RuntimeError(f"Could not write output file: {e}")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Convert a World JSON file into a World text file."
    )

    parser.add_argument(
        "input",
        type=Path,
        help="Input .json file"
    )

    parser.add_argument(
        "output",
        type=Path,
        help="Output .txt file"
    )

    args = parser.parse_args()

    if args.input.suffix.lower() != ".json":
        print(
            "Warning: input file does not have a .json extension",
            file=sys.stderr
        )

    if args.output.suffix.lower() != ".txt":
        print(
            "Warning: output file does not have a .txt extension",
            file=sys.stderr
        )

    try:
        json_to_world(args.input, args.output)
    except RuntimeError as e:
        print(f"Error: {e}", file=sys.stderr)
        return 1

    print(f"Converted {args.input} -> {args.output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
