#!/bin/bash

set -e  # Exit on error

# Define variables
DRIVER_DIR="$(pwd)"
PATCH_FILE="$DRIVER_DIR/ingenic-sdk.patch"
SUBMODULE_DIR="$DRIVER_DIR/external/ingenic-sdk"
KERNEL_SRC_DIR="$SUBMODULE_DIR/3.10/isp/t31"
INCLUDE_DIR="$KERNEL_SRC_DIR/include"

# Step 1: Initialize and update the submodule
echo "Initializing and updating submodule..."
git submodule init
git submodule update --remote --checkout

# Step 2: Apply the patch
if [ -f "$PATCH_FILE" ]; then
    echo "Applying patch from $PATCH_FILE..."
    (cd "$SUBMODULE_DIR" && git apply "$PATCH_FILE")
else
    echo "Patch file not found: $PATCH_FILE"
    exit 1
fi

# Step 3: Copy .c and .h files from the driver directory to the submodule
echo "Copying source files..."
cp "$DRIVER_DIR"/*.c "$KERNEL_SRC_DIR/"
cp "$DRIVER_DIR"/*.h "$INCLUDE_DIR/"

echo "Operation completed successfully!"
