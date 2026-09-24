#include <sv_modulation.h>
#include <sv_transformations.h>
#include <numeric_constants.h>

#include <periph_layer_initializer.h>
#include <assert.h>

#define SV_MIN_VBUS 1.0f

static inline float_t max_3(const float_t v_a, const float_t v_b,  const float_t v_c)
{
    float_t max = (v_a > v_b) ? v_a : v_b;
    return (max > v_c) ? max : v_c;
}

static inline float_t min_3(const float_t v_a, const float_t v_b,  const float_t v_c)
{
    float_t min = (v_a < v_b) ? v_a : v_b;
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

void sv_modulation_process(sv_modulation_t *const instance, float_t electrical_angle)
{

	float_t sin_value = sinf(electrical_angle);
	float_t cos_value = cosf(electrical_angle);
	// Inverse Park-transformation
	sv_inv_park_transform(instance->v_d, instance->v_q, sin_value, cos_value, &instance->v_alfa, &instance->v_beta);

	float_t va;
	float_t vb;
	float_t vc;
	// Inverse Clarke-transformation
	sv_inv_clarke_transform(instance->v_alfa, instance->v_beta, &va, &vb, &vc);

	// Calculating the average of the two highest voltage components
	float_t v_max = max_3(va, vb, vc);
	float_t v_min = min_3(va, vb, vc);
	float_t v_offs = ONE_BY_TWO * (v_max + v_min);

	// Center the 3 phases around 0
	va -= v_offs;
	vb -= v_offs;
	vc -= v_offs;

	float_t inv_vbus = instance->inv_v_bus;
	float_t du = (va * inv_vbus) + 0.5f;
	float_t dv = (vb * inv_vbus) + 0.5f;
	float_t dw = (vc * inv_vbus) + 0.5f;


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

	pwm_set_duty_cycles(pwm, dc_phase_u, dc_phase_v, dc_phase_w);
}

void sv_modulation_set_vbus(sv_modulation_t *const instance, const float_t new_vbus)
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

void sv_modulation_set_target_vd_vq(sv_modulation_t *const instance, const float_t new_vd, const float_t new_vq)
{
	instance->v_d = new_vd;
	instance->v_q = new_vq;
}

void sv_modulation_get_v_alfa_v_beta(sv_modulation_t *const instance, float_t *v_alfa_ptr, float_t *v_beta_ptr)
{
	*v_alfa_ptr = instance->v_alfa;
	*v_beta_ptr = instance->v_beta;
}

void sv_modulation_reset(sv_modulation_t *const instance)
{
	instance->v_d = 0.0f;
	instance->v_q = 0.0f;
	pwm_set_duty_cycles(pwm, PWM_DC_50, PWM_DC_50, PWM_DC_50);

}

