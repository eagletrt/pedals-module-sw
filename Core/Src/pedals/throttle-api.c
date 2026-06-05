#include "throttle-api.h"
#include "sensor-types.h"
#include "voltage-scaling-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct ThrottleHandler throttle_handler = {
    .error_status = THROTTLE_RC_NO_ERROR,
    .last_throttle_value = 0.0,
    .throttle_status = THROTTLE_STATUS_OK,
	.start_timer = NULL,
	.stop_timer = NULL
};

// internal functions ---------------------------------------

/*!
 * \brief Checks if a percentage is within [0%-100%] with an epsilon
 * 
 * \param val
 * \retval true if it's between 0% and 100%
 * \retval false if outside the range
 */
bool prv_throttle_is_percentage_valid(float val) {
    return val - 1.0 < THROTTLE_APPS_EPSILON && val > -THROTTLE_APPS_EPSILON;
}

/*!
 * \brief Checks if there is less than 10% between two APPS sensors
 * 
 * \param val_1
 * \param val_2
 * \retval true if within 10% of difference
 * \retval false if outside
 */
bool prv_throttle_is_percentage_within_plausibility(float val_1, float val_2) {
    if (val_1 > val_2) {
        return (val_1 - val_2) < (THROTTLE_APPS_IMPLAUSIBILITY_PERCENTAGE + THROTTLE_APPS_EPSILON);
    } else {
        return (val_2 - val_1) < (THROTTLE_APPS_IMPLAUSIBILITY_PERCENTAGE + THROTTLE_APPS_EPSILON);
    }
}

/*!
 * \brief function called to start the timer for implausibility
 * 
 */
void prv_throttle_set_to_implausibility() {
    if (throttle_handler.throttle_status == THROTTLE_STATUS_OK) {
        if(throttle_handler.start_timer == NULL || throttle_handler.start_timer() == THROTTLE_RC_CALLBACK_FAILURE){
			throttle_handler.error_status = THROTTLE_RC_CALLBACK_FAILURE;
			return;
		}
        throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;
    }
}

/*!
 * \brief resets the throttle to its good state, stops implausibility timer and calculates the next travel percentage 
 * 
 * \param is_valid bool array to indicate if a sensor is within the values [0%-100%]
 * \param n_valid number of valid sensors
 * \param percentages the percentages measured by the APPS sensors
 */
void prv_throttle_set_to_valid(bool is_valid[], int n_valid, float percentages[]) {
    if (throttle_handler.throttle_status == THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE) {
        if(throttle_handler.stop_timer == NULL || throttle_handler.stop_timer() == THROTTLE_RC_CALLBACK_FAILURE){
			throttle_handler.error_status = THROTTLE_RC_CALLBACK_FAILURE;
			return;
		}
        throttle_handler.throttle_status = THROTTLE_STATUS_OK;
    }

    float result = 0.0F;

    for (int i = 0; i < THROTTLE_APPS_NUMBER; i++) {
        result = is_valid[i] ? result + percentages[i] : result;
    }

    throttle_handler.last_throttle_value = result / (float)n_valid;
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

float throttle_get_travel_percentage() {

    // if status is THROTTLE_STATUS_IMPLAUSIBLE_ERROR just return 0.0, as to shut down the power to the motor as per T 11.8.8
    if (throttle_handler.throttle_status == THROTTLE_STATUS_IMPLAUSIBLE_ERROR) {
        return 0.0F;
    }

    float percentages[THROTTLE_APPS_NUMBER];
	enum VoltageScalingReturnCode return_code;

    return_code = voltage_scaling_get_percentage(&percentages[0], SENSOR_TYPES_NAME_APPS_1, THROTTLE_APPS_1_MIN_VALUE, THROTTLE_APPS_1_MAX_VALUE);
    if(return_code== VOLTAGE_SCALING_RC_ERROR){
		throttle_handler.error_status = THROTTLE_RC_CALLBACK_FAILURE;
		return 0.0F;
	}
	return_code= voltage_scaling_get_percentage(&percentages[1],SENSOR_TYPES_NAME_APPS_2, THROTTLE_APPS_2_MIN_VALUE, THROTTLE_APPS_2_MAX_VALUE);
    if(return_code== VOLTAGE_SCALING_RC_ERROR){
		throttle_handler.error_status = THROTTLE_RC_CALLBACK_FAILURE;
		return 0.0F;
	}
	return_code= voltage_scaling_get_percentage(&percentages[2],SENSOR_TYPES_NAME_APPS_3, THROTTLE_APPS_3_MIN_VALUE, THROTTLE_APPS_3_MAX_VALUE);
	if(return_code== VOLTAGE_SCALING_RC_ERROR){
		throttle_handler.error_status = THROTTLE_RC_CALLBACK_FAILURE;
		return 0.0F;
	}

    bool perc_is_valid[THROTTLE_APPS_NUMBER];
    int valid_sensors = 0;

    // check if it's in range [0,1] as per T 11.9.2
    for (int i = 0; i < THROTTLE_APPS_NUMBER; i++) {
        perc_is_valid[i] =prv_throttle_is_percentage_valid(percentages[i]);
        valid_sensors = perc_is_valid[i] ? valid_sensors + 1 : valid_sensors;
    }

    // check if every pair has less than 10% difference
    bool perc_1_2_is_plausible =prv_throttle_is_percentage_within_plausibility(percentages[0], percentages[1]);
    bool perc_1_3_is_plausible =prv_throttle_is_percentage_within_plausibility(percentages[0], percentages[2]);
    bool perc_2_3_is_plausible =prv_throttle_is_percentage_within_plausibility(percentages[1], percentages[2]);

    // values are implausible if there are less than 2 working sensors or if any of the working pair of sensors has more than 10% difference as per T 11.8.9 and T 11.9
    if (
        (valid_sensors < 2) ||
        (perc_is_valid[0] && perc_is_valid[1] && !perc_1_2_is_plausible) ||
        (perc_is_valid[0] && perc_is_valid[2] && !perc_1_3_is_plausible) ||
        (perc_is_valid[1] && perc_is_valid[2] && !perc_2_3_is_plausible)) {
       prv_throttle_set_to_implausibility();

    } else {
      prv_throttle_set_to_valid(perc_is_valid, valid_sensors, percentages);
    }

    return throttle_handler.last_throttle_value;
}

enum ThrottleReturnCode throttle_get_error_status() {
	enum ThrottleReturnCode error_state = throttle_handler.error_status;
	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;
    if (error_state == THROTTLE_RC_IMPLAUSIBILITY) {
        throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_ERROR;
        throttle_handler.last_throttle_value = 0.0F; 
    }
	return error_state;
}

void throttle_timer_trigger() {
    throttle_handler.error_status = THROTTLE_RC_IMPLAUSIBILITY;
}