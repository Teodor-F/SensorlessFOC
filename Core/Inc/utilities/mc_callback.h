#ifndef MC_CALLBACK_H_
#define MC_CALLBACK_H_

typedef struct mc_callback mc_callback_t;
typedef void (*mc_callback_function_t)(void* mc_param);
typedef void* mc_callback_param_t;

struct mc_callback
{
	mc_callback_function_t 	mc_function;
	mc_callback_param_t		mc_param;
};

void mc_callback_init(mc_callback_t *callback);

void mc_callback_execute(mc_callback_t *callback);

void mc_callback_register_function(mc_callback_t *callback, mc_callback_function_t function, mc_callback_param_t param);

#endif /* MC_CALLBACK_H_ */


