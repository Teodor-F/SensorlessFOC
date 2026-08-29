#ifndef CONTROL_LAYER_INITIALIZER_H_
#define CONTROL_LAYER_INITIALIZER_H_

#include <velocity_controller.h>
#include <current_controller.h>
#include <sv_modulation.h>
#include <lpf_first_order.h>

extern velocity_controller_t *velocity_controller;
extern current_controller_t *current_controller;
extern sv_modulation_t *sv_modulation;

extern lpf_first_order_t *lpf_id;
extern lpf_first_order_t *lpf_iq;

void control_layer_initializer(void);

#endif /* CONTROL_LAYER_INITIALIZER_H_ */
