#ifndef __TX_ISP_SYSFS_H__
#define __TX_ISP_SYSFS_H__

#include <linux/device.h>

/* Define missing macros */
#ifndef __ATTR_RW
#define __ATTR_RW(_name) \
    __ATTR(_name, 0644, _name##_show, _name##_store)
#endif

/* Define kstrtobool if not available */
static inline int kstrtobool(const char *s, bool *res)
{
    if (!s)
        return -EINVAL;

    switch (s[0]) {
    case 'y':
    case 'Y':
    case '1':
        *res = true;
        break;
    case 'n':
    case 'N':
    case '0':
        *res = false;
        break;
    default:
        return -EINVAL;
    }

    return 0;
}

#endif /* __TX_ISP_SYSFS_H__ */
