#include "sensor_sim.h"

#define SENSOR_SIM_TIMEOUT_EVERY_N 20
#define SENSOR_SIM_INVALID_EVERY_N 13

/**
 * @brief Read one simulated sensor sample with deterministic behavior.
 *
 * The input @p seq is the logical sample index (for example, 1st/2nd/3rd read)
 * and fully determines the simulator output. This keeps behavior reproducible
 * across runs so retry and fault-handling logic can be validated consistently.
 *
 * Fault injection rules:
 * - Every 20th sequence: return timeout (ESP_ERR_TIMEOUT)
 * - Every 13th sequence (except timeout cases): return ESP_OK with valid=false
 * - All other sequences: return ESP_OK with valid=true and a generated value
 *
 * @param seq Logical sample sequence number provided by the caller.
 * @param out Destination for the sample result.
 * @return ESP_ERR_INVALID_ARG if @p out is NULL, otherwise one of the outcomes
 * defined by the deterministic rules above.
 */
esp_err_t sensor_sim_read(uint32_t seq, sensor_sim_sample_t *out)
{
	if (out == NULL)
	{
		return ESP_ERR_INVALID_ARG;
	}

	out->value = 0;
	out->valid = false;

	if ((seq % SENSOR_SIM_TIMEOUT_EVERY_N) == 0)
	{
		return ESP_ERR_TIMEOUT;
	}

	if ((seq % SENSOR_SIM_INVALID_EVERY_N) == 0)
	{
		return ESP_OK;
	}

	out->value = (int)(seq % 50) * 3;
	out->valid = true;
	return ESP_OK;
}
