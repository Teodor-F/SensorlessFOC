#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_

#include <foc_monitor.h>
#include <stdbool.h>

void motor_control_initializer(void);
extern volatile bool foc_telemetry_ready;
extern volatile foc_monitor_frame_t monitor_frame;

#endif /* MOTOR_CONTROL_H_ */
