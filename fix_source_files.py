#!/usr/bin/env python3
"""
Script to fix ISP driver source files:
1. Add #include <stdint.h> to all .c files
2. Fix function prototypes by adding (void) for parameterless functions
3. Extract function signatures and create a common header file
4. Add include for common header to all .c files
"""

import os
import re
import glob
from typing import List, Set, Tuple

def extract_function_signature(file_path: str) -> List[str]:
    """Extract function signatures from a C file."""
    signatures = []
    
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Pattern to match function definitions
        # Matches: return_type function_name(parameters) {
        pattern = r'^\s*(\w+(?:\s+\w+)*)\s+(\w+)\s*\([^)]*\)\s*\{'
        
        for match in re.finditer(pattern, content, re.MULTILINE):
            return_type = match.group(1).strip()
            func_name = match.group(2).strip()
            
            # Get the full function line to extract parameters
            line_start = content.rfind('\n', 0, match.start()) + 1
            line_end = content.find('\n', match.end())
            if line_end == -1:
                line_end = len(content)
            
            full_line = content[line_start:match.end()].strip()
            
            # Extract parameters part
            paren_start = full_line.find('(')
            paren_end = full_line.rfind(')')
            if paren_start != -1 and paren_end != -1:
                params = full_line[paren_start+1:paren_end].strip()
                
                # Clean up the signature for header file
                if not params or params == 'void':
                    signature = f"{return_type} {func_name}(void);"
                else:
                    signature = f"{return_type} {func_name}({params});"
                
                signatures.append(signature)
    
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
    
    return signatures

def fix_source_file(file_path: str, common_header: str) -> bool:
    """Fix a single C source file."""
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        # Check if already has stdint.h include
        has_stdint = '#include <stdint.h>' in content
        has_common_header = f'#include "{common_header}"' in content
        
        # Fix function prototypes - add (void) to parameterless functions
        # Pattern: function_name() -> function_name(void)
        content = re.sub(r'(\w+)\s*\(\s*\)\s*\{', r'\1(void) {', content)
        
        new_content = ""
        
        # Add includes at the top if not present
        if not has_stdint:
            new_content += "#include <stdint.h>\n"
        
        if not has_common_header:
            new_content += f'#include "{common_header}"\n'
        
        if new_content:
            new_content += "\n" + content
        else:
            new_content = content
        
        # Write back the modified content
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        return True
    
    except Exception as e:
        print(f"Error fixing {file_path}: {e}")
        return False

def create_common_header(signatures: Set[str], header_path: str):
    """Create a common header file with all function signatures."""
    header_guard = "ISP_DRIVER_COMMON_H"
    
    content = f"""#ifndef {header_guard}
#define {header_guard}

#include <stdint.h>

// Forward declarations for all ISP driver functions
"""
    
    # Sort signatures for better organization
    sorted_signatures = sorted(signatures)
    
    for signature in sorted_signatures:
        content += signature + "\n"
    
    content += f"""
#endif /* {header_guard} */
"""
    
    try:
        with open(header_path, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Created common header: {header_path}")
    except Exception as e:
        print(f"Error creating header {header_path}: {e}")

def main():
    """Main function to process all source files."""
    driver_dir = "driver"
    common_header = "isp_driver_common.h"
    common_header_path = os.path.join(driver_dir, common_header)
    
    if not os.path.exists(driver_dir):
        print(f"Error: {driver_dir} directory not found")
        return
    
    # Find all .c files
    c_files = glob.glob(os.path.join(driver_dir, "*.c"))
    
    if not c_files:
        print(f"No .c files found in {driver_dir}")
        return
    
    print(f"Found {len(c_files)} C files to process")
    
    # Extract all function signatures
    all_signatures = set()
    
    for c_file in c_files:
        print(f"Extracting signatures from {os.path.basename(c_file)}")
        signatures = extract_function_signature(c_file)
        all_signatures.update(signatures)
    
    print(f"Extracted {len(all_signatures)} unique function signatures")
    
    # Create common header file
    create_common_header(all_signatures, common_header_path)
    
    # Fix all source files
    fixed_count = 0
    for c_file in c_files:
        print(f"Fixing {os.path.basename(c_file)}")
        if fix_source_file(c_file, common_header):
            fixed_count += 1
    
    print(f"Successfully fixed {fixed_count} out of {len(c_files)} files")
    print(f"Common header created: {common_header}")
    print("All files should now compile without missing type and function declaration errors")

if __name__ == "__main__":
    main()
