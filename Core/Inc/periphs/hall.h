#ifndef HALL_H
#define HALL_H

#include <stdint.h>


void hall_init(void);

uint32_t hall_getPosition(void);

uint32_t hall_getActElectricalPosition(void);

float hall_getElectricalAngle(void);

#endif /* HALL_H */
