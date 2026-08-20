#include <config/motor_cfg.h>
#include "lpf_butterworth.h"
#include <numeric_constants.h>


void lpf_butterworth_init(lpf_butterworth_t* const instance, const lpf_butterworth_cfg_t* const cfg)
{
	assert(instance != NULL && cfg != NULL);

    float w = CONSTANT_TWO_PI * (cfg->cutoff_freq / cfg->sampling_freq);
    float Q = 1.0f / SQRT_TWO;
    float alpha = sinf(w) / (2.0f * Q);

    float cos_w0 = cosf(w);
    float a0 = 1.0f + alpha;

    // Butterworth coefficients
    instance->b0 = (1.0f - cos_w0) / (2.0f * a0);
    instance->b1 = (1.0f - cos_w0) / a0;
    instance->b2 = instance->b0;
    instance->a1 = -2.0f * (cos_w0 / a0);
    instance->a2 = (1.0f - alpha) / a0;

    // Set internal calculation results to zero
    instance->x1 = 0.0f;
    instance->x2 = 0.0f;
    instance->y1 = 0.0f;
    instance->y2 = 0.0f;
    instance->processed_value = 0.0f;

}

void lpf_butterworth_process(lpf_butterworth_t* const instance, float input_value)
{
	float output = 	instance->b0 * input_value 	+
					instance->b1 * instance->x1 +
					instance->b2 * instance->x2 -
					instance->a1 * instance->y1 -
					instance->a2 * instance->y2	;

	 // Update internal calculation result
	instance->x2 = instance->x1;
	instance->x1 = input_value;
	instance->y2 = instance->y1;
	instance->y1 = output;
	instance->processed_value = output;
}

float lpf_butterworth_process_get_value(lpf_butterworth_t* const instance)
{
	float ret_val = instance->processed_value;
	return ret_val;
}
