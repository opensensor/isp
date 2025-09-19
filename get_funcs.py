import re
import os
import sys
from pathlib import Path

def get_function_names_from_c_files(file_paths):
    """
    Extract all function names defined across multiple .c files.

    Args:
        file_paths: List of file paths or a directory path containing .c files

    Returns:
        set: Set of function names found in the C files
    """
    function_names = set()

    # If a single path is provided and it's a directory, get all .c files
    if isinstance(file_paths, (str, Path)) and os.path.isdir(file_paths):
        file_paths = list(Path(file_paths).glob("*.c"))

    # Regex pattern to match C function definitions
    function_pattern = re.compile(
        r'^(?:static\s+|extern\s+|inline\s+)*'  # Optional storage class specifiers
        r'(?:const\s+|volatile\s+)*'             # Optional type qualifiers
        r'(?:\w+\s*\*?\s+)'                      # Return type (including pointers)
        r'(\w+)\s*\('                           # Function name (captured)
        r'[^)]*\)\s*\{'                         # Parameters and opening brace
        , re.MULTILINE
    )

    for file_path in file_paths:
        try:
            with open(file_path, 'r', encoding='utf-8') as file:
                content = file.read()

                # Remove comments to avoid false matches
                content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
                content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)

                # Find all function names
                matches = function_pattern.findall(content)
                function_names.update(matches)

        except (IOError, UnicodeDecodeError) as e:
            print(f"Warning: Could not read file {file_path}: {e}")
            continue

    return function_names

# Example usage:
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python get_funcs.py <directory_or_file_path>")
        sys.exit(1)

    path = sys.argv[1]
    functions = get_function_names_from_c_files(path)

    print(f"Found {len(functions)} unique functions:")
    for func in sorted(functions):
        print(f"  {func}")