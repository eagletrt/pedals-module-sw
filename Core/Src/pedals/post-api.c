#include "post-api.h"
#include "voltage-scaling-api.h"
#include "throttle-api.h"
#include "stddef.h"

enum PostReturnCode post_init(struct PostInit* init){
	if(init == NULL){
		return POST_RC_ERROR;
	}

	if(voltage_scaling_init(init->read_raw_values) != VOLTAGE_SCALING_RC_OK){
		return POST_RC_ERROR;
	}

	if(throttle_init(init->start_timer, init->stop_timer) != THROTTLE_RC_NO_ERROR){
		return POST_RC_ERROR;
	}

	return POST_RC_OK;

}