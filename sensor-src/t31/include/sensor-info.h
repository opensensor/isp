#ifndef SENSOR_INFO_H
#define SENSOR_INFO_H

struct sensor_info {
	const char *name;
	unsigned int chip_id;
	const char *version;
	int min_fps;
	int max_fps;
	int actual_fps;
	unsigned int chip_i2c_addr;
	int width;
	int height;
	void *priv;
};

/* The open ISP core owns the live /proc/jz/sensor registry. */
static inline void sensor_common_init(struct sensor_info *info) { }
static inline void sensor_common_exit(void) { }
static inline void sensor_update_actual_fps(int fps) { }

#endif
