#ifndef SENSOR_SIM_H
#define SENSOR_SIM_H

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"

typedef struct
{
	int value;
	bool valid;
} sensor_sim_sample_t;

esp_err_t sensor_sim_read(uint32_t seq, sensor_sim_sample_t *out);

#endif
