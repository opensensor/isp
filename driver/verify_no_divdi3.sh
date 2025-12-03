#!/bin/bash
# Verify that the compiled module doesn't reference __divdi3 or __udivdi3

set -e

MODULE_PATH="${1:-../external/ingenic-sdk/3.10/isp/tx-isp-t31.ko}"

if [ ! -f "$MODULE_PATH" ]; then
    echo "Error: Module not found at $MODULE_PATH"
    echo "Usage: $0 [path/to/tx-isp-t31.ko]"
    exit 1
fi

echo "Checking $MODULE_PATH for problematic division symbols..."
echo ""

# Check for __divdi3 and __udivdi3
if ${CROSS_COMPILE}nm "$MODULE_PATH" 2>/dev/null | grep -E "__divdi3|__udivdi3"; then
    echo ""
    echo "ERROR: Found problematic division symbols!"
    echo "The module still contains references to __divdi3 or __udivdi3"
    echo "This will cause 'Unknown symbol' errors on MIPS32"
    exit 1
else
    echo "SUCCESS: No __divdi3 or __udivdi3 symbols found!"
    echo "The module should load correctly on MIPS32"
    exit 0
fi

