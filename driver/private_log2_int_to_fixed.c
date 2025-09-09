#include "include/main.h"

uint32_t private_log2_int_to_fixed(const uint32_t val, const uint8_t out_precision, const uint8_t shift_out)
{
    // Return 0 for log2(0) which is undefined
    if (val == 0) {
        return 0;
    }

    // Find the position of the most significant bit (integer part of log2)
    int32_t msb_position = private_leading_one_position(val);

    // Normalize the input value to a fixed-point format
    // We want to shift the value so the MSB is at a fixed position (bit 15)
    uint32_t normalized;
    if (msb_position >= 16) {
        // Shift right if MSB is at position 16 or higher
        normalized = val >> ((msb_position - 15) & 0x1f);
    } else {
        // Shift left if MSB is below position 16
        normalized = val << ((15 - msb_position) & 0x1f);
    }

    // Calculate the fractional part using iterative approximation
    // This uses a form of binary search or CORDIC-like algorithm
    uint32_t fractional_part = 0;
    uint32_t x = normalized;

    for (uint32_t i = 0; i < out_precision; i++) {
        // Shift the fractional accumulator left
        fractional_part <<= 1;

        // Square the normalized value (in fixed-point arithmetic)
        uint64_t x_squared = ((uint64_t)x * x);

        // Check if we need to set the current bit
        if (x_squared >= (1ULL << 30)) {
            // MSB of squared result is set, shift right by 15
            x = x_squared >> 15;
        } else {
            // MSB not set, set current bit and shift right by 16
            fractional_part |= 1;
            x = x_squared >> 16;
        }
    }

    // Combine integer and fractional parts with appropriate shifting
    // Integer part is shifted left by out_precision bits
    // Fractional part is added
    // Then the whole result is shifted by shift_out
    uint32_t result = ((msb_position << out_precision) + fractional_part);

    // Apply the final output shift
    if (shift_out > 0) {
        result <<= shift_out;
    }

    // If there's remaining precision in x, we might want to round
    // by incorporating the upper bits of x
    if (shift_out < 15) {
        result |= (x & 0x7fff) >> ((15 - shift_out) & 0x1f);
    }

    return result;
}