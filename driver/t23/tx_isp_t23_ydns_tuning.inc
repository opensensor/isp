/* T23 IQ-bin layout for the YDNS parameter block. */
#define REGTRACE_T23_YDNS_TUNING_FILE_OFFSET 0x15588U

struct regtrace_t23_ydns_tuning_image {
    uint32_t edge_out;
    uint32_t mv_thres0[9];
    uint32_t mv_thres1[9];
    uint32_t mv_thres2[9];
    uint32_t fus_level[9];
    uint32_t fus_min_thres[9];
    uint32_t fus_max_thres[9];
    uint32_t fus_sswei[9];
    uint32_t fus_sewei[9];
    uint32_t fus_mswei[9];
    uint32_t fus_mewei[9];
    uint32_t fus_uvwei[9];
    uint32_t edge_wei[9];
    uint32_t edge_div[9];
    uint32_t edge_thres[9];
};
