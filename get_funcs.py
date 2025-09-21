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
    elif isinstance(file_paths, (str, Path)):
        file_paths = [file_paths]

    for file_path in file_paths:
        try:
            with open(file_path, 'r', encoding='utf-8') as file:
                content = file.read()

                # Remove comments to avoid false matches
                # Remove multi-line comments
                content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
                # Remove single-line comments
                content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)

                # Remove string literals to avoid false matches
                content = re.sub(r'"(?:[^"\\]|\\.)*"', '""', content)
                content = re.sub(r"'(?:[^'\\]|\\.)*'", "''", content)

                # Find all function names using multiple patterns
                functions = extract_functions(content)
                function_names.update(functions)

        except (IOError, UnicodeDecodeError) as e:
            print(f"Warning: Could not read file {file_path}: {e}")
            continue

    return function_names

def extract_functions(content):
    """
    Extract function names from C source code content using multiple patterns.
    """
    functions = set()

    # Pattern 1: Most common C function pattern
    # Matches: int func_name(params) { or int func_name(params)\n{
    pattern1 = re.compile(
        r'^\s*'                                   # Optional leading whitespace
        r'(?:static\s+|extern\s+|inline\s+|const\s+|volatile\s+|unsigned\s+|signed\s+)*'  # Optional modifiers
        r'(?:struct\s+\w+\s*\*?\s+|'             # struct return type
        r'enum\s+\w+\s*\*?\s+|'                  # enum return type  
        r'union\s+\w+\s*\*?\s+|'                 # union return type
        r'(?:void|int|char|short|long|float|double|size_t|ssize_t|uint\d+_t|int\d+_t|bool)\s*\*?\s+|'  # Common types
        r'\w+\s*\*?\s+)+'                        # Custom types (at least one required)
        r'(\w+)'                                  # Function name (captured)
        r'\s*\([^;]*?\)'                         # Parameters (can span lines)
        r'[^;]*?\s*\{'                           # Opening brace (not semicolon)
        , re.MULTILINE | re.DOTALL
    )

    # Pattern 2: Function with pointer return type
    # Specifically handles: struct tx_isp_subdev *func_name(...)
    pattern2 = re.compile(
        r'^\s*'
        r'(?:static\s+|extern\s+|inline\s+)*'
        r'(?:struct\s+|enum\s+|union\s+)?'
        r'(\w+\s+)*'                             # Type name(s)
        r'\*+\s*'                                # Pointer(s)
        r'(\w+)'                                  # Function name
        r'\s*\([^;]*?\)'
        r'[^;]*?\s*\{'
        , re.MULTILINE | re.DOTALL
    )

    # Pattern 3: Simple pattern for basic functions
    # Handles: int func(void) {
    pattern3 = re.compile(
        r'^[a-zA-Z_][\w\s\*]+\s+'                # Return type
        r'([a-zA-Z_]\w*)'                        # Function name
        r'\s*\([^)]*\)'                          # Parameters
        r'\s*\{'                                 # Opening brace
        , re.MULTILINE
    )

    # Pattern 4: Very simple pattern as fallback
    # Just looks for word(anything){
    pattern4 = re.compile(
        r'\b(\w+)\s*\([^;)]*\)\s*\{',
        re.MULTILINE | re.DOTALL
    )

    # Apply all patterns
    for i, pattern in enumerate([pattern1, pattern2, pattern3, pattern4], 1):
        matches = pattern.findall(content)
        if matches:
            # For pattern2, we need the last captured group
            if i == 2:
                matches = [m[-1] if isinstance(m, tuple) else m for m in matches]
            functions.update(matches)

    # Filter out common false positives
    false_positives = {'if', 'while', 'for', 'switch', 'return', 'sizeof',
                       'typedef', 'else', 'do', 'catch', 'try', 'throw'}
    functions = {f for f in functions if f not in false_positives and f.isidentifier()}

    return functions

def get_function_declarations(file_paths):
    """
    Also extract function declarations (prototypes) from header files.
    """
    declarations = set()

    # If a single path is provided and it's a directory, get all .h files
    if isinstance(file_paths, (str, Path)) and os.path.isdir(file_paths):
        file_paths = list(Path(file_paths).glob("*.h"))
    elif isinstance(file_paths, (str, Path)):
        file_paths = [file_paths]

    declaration_pattern = re.compile(
        r'(?:static\s+|extern\s+|inline\s+)*'
        r'(?:\w+\s*\*?\s+)+'                     # Return type
        r'(\w+)\s*\([^)]*\)\s*;'                 # Function name with semicolon
        , re.MULTILINE
    )

    for file_path in file_paths:
        try:
            with open(file_path, 'r', encoding='utf-8') as file:
                content = file.read()

                # Remove comments
                content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
                content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)

                matches = declaration_pattern.findall(content)
                declarations.update(matches)

        except (IOError, UnicodeDecodeError) as e:
            print(f"Warning: Could not read file {file_path}: {e}")
            continue

    return declarations

# Example usage:
if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python get_funcs.py <directory_or_file_path> [--include-headers] [--debug]")
        sys.exit(1)

    path = sys.argv[1]
    include_headers = '--include-headers' in sys.argv
    debug = '--debug' in sys.argv

    # Get function definitions from .c files
    functions = get_function_names_from_c_files(path)

    # Debug mode: show what was processed
    if debug:
        print("\n=== DEBUG MODE ===")
        if os.path.isfile(path):
            with open(path, 'r', encoding='utf-8') as f:
                content = f.read()
                # Remove comments
                content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
                content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)

                print(f"File size: {len(content)} characters after comment removal")

                # Look for potential function definitions
                potential_funcs = re.findall(r'\b(\w+)\s*\([^;)]*\)\s*\{', content, re.MULTILINE | re.DOTALL)
                print(f"Potential functions found by simple pattern: {potential_funcs}")

                # Check specifically for vic_core_ops_ioctl
                if 'vic_core_ops_ioctl' in content:
                    print("✓ vic_core_ops_ioctl found in file content")
                    # Find the line it's on
                    for i, line in enumerate(content.split('\n'), 1):
                        if 'vic_core_ops_ioctl' in line and '(' in line:
                            print(f"  Found on line {i}: {line.strip()}")
                            # Show context
                            lines = content.split('\n')
                            start = max(0, i-3)
                            end = min(len(lines), i+2)
                            print("  Context:")
                            for j in range(start, end):
                                print(f"    {j+1}: {lines[j]}")
                else:
                    print("✗ vic_core_ops_ioctl NOT found in file content")

    # Optionally get function declarations from .h files
    if include_headers and os.path.isdir(path):
        declarations = get_function_declarations(path)
        print(f"\nFound {len(declarations)} function declarations in headers:")
        for decl in sorted(declarations):
            print(f"  {decl}")

    print(f"\nFound {len(functions)} unique function definitions:")
    for func in sorted(functions):
        print(f"  {func}")

    # Check if vic_core_ops_ioctl was found
    if 'vic_core_ops_ioctl' not in functions:
        print("\n⚠️  vic_core_ops_ioctl was NOT found in the results!")
    else:
        print("\n✓ vic_core_ops_ioctl was successfully found!")