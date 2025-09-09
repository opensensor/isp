#include "include/main.h"

// Assuming __pow2_lut is defined elsewhere as a lookup table for 2^x values
extern const uint32_t __pow2_lut[];

uint32_t private_math_exp2(uint32_t val, const unsigned char shift_in, const unsigned char shift_out)
{
    // Extract integer and fractional parts based on shift_in
    // shift_in indicates how many bits represent the fractional part
    uint32_t frac_mask = (1U << shift_in) - 1;
    uint32_t fractional_part = val & frac_mask;
    uint32_t integer_part = val >> shift_in;

    // For small shift_in values (< 6), use direct lookup without interpolation
    if (shift_in < 6) {
        // Scale up the fractional part to match the LUT granularity (32 entries = 5 bits)
        uint32_t lut_index = fractional_part << ((5 - shift_in) & 0x1f);
        uint32_t lut_value = __pow2_lut[lut_index];

        // Apply the integer part as a shift (2^integer_part) and output shift
        // The result is shifted right by (30 - shift_out - integer_part)
        return lut_value >> ((30 - shift_out - integer_part) & 0x1f);
    }

    // For larger shift_in values, use linear interpolation between LUT entries
    // Extract the upper 5 bits of fractional part for LUT indexing
    uint32_t lut_index = fractional_part >> ((shift_in - 5) & 0x1f);

    // Get the base value from the lookup table
    uint32_t lut_value_low = __pow2_lut[lut_index];
    uint32_t lut_value_high = __pow2_lut[lut_index + 1];

    // Calculate interpolation factor (lower bits of fractional part)
    uint32_t interp_mask = (1U << ((shift_in - 5) & 0x1f)) - 1;
    uint32_t interp_factor = fractional_part & interp_mask;

    // Perform linear interpolation
    // (high - low) * factor / scale + low
    uint32_t delta = lut_value_high - lut_value_low;

    // 64-bit multiplication for precision
    uint64_t product = (uint64_t)delta * interp_factor;

    // Shift right to normalize (equivalent to division by 2^(shift_in-5))
    uint32_t interpolated_delta = (uint32_t)(product >> ((shift_in - 5) & 0x1f));

    // Add the interpolated delta to the base value
    uint32_t interpolated_value = lut_value_low + interpolated_delta;

    // Apply the integer part as a shift and the output shift
    // The LUT values are typically in a fixed format (e.g., 30-bit fixed point)
    // So we shift right by (30 - shift_out - integer_part) to get the desired output format
    return interpolated_value >> ((30 - shift_out - integer_part) & 0x1f);
}

// Helper function that might be needed for 64-bit right shift on 32-bit systems
// (This is what __lshrdi3 typically does)
static inline uint32_t logical_shift_right_64(uint32_t low, uint32_t high, unsigned shift)
{
    if (shift >= 32) {
        return high >> (shift - 32);
    } else if (shift == 0) {
        return low;
    } else {
        return (low >> shift) | (high << (32 - shift));
    }
}