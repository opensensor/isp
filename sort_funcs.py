import sys
import os

def sort_functions_file(input_file, output_file=None):
    """
    Sort function names in a text file alphabetically.

    Args:
        input_file: Path to the input text file containing function names
        output_file: Optional path for output file. If None, overwrites input file.
    """
    try:
        # Read the function names
        with open(input_file, 'r', encoding='utf-8') as file:
            lines = file.readlines()

        # Strip whitespace and filter out empty lines
        functions = [line.strip() for line in lines if line.strip()]

        # Sort alphabetically (case-insensitive)
        functions.sort(key=str.lower)

        # Determine output file
        if output_file is None:
            output_file = input_file

        # Write sorted functions
        with open(output_file, 'w', encoding='utf-8') as file:
            for func in functions:
                file.write(func + '\n')

        print(f"Sorted {len(functions)} functions")
        if output_file != input_file:
            print(f"Output written to: {output_file}")
        else:
            print(f"File updated: {input_file}")

    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found")
        sys.exit(1)
    except IOError as e:
        print(f"Error reading/writing file: {e}")
        sys.exit(1)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python sort_functions.py <input_file> [output_file]")
        print("  input_file:  Text file containing function names to sort")
        print("  output_file: Optional output file (default: overwrites input)")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2] if len(sys.argv) > 2 else None

    sort_functions_file(input_file, output_file)