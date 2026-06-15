#include "throttle-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct ThrottleHandler throttle_handler = {
    .is_implausibility_timeout = false,
    .last_throttle_value = THROTTLE_MIN_VALUE,
    .throttle_status = THROTTLE_STATUS_OK,
	.error_status = THROTTLE_RC_NO_ERROR,
	.start_timer = NULL,
	.stop_timer = NULL
};

// internal functions ---------------------------------------

float prv_throttle_calculate_next_value(float apps1, float apps2, float apps3){
	float values[] = {apps1, apps2, apps3};
	float n_valid = THROTTLE_MIN_VALUE;
	float total_value = THROTTLE_MIN_VALUE;
	float min = THROTTLE_MAX_VALUE;
	float max = THROTTLE_MIN_VALUE;

	for(int i = 0; i < THROTTLE_APPS_NUMBER; i++){
		// check if it's in range [0,1] as per T 11.9.2
		if(values[i] != THROTTLE_ERROR_VALUE){
			n_valid++;
			total_value += values[i];
			min = EAGLETRT_API_MIN(min,values[i]);
			max = EAGLETRT_API_MAX(max,values[i]);
		}
	}

	float result;
	// values are implausible if there are less than 2 working sensors or if any of the working pair of sensors has more than 10% difference as per T 11.8.9 and T 11.9
	if(n_valid >= THROTTLE_MIN_NUMBER_VALID_APPS && ((max - min) <= THROTTLE_MAX_PERCENTAGE_DEVIATION)){
		result = total_value / n_valid;
	} else {
		result = THROTTLE_ERROR_VALUE;
	}

	return result;
}

void prv_throttle_update_error(enum ThrottleReturnCode error){
	if(error == THROTTLE_RC_IMPLAUSIBILITY || (error == THROTTLE_RC_CALLBACK_FAILURE && throttle_handler.error_status != THROTTLE_RC_IMPLAUSIBILITY)){
		throttle_handler.error_status = error;
	}
}

void prv_throttle_status_ok_routine(float new_value){
	enum ThrottleReturnCode err = THROTTLE_RC_NO_ERROR;

	if(new_value != THROTTLE_ERROR_VALUE){
		throttle_handler.last_throttle_value = new_value;
	} 
	else {
		throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;
		if(throttle_handler.start_timer != NULL){
			throttle_handler.start_timer();
		} else {
			err = THROTTLE_RC_CALLBACK_FAILURE;
		}
	}

	prv_throttle_update_error(err);
}

void prv_throttle_status_recoverable_routine(float new_value){
	enum ThrottleReturnCode err = THROTTLE_RC_NO_ERROR;

	if(new_value != THROTTLE_ERROR_VALUE){
		throttle_handler.throttle_status = THROTTLE_STATUS_OK;
		throttle_handler.last_throttle_value = new_value;
		if(throttle_handler.stop_timer != NULL){
			throttle_handler.stop_timer();
		} else {
			err = THROTTLE_RC_CALLBACK_FAILURE;
		}
	}

	prv_throttle_update_error(err);
}

void prv_throttle_status_implausible_routine(){
	throttle_handler.last_throttle_value = THROTTLE_MIN_VALUE;
	throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_ERROR;
	throttle_handler.is_implausibility_timeout = false;
	prv_throttle_update_error(THROTTLE_RC_IMPLAUSIBILITY);
}

void prv_throttle_next_state(float new_value){
	// regardless of the current state, if the flag is found activated you must set to IMPLAUSIBLE_ERROR and notify the user
	if(throttle_handler.is_implausibility_timeout){
		prv_throttle_status_implausible_routine();
	}

	switch(throttle_handler.throttle_status) {
		case THROTTLE_STATUS_OK: {
			prv_throttle_status_ok_routine(new_value);
			break;
		}
		case THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE: {
			prv_throttle_status_recoverable_routine(new_value);
			break;
		}

		default: {} //do nothing, in IMPLAUSIBLE ERROR you can't change state anymore
	}

}

// actual api ---------------------------------------

enum ThrottleReturnCode throttle_init(throttle_timer_callback start_timer, throttle_timer_callback stop_timer){
	if(start_timer == NULL || stop_timer == NULL){
		return THROTTLE_RC_CALLBACK_FAILURE;
	}

	throttle_handler.start_timer = start_timer;
	throttle_handler.stop_timer = stop_timer;
	return THROTTLE_RC_NO_ERROR;
}

void throttle_update_pedal_values(float apps1, float apps2, float apps3){
	// if status is THROTTLE_STATUS_IMPLAUSIBLE_ERROR you can't recover from the error, leave the throttle state as it is
    if (throttle_handler.throttle_status == THROTTLE_STATUS_IMPLAUSIBLE_ERROR) {
        return;
    }
	apps1 = ((apps1 > THROTTLE_MAX_VALUE) || (apps1 < THROTTLE_MIN_VALUE)) ? THROTTLE_ERROR_VALUE : apps1;
	apps2 = ((apps2 > THROTTLE_MAX_VALUE) || (apps2 < THROTTLE_MIN_VALUE)) ? THROTTLE_ERROR_VALUE : apps2;
	apps3 = ((apps3 > THROTTLE_MAX_VALUE) || (apps3 < THROTTLE_MIN_VALUE)) ? THROTTLE_ERROR_VALUE : apps3;

	float next_val = prv_throttle_calculate_next_value(apps1, apps2, apps3);

	prv_throttle_next_state(next_val);
}


struct ThrottleReturnValue throttle_get_travel_percentage(){
	struct ThrottleReturnValue ret = {
		.throttle_error = throttle_handler.error_status,
		.throttle_value = throttle_handler.last_throttle_value
	};
	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;

	return ret;
}


void throttle_implausibility_timeout_trigger() {
    throttle_handler.is_implausibility_timeout = true;
}