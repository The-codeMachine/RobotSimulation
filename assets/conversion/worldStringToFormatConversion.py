import argparse
from pathlib import Path

OBJECT_TYPES = {
    " ": 0,  # EMPTY
    "#": 1,  # WALL
    "Robot": 2   # ROBOT
}

def convert_world_file(input_path: str, output_path: str) -> None:
    # Read the world grid from the input text file
    world = Path(input_path).read_text().splitlines()
    
    output = []
    for y, row in enumerate(world):
        for x, char in enumerate(row):
            if char not in OBJECT_TYPES:
                raise ValueError(f"Unknown character: '{char}' at ({x}, {y})")
            output.append(f"{x},{y},0,{OBJECT_TYPES[char]}")
            
    Path(output_path).write_text(";".join(output) + ";")
    print(f"Successfully converted {input_path} to {output_path}")

if __name__ == "__main__":
    # Set up command-line argument parsing
    parser = argparse.ArgumentParser(description="Convert a text grid world into a coordinate-mapped file.")
    
    # Define positional arguments
    parser.add_argument("input_file", help="Path to the source text file containing the map grid.")
    parser.add_argument("output_file", help="Path where the converted coordinate file will be saved.")
    
    # Parse the arguments from the terminal
    args = parser.parse_args()
    
    # Run the conversion
    convert_world_file(args.input_file, args.output_file)
