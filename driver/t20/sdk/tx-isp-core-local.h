#ifndef __OPEN_TX_ISP_T20_CORE_LOCAL_H__
#define __OPEN_TX_ISP_T20_CORE_LOCAL_H__

/*
 * Every translation unit that dereferences struct tx_isp_core_device must see
 * the repository-owned repaired definition before an unchanged SDK header can
 * claim the vendor include guard.  Include delay.h first because the legacy
 * Apical headers redefine msleep while Linux is still declaring it.
 */
#include <linux/delay.h>
#include "source/apical-isp/tx-isp-core.h"

#endif /* __OPEN_TX_ISP_T20_CORE_LOCAL_H__ */
