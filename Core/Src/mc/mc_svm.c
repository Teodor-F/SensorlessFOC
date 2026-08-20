#include <config/motor_cfg.h>
#include <mc_svm.h>
#include <numeric_constants.h>

static inline float mc_max3(float a, float b, float c)
{
    float m = (a > b) ? a : b;
    return (m > c) ? m : c;
}

static inline float mc_min3(float a, float b, float c)
{
    float m = (a < b) ? a : b;
    return (m < c) ? m : c;
}

void mc_svm(float v_alpha, float v_beta, float vbus, float *du, float *dv, float *dw)
{
	const float va = v_alpha;
	const float vb = -ONE_BY_TWO * v_alpha + SQRT_THREE_BY_TWO * v_beta;
	const float vc = -ONE_BY_TWO * v_alpha - SQRT_THREE_BY_TWO * v_beta;

	const float vmax = mc_max3(va, vb, vc);
	const float vmin = mc_min3(va, vb, vc);
	const float voff = ONE_BY_TWO * (vmax + vmin);

	const float inv_vbus = 1.0f / vbus;
	*du = 0.5f + (va - voff) * inv_vbus;
	*dv = 0.5f + (vb - voff) * inv_vbus;
	*dw = 0.5f + (vc - voff) * inv_vbus;
}
