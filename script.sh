#!/bin/bash

# ISP Build Automation Script
# This script automates the process of pulling updates, copying driver files, and building

set -e  # Exit on any error

export CROSS_COMPILE=mipsel-linux-
export KDIR=/home/matteius/output/wyze_cam3_t31x_gc2053_rtl8189ftv/build/linux-4fb8bc9f91c2951629f818014b7d3b5cc2a1ec81/
export PATH=/home/matteius/output/wyze_cam3_t31x_gc2053_rtl8189ftv/per-package/toolchain-external-custom/host/bin/:$PATH


# Configuration
SENSOR_MODEL="${SENSOR_MODEL:-gc2053}"  # Default sensor model, can be overridden
TARGET="${TARGET:-t31}"                 # Default target, can be overridden
REMOTE_HOST="${REMOTE_HOST:-192.168.50.211}"  # Default remote host
REMOTE_PATH="${REMOTE_PATH:-/tmp/}"     # Default remote path

# Determine if we're in SDK directory or ISP root
if [[ -f "build.sh" && -d "3.10/isp" ]]; then
    # We're in the SDK directory
    ISP_ROOT="../.."
    SDK_DIR="."
elif [[ -d "driver" && -d "external/ingenic-sdk" ]]; then
    # We're in the ISP root directory
    ISP_ROOT="."
    SDK_DIR="external/ingenic-sdk"
else
    print_error "Script must be run from either isp/ root directory or isp/external/ingenic-sdk/ directory"
    exit 1
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to upload kernel modules
upload_modules() {
    print_status "Uploading kernel modules to ${REMOTE_HOST}:${REMOTE_PATH}..."
    
    # Navigate to SDK directory if not already there
    cd "${SDK_DIR}"
    
    # Check if the required .ko files exist
    local ko_files=("tx-isp-${TARGET}.ko" "sensor_${SENSOR_MODEL}_${TARGET}.ko")
    local missing_files=()
    
    for file in "${ko_files[@]}"; do
        if [[ ! -f "$file" ]]; then
            missing_files+=("$file")
        fi
    done
    
    if [[ ${#missing_files[@]} -gt 0 ]]; then
        print_error "Missing kernel module files:"
        for file in "${missing_files[@]}"; do
            print_error "  - $file"
        done
        print_error "Make sure the build completed successfully"
        exit 1
    fi
    
    # Upload the files
    if scp -O "${ko_files[@]}" "root@${REMOTE_HOST}:${REMOTE_PATH}"; then
        print_status "Kernel modules uploaded successfully"
    else
        print_error "Failed to upload kernel modules"
        exit 1
    fi
    
    # Return to original directory
    cd - > /dev/null
}

# Function to pull latest changes (only if in ISP root)
pull_updates() {
    
    print_status "Pulling latest changes from git..."
    if git pull; then
        print_status "Git pull completed successfully"
    else
        print_error "Git pull failed"
        exit 1
    fi
}

# Function to copy driver files (only if in ISP root)
copy_drivers() {
    print_status "Copying driver files to external/ingenic-sdk/3.10/isp/${TARGET}/"
    
    if cp -rf driver/* "external/ingenic-sdk/3.10/isp/${TARGET}/"; then
        print_status "Driver files copied successfully"
    else
        print_error "Failed to copy driver files"
        exit 1
    fi
}

# Function to build the project
build_project() {
    print_status "Building project with SENSOR_MODEL=${SENSOR_MODEL} for target ${TARGET}..."
    
    cd "${SDK_DIR}"
    
    if SENSOR_MODEL="${SENSOR_MODEL}" ./build.sh "${TARGET}"; then
        print_status "Build completed successfully!"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Main execution
main() {
    print_status "Starting ISP build automation..."
    print_status "Sensor Model: ${SENSOR_MODEL}"
    print_status "Target: ${TARGET}"
    print_status "Remote Host: ${REMOTE_HOST}"
    
    cd ../..
    pull_updates
    copy_drivers
    cd external/ingenic-sdk
    
    build_project
    upload_modules
    
    print_status "All operations completed successfully!"
    print_status "Kernel modules have been uploaded to ${REMOTE_HOST}:${REMOTE_PATH}"
}

# Help function
show_help() {
    echo "ISP Build Automation Script"
    echo ""
    echo "Usage: $0 [options]"
    echo ""
    echo "This script can be run from either:"
    echo "  - isp/ root directory (full workflow: git pull + copy + build + upload)"
    echo "  - isp/external/ingenic-sdk/ directory (build + upload only)"
    echo ""
    echo "Options:"
    echo "  -s, --sensor MODEL    Set sensor model (default: gc2053)"
    echo "  -t, --target TARGET   Set build target (default: t31)"
    echo "  -r, --remote HOST     Set remote host IP (default: 192.168.50.211)"
    echo "  -p, --path PATH       Set remote path (default: /tmp/)"
    echo "  -h, --help           Show this help message"
    echo ""
    echo "Environment variables:"
    echo "  SENSOR_MODEL         Override default sensor model"
    echo "  TARGET               Override default target"
    echo "  REMOTE_HOST          Override default remote host"
    echo "  REMOTE_PATH          Override default remote path"
    echo ""
    echo "Examples:"
    echo "  $0                           # Use all defaults"
    echo "  $0 -s gc4653 -t t21         # Custom sensor and target"
    echo "  $0 -r 192.168.1.100         # Custom remote host"
    echo "  SENSOR_MODEL=ov2735 $0       # Using environment variable"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -s|--sensor)
            SENSOR_MODEL="$2"
            shift 2
            ;;
        -t|--target)
            TARGET="$2"
            shift 2
            ;;
        -r|--remote)
            REMOTE_HOST="$2"
            shift 2
            ;;
        -p|--path)
            REMOTE_PATH="$2"
            shift 2
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Run main function
main
