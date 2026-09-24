#ifndef SVM_H_
#define SVM_H_

typedef struct sv_modulation sv_modulation_t;
typedef struct sv_modulation_cfg sv_modulation_cfg_t;
typedef enum sv_modulation_state sv_modulation_state_t;

enum sv_modulation_state {
	SV_MODULATION_DISABLED,
	SV_MODULATION_ENABLED
};

struct sv_modulation_cfg {
	float_t v_bus;		//[mV]
};

struct sv_modulation {
	sv_modulation_state_t mod_state;
	float_t v_d;			//[mV]
	float_t v_q;			//[mV]
	float_t v_alfa;		//[mV]
	float_t v_beta;		//[mV]
	float_t v_bus;		//[mV]
	float_t inv_v_bus;	//[mV]
};


void sv_modulation_init(sv_modulation_t *const instance, const sv_modulation_cfg_t* const cfg);

void sv_modulation_process(sv_modulation_t *const instance, float_t electrical_angle);

void sv_modulation_set_vbus(sv_modulation_t *const instance, const float_t vbus);

void sv_modulation_set_state(sv_modulation_t *const instance, sv_modulation_state_t new_modulation_state);

void sv_modulation_set_target_vd_vq(sv_modulation_t *const instance, const float_t new_vd, const float_t new_vq);

void sv_modulation_get_v_alfa_v_beta(sv_modulation_t *const instance, float_t *v_alfa_ptr, float_t *v_beta_ptr);

void sv_modulation_reset(sv_modulation_t *const instance);

#endif /* SVM_H_ */
