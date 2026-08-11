#ifndef NUMERIC_CONSTANTS
#define NUMERIC_CONSTANTS

#include <math.h>

#define ONE_BY_TWO			0.5f
#define ONE_BY_THREE		0.33333333333f
#define TWO_BY_THREE		0.66666666666f
#define SQRT_TWO			1.41421356237f
#define SQRT_THREE			1.73205080757f
#define ONE_BY_SQRT_THREE   0.57735025882720947265625f
#define TWO_BY_SQRT_THREE	1.15470053838f
#define SQRT_THREE_BY_TWO   0.866025388240814208984375f
#define CONSTANT_PI     	3.1415926535897932384626433832795f
#define CONSTANT_TWO_PI 	2.0f * CONSTANT_PI


#define CONSTRAIN_ANGLE_RAD_ZERO_TWO_PI(theta)	\
    do {                                 		\
        while ((theta) < 0.0f)           		\
            (theta) += CONSTANT_TWO_PI;         \
        while ((theta) >= CONSTANT_TWO_PI)      \
            (theta) -= CONSTANT_TWO_PI;         \
    } while (0)

#endif /* NUMERIC_CONSTANTS */
