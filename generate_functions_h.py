#!/usr/bin/env python3
"""
Script to generate a proper functions.h file by parsing function definitions from .c files
"""

import os
import re
import sys
from typing import List, Set, Dict, Tuple

def extract_function_signature(file_content: str, filename: str) -> List[Tuple[str, str]]:
    """
    Extract function signatures from C file content.
    Returns list of tuples (return_type, function_declaration)
    """
    functions = []
    
    # Pattern to match function definitions
    # This matches: return_type function_name(parameters) followed by opening brace
    function_pattern = r'^\s*([a-zA-Z_][a-zA-Z0-9_*\s]*?)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(\s*([^)]*)\)\s*\{'
    
    lines = file_content.split('\n')
    
    for i, line in enumerate(lines):
        # Skip lines that are comments, preprocessor directives, or inside comments
        line = line.strip()
        if (line.startswith('//') or line.startswith('#') or 
            line.startswith('/*') or line.startswith('*') or 
            not line or line.startswith('}')):
            continue
            
        # Look for function definitions that might span multiple lines
        combined_line = line
        j = i + 1
        
        # If line doesn't contain opening brace, check next few lines
        if '{' not in combined_line and j < len(lines):
            while j < len(lines) and j < i + 5:  # Look ahead max 5 lines
                next_line = lines[j].strip()
                if next_line and not next_line.startswith('//'):
                    combined_line += ' ' + next_line
                    if '{' in next_line:
                        break
                j += 1
        
        match = re.match(function_pattern, combined_line, re.MULTILINE)
        if match:
            return_type = match.group(1).strip()
            function_name = match.group(2).strip()
            params = match.group(3).strip()
            
            # Skip if it looks like a struct/enum definition
            if return_type in ['struct', 'enum', 'typedef', 'union']:
                continue
                
            # Skip if return type contains keywords that indicate it's not a function
            if any(keyword in return_type.lower() for keyword in ['if', 'while', 'for', 'switch', 'case']):
                continue
            
            # Clean up return type
            return_type = re.sub(r'\s+', ' ', return_type).strip()
            
            # Clean up parameters
            if not params or params.strip() == '':
                params = 'void'
            else:
                # Clean up parameter formatting
                params = re.sub(r'\s+', ' ', params).strip()
            
            # Create the function declaration
            declaration = f"{return_type} {function_name}({params})"
            functions.append((return_type, declaration))
    
    return functions

def scan_c_files(directory: str) -> Dict[str, List[Tuple[str, str]]]:
    """
    Scan directory for .c files and extract function signatures.
    Returns dict mapping filename to list of function signatures.
    """
    c_files_functions = {}
    
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.c'):
                filepath = os.path.join(root, file)
                try:
                    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                    
                    functions = extract_function_signature(content, file)
                    if functions:
                        c_files_functions[file] = functions
                        print(f"Found {len(functions)} functions in {file}")
                
                except Exception as e:
                    print(f"Error reading {filepath}: {e}")
                    continue
    
    return c_files_functions

def generate_functions_h(c_files_functions: Dict[str, List[Tuple[str, str]]], output_file: str):
    """
    Generate the functions.h file from extracted function signatures.
    """
    # Collect all unique function declarations
    all_functions = set()
    function_sources = {}  # Track which file each function came from
    
    for filename, functions in c_files_functions.items():
        for return_type, declaration in functions:
            # Extract function name for deduplication
            func_name_match = re.search(r'\b([a-zA-Z_][a-zA-Z0-9_]*)\s*\(', declaration)
            if func_name_match:
                func_name = func_name_match.group(1)
                all_functions.add(declaration)
                if func_name not in function_sources:
                    function_sources[func_name] = []
                function_sources[func_name].append(filename)
    
    # Sort functions alphabetically
    sorted_functions = sorted(list(all_functions))
    
    # Generate the header file content
    header_content = """/* Function Forward Declarations for tx-isp-t31.ko */
/* Auto-generated from .c files - DO NOT EDIT MANUALLY */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

/* Standard includes for common types */
#include <linux/types.h>
#include <linux/kernel.h>

/* Forward declarations for common structures */
struct clk;
struct device;
struct platform_device;
struct i2c_driver;
struct i2c_client;
struct i2c_adapter;
struct i2c_msg;
struct spi_driver;
struct spi_device;
struct v4l2_subdev;
struct tx_isp_subdev;
struct file;
struct inode;
struct dentry;
struct mutex;
struct spinlock;
struct work_struct;

/* Generic enums for GPIO and other subsystems */
enum gpio_port { GPIO_PORT_A, GPIO_PORT_B, GPIO_PORT_C, GPIO_PORT_D };
enum gpio_function { GPIO_FUNC_0, GPIO_FUNC_1, GPIO_FUNC_2, GPIO_FUNC_3 };

/* Function Declarations */
"""
    
    # Add function declarations
    for declaration in sorted_functions:
        # Convert declaration to forward declaration by adding semicolon
        if not declaration.endswith(';'):
            declaration += ';'
        header_content += declaration + '\n'
    
    header_content += f"""
/* Total functions declared: {len(sorted_functions)} */
/* Generated from {len(c_files_functions)} .c files */

#endif /* FUNCTIONS_H */
"""
    
    # Write to output file
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(header_content)
    
    print(f"Generated {output_file} with {len(sorted_functions)} function declarations")
    
    # Print summary of sources
    print("\nSummary by source file:")
    for filename, functions in c_files_functions.items():
        print(f"  {filename}: {len(functions)} functions")

def main():
    # Directory containing the .c files
    driver_dir = "driver"
    
    if not os.path.exists(driver_dir):
        print(f"Error: Directory '{driver_dir}' not found")
        sys.exit(1)
    
    print(f"Scanning {driver_dir} for .c files...")
    c_files_functions = scan_c_files(driver_dir)
    
    if not c_files_functions:
        print("No .c files with functions found")
        sys.exit(1)
    
    print(f"Found functions in {len(c_files_functions)} .c files")
    
    # Generate the new functions.h
    output_file = "driver/include/functions.h"
    generate_functions_h(c_files_functions, output_file)
    
    print(f"Successfully generated {output_file}")

if __name__ == "__main__":
    main()
