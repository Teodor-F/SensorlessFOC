#ifndef MEASUREMENT_LAYER_INITIALIZER_H_
#define MEASUREMENT_LAYER_INITIALIZER_H_

#include <current_measure.h>
#include <sliding_mode_observer.h>
#include <pll.h>
#include <velocity_measure.h>

extern velocity_measure_t *velocity_measure;
extern current_measure_t *current_measure;
extern sliding_mode_observer_t *smo;
extern pll_t *pll;

void measurement_layer_initializer(void);

#endif /* MEASUREMENT_LAYER_INITIALIZER_H_ */
