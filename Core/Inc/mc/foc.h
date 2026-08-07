#ifndef FOC_H_
#define FOC_H_

typedef enum foc_mode foc_mode_t;

enum foc_mode {
    MODE_DISABLED      	= 0,
	MODE_ROTOR_ALIGN 	= 1,
    MODE_OPEN_LOOP     	= 2,
	MODE_HFI			= 3,
    MODE_CLOSED_LOOP    = 4
};

void foc_init(void);

void foc_set_mode(foc_mode_t new_mode);

void foc_callback(void *param);

#endif /* FOC_H_ */
