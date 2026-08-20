#include <config/motor_cfg.h>
#include <mc_callback.h>


void mc_callback_init(mc_callback_t *callback)
{
	callback->mc_function = NULL;
	callback->mc_param = NULL;
}

void mc_callback_execute(mc_callback_t *callback)
{
	if(callback->mc_function != NULL)
	{
		callback->mc_function(callback->mc_param);
	}
}

void mc_callback_register_function(mc_callback_t *callback, mc_callback_function_t function, mc_callback_param_t param)
{
	callback->mc_function = function;
	callback->mc_param = param;
}

void mc_callback_unregister_function(mc_callback_t *callback)
{
	callback->mc_function = NULL;
	callback->mc_param = NULL;
}

