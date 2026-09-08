#ifndef TX_ISP_T41_CAPTURE_GATE_H
#define TX_ISP_T41_CAPTURE_GATE_H

/* Process-context input ownership, separate from each MSCA output's state.
 * begin returns 1 with the gate held when startup is needed, 0 if already
 * started, or a negative errno. Exactly one end must follow a return of 1.
 * Only the input's stop/teardown owner may call stopped; output stop must not.
 */
int tx_isp_t41_capture_start_begin(unsigned int input);
void tx_isp_t41_capture_start_end(unsigned int input, int result);
void tx_isp_t41_capture_stopped(unsigned int input);

#endif
