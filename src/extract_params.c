
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Structure to hold extracted parameters
#define MAX_PARAM_VALUES 64

typedef struct {
    uint32_t values[MAX_PARAM_VALUES];
} isp_param_block_t;

// Function to extract data from a given offset in the binary
int extract_from_offset(const char *binary, size_t offset, isp_param_block_t *block) {
    FILE *fp = fopen(binary, "rb");
    if (!fp) {
        fprintf(stderr, "Failed to open binary file: %s\n", binary);
        return -1;
    }

    // Seek to the given offset
    if (fseek(fp, offset, SEEK_SET) != 0) {
        fprintf(stderr, "Failed to seek to offset 0x%lX\n", offset);
        fclose(fp);
        return -1;
    }

    // Read the parameter values
    size_t read_count = fread(block->values, sizeof(uint32_t), MAX_PARAM_VALUES, fp);
    if (read_count != MAX_PARAM_VALUES) {
        fprintf(stderr, "Warning: only read %zu values at offset 0x%lX\n", read_count, offset);
    }

    fclose(fp);
    return 0;
}

// Function to output the extracted data to a header file
void output_header(const char *filename, isp_param_block_t *day, isp_param_block_t *night, isp_param_block_t *cust) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Failed to open output file: %s\n", filename);
        return;
    }

    fprintf(fp, "#ifndef ISP_PARAMS_H\n");
    fprintf(fp, "#define ISP_PARAMS_H\n\n");

    fprintf(fp, "// Extracted ISP parameters\n");

    fprintf(fp, "static const uint32_t tparams_day[] = {\n");
    for (int i = 0; i < MAX_PARAM_VALUES; i++) {
        fprintf(fp, "    0x%08X,", day->values[i]);
        if ((i + 1) % 8 == 0) fprintf(fp, "\n");
    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "static const uint32_t tparams_night[] = {\n");
    for (int i = 0; i < MAX_PARAM_VALUES; i++) {
        fprintf(fp, "    0x%08X,", night->values[i]);
        if ((i + 1) % 8 == 0) fprintf(fp, "\n");
    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "static const uint32_t tparams_cust[] = {\n");
    for (int i = 0; i < MAX_PARAM_VALUES; i++) {
        fprintf(fp, "    0x%08X,", cust->values[i]);
        if ((i + 1) % 8 == 0) fprintf(fp, "\n");
    }
    fprintf(fp, "};\n\n");

    fprintf(fp, "#endif // ISP_PARAMS_H\n");

    fclose(fp);
    printf("Header file generated: %s\n", filename);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <binary>\n", argv[0]);
        return -1;
    }

    const char *binary = argv[1];

    // Define the known offsets
    size_t offset_day = 0x85a7c;
    size_t offset_night = 0x85abc;
    size_t offset_cust = 0x85c90;

    isp_param_block_t tparams_day, tparams_night, tparams_cust;

    // Extract data from the known offsets
    if (extract_from_offset(binary, offset_day, &tparams_day) != 0) return -1;
    if (extract_from_offset(binary, offset_night, &tparams_night) != 0) return -1;
    if (extract_from_offset(binary, offset_cust, &tparams_cust) != 0) return -1;

    // Output the extracted parameters to a header file
    output_header("isp_params.h", &tparams_day, &tparams_night, &tparams_cust);

    return 0;
}

