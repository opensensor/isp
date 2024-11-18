#!/bin/bash

set -e  # Exit immediately on error

# Define variables
DRIVER_DIR="$(pwd)"
PATCH_FILE="$DRIVER_DIR/sdk-patch.diff"
SUBMODULE_DIR="$DRIVER_DIR/external/ingenic-sdk"
SDK_SRC_DIR="$SUBMODULE_DIR/3.10/isp/t31"
INCLUDE_DIR="$SDK_SRC_DIR/include"

# Step 1: Initialize and update the submodule
echo "Initializing and updating submodule..."
cd external/ingenic-sdk
git checkout .
cd ../..
git submodule init
git submodule update --remote --checkout

# Step 2: Apply the patch within the submodule directory
if [ -f "$PATCH_FILE" ]; then
    echo "Applying patch from $PATCH_FILE..."
    (cd "$SUBMODULE_DIR" && git apply "$PATCH_FILE")
else
    echo "Error: Patch file not found at $PATCH_FILE"
    exit 1
fi

# Step 3: Copy .c files to the kernel source directory
echo "Copying .c files to $SDK_SRC_DIR..."
mkdir -p "$SDK_SRC_DIR"
cp "$DRIVER_DIR"/driver/*.c "$SDK_SRC_DIR/"

# Step 4: Copy .h files to the include directory
echo "Copying .h files to $INCLUDE_DIR..."
mkdir -p "$INCLUDE_DIR"
cp "$DRIVER_DIR"/driver/include/*.h "$INCLUDE_DIR/"

echo "Operation completed successfully!"
echo "Now you should be able to build using SENSOR_MODEL=sc2336 ./build.sh t31 3.10" 

