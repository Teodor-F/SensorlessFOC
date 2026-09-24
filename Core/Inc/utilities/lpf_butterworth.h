#ifndef _LPF_BUTTERWORTH_H_
#define _LPF_BUTTERWORTH_H_

typedef struct lpf_butterworth lpf_butterworth_t;

typedef struct lpf_butterworth_cfg lpf_butterworth_cfg_t;

struct lpf_butterworth
{
    float_t b0, b1, b2;  // Numerator coefficients
    float_t a1, a2;      // Denominator coefficients
    float_t x1, x2;      // Input delays
    float_t y1, y2;      // Output delays
    float_t processed_value;
};

struct lpf_butterworth_cfg
{
	float_t cutoff_freq;		// Hz
	float_t sampling_freq;	// Hz
};

void lpf_butterworth_init(lpf_butterworth_t* const instance, const lpf_butterworth_cfg_t* const cfg);

void lpf_butterworth_process(lpf_butterworth_t* const this, float_t value);

float_t lpf_butterworth_get_value(lpf_butterworth_t* const instance);

#endif /* _LPF_BUTTERWORTH_H_ */
