/* One sensor/CSI/VIC input can feed multiple independently restarted outputs.
 * Keep owned state in this object, outside the recovered module's BSS aliases.
 */
#ifdef __KERNEL__
#include <linux/errno.h>
#include <linux/mutex.h>
static DEFINE_MUTEX(capture_lock);
#define gate_lock() mutex_lock(&capture_lock)
#define gate_unlock() mutex_unlock(&capture_lock)
#else
#include <errno.h>
#include <pthread.h>
static pthread_mutex_t capture_lock = PTHREAD_MUTEX_INITIALIZER;
#define gate_lock() pthread_mutex_lock(&capture_lock)
#define gate_unlock() pthread_mutex_unlock(&capture_lock)
#endif
#include "tx_isp_t41_capture_gate.h"

static unsigned int running_inputs;

int tx_isp_t41_capture_start_begin(unsigned int input)
{
	if (input >= 3)
		return -EINVAL;
	gate_lock();
	if (running_inputs & (1U << input)) {
		gate_unlock();
		return 0;
	}
	return 1;
}

void tx_isp_t41_capture_start_end(unsigned int input, int result)
{
	if (!result)
		running_inputs |= 1U << input;
	gate_unlock();
}

void tx_isp_t41_capture_stopped(unsigned int input)
{
	if (input >= 3)
		return;
	gate_lock();
	running_inputs &= ~(1U << input);
	gate_unlock();
}
