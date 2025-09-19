import re
import os
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
    # Matches: return_type function_name(parameters) {
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
    # Method 1: Pass a list of specific files
    files = ["file1.c", "file2.c", "file3.c"]
    functions = get_function_names_from_c_files(files)

    # Method 2: Pass a directory path (will find all .c files)
    # functions = get_function_names_from_c_files("./src")

    print(f"Found {len(functions)} unique functions:")
    for func in sorted(functions):
        print(f"  {func}")