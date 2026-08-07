#include <sv_modulation.h>
#include <assert.h>

#include <numeric_constants.h>
#include <sv_transformations.h>
#include <pwm.h>


#define SV_MIN_VBUS 1.0f

static inline float max_3(const float v_a, const float v_b,  const float v_c)
{
    float max = (v_a > v_b) ? v_a : v_b;
    return (max > v_c) ? max : v_c;
}

static inline float min_3(const float v_a, const float v_b,  const float v_c)
{
    float min = (v_a < v_b) ? v_a : v_b;
    return (min < v_c) ? min : v_c;
}

void sv_modulation_init(sv_modulation_t *const instance, const sv_modulation_cfg_t* const cfg)
{
	assert(instance != NULL && cfg != NULL);
	instance->v_d = 0.0f;
	instance->v_q = 0.0f;
	instance->v_bus = cfg->v_bus;
	instance->inv_v_bus = 1.0f / instance->v_bus;
	instance->mod_state = SV_MODULATION_DISABLED;
}

void sv_modulation_process(sv_modulation_t *const instance, float electrical_angle)
{

	float sin_value = sinf(electrical_angle);
	float cos_value = cosf(electrical_angle);
	// Inverse Park-transformation
	sv_inv_park_transform(instance->v_d, instance->v_q, sin_value, cos_value, &instance->v_alfa, &instance->v_beta);

	float va;
	float vb;
	float vc;
	// Inverse Clarke-transformation
	sv_inv_clarke_transform(instance->v_alfa, instance->v_beta, &va, &vb, &vc);

	// Calculating the average of the two highest voltage components
	float v_max = max_3(va, vb, vc);
	float v_min = min_3(va, vb, vc);
	float v_offs = ONE_BY_TWO * (v_max + v_min);

	// Center the 3 phases around 0
	va -= v_offs;
	vb -= v_offs;
	vc -= v_offs;

	float inv_vbus = instance->inv_v_bus;
	float du = (va * inv_vbus) + 0.5f;
	float dv = (vb * inv_vbus) + 0.5f;
	float dw = (vc * inv_vbus) + 0.5f;


	if (du < 0.0f)
		du = 0.0f;
	if (du > 1.0f)
		du = 1.0f;

	if (dv < 0.0f)
		dv = 0.0f;
	if (dv > 1.0f)
		dv = 1.0f;

	if (dw < 0.0f)
		dw = 0.0f;
	if (dw > 1.0f)
		dw = 1.0f;


	uint32_t dc_phase_u = (uint32_t)(du * PWM_DC_100);
	uint32_t dc_phase_v = (uint32_t)(dv * PWM_DC_100);
	uint32_t dc_phase_w = (uint32_t)(dw * PWM_DC_100);

	pwm_set_duty_cycles(dc_phase_u, dc_phase_v, dc_phase_w);
}

void sv_modulation_set_vbus(sv_modulation_t *const instance, const float new_vbus)
{
	if(new_vbus > SV_MIN_VBUS)
	{
		instance->v_bus = new_vbus;
	}
	else
	{
		instance->v_bus = SV_MIN_VBUS;
	}
	instance->inv_v_bus = 1.0f / instance->v_bus;

}

void sv_modulation_set_state(sv_modulation_t *const instance, sv_modulation_state_t new_modulation_state)
{
	instance->mod_state = new_modulation_state;
}

void sv_modulation_set_target_vd_vq(sv_modulation_t *const instance, const float new_vd, const float new_vq)
{
	instance->v_d = new_vd;
	instance->v_q = new_vq;
}
