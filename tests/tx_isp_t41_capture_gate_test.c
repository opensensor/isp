#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include "../driver/t41/tx_isp_t41_capture_gate.h"

static unsigned int starts;

static void *start_outputs(void *unused)
{
	unsigned int i;

	(void)unused;
	for (i = 0; i < 1000; ++i) {
		int ret = tx_isp_t41_capture_start_begin(0);

		assert(ret >= 0);
		if (ret) {
			++starts; /* The startup gate, not another test lock, owns this. */
			tx_isp_t41_capture_start_end(0, 0);
		}
	}
	return NULL;
}

int main(void)
{
	pthread_t threads[4];
	unsigned int i;

	assert(tx_isp_t41_capture_start_begin(3) == -EINVAL);
	tx_isp_t41_capture_stopped(3);
	assert(tx_isp_t41_capture_start_begin(0) == 1);
	tx_isp_t41_capture_start_end(0, -EIO);
	assert(tx_isp_t41_capture_start_begin(0) == 1); /* Failed startup retries. */
	tx_isp_t41_capture_start_end(0, 0);
	assert(tx_isp_t41_capture_start_begin(0) == 0); /* A second output. */
	assert(tx_isp_t41_capture_start_begin(1) == 1); /* Independent input. */
	tx_isp_t41_capture_start_end(1, 0);
	tx_isp_t41_capture_stopped(0);
	assert(tx_isp_t41_capture_start_begin(1) == 0);
	for (i = 0; i < 4; ++i)
		assert(!pthread_create(&threads[i], NULL, start_outputs, NULL));
	for (i = 0; i < 4; ++i)
		assert(!pthread_join(threads[i], NULL));
	assert(starts == 1);
	tx_isp_t41_capture_stopped(0);
	assert(tx_isp_t41_capture_start_begin(0) == 1); /* Full input restart. */
	tx_isp_t41_capture_start_end(0, 0);
	puts("T41 shared capture ownership: passed");
	return 0;
}
