#include "brake-api.h"
#include "voltage-scaling-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC enum BrakeErrorStatus brake_error_status = BRAKE_ERROR_STATUS_NO_ERROR;

float brake_get_travel_percentage() {
    float res;
	enum VoltageScalingReturnCode rc = voltage_scaling_get_percentage(&res, SENSOR_TYPES_NAME_BPPS, BRAKE_BPPS_MIN_VALUE, BRAKE_BPPS_MAX_VALUE);
	
	if(rc == VOLTAGE_SCALING_RC_ERROR){
		brake_error_status = BRAKE_ERROR_STATUS_CALLBACK_FAILURE;
		return 0.0F;
	}
	if (res < 0.0F) {
		brake_error_status = BRAKE_ERROR_STATUS_VALUE_OUTSIDE_RANGE;
		return 0.0F;
	} 
	if (res > 1.0F) {
		brake_error_status = BRAKE_ERROR_STATUS_VALUE_OUTSIDE_RANGE;
		return 1.0F;
	} 
	return res; // Returns the percentage as it is, if it's not outisde the range
}

enum BrakeErrorStatus brake_get_error_status() {
	enum BrakeErrorStatus error_state = brake_error_status;
	brake_error_status = BRAKE_ERROR_STATUS_NO_ERROR;
	return error_state;
}

float brake_get_front_pressure() {
    float perc;
	enum VoltageScalingReturnCode rc = voltage_scaling_get_percentage(&perc, SENSOR_TYPES_NAME_BSPS_F, VOLTAGE_SCALING_MIN_RAW_VALUE, VOLTAGE_SCALING_MAX_RAW_VALUE);
    if(rc == VOLTAGE_SCALING_RC_ERROR){
		brake_error_status = BRAKE_ERROR_STATUS_CALLBACK_FAILURE;
		return 0.0F;
	}
	return perc * BRAKE_BSPS_F_CONVERSION_VALUE;
}

float brake_get_rear_pressure() {
    float perc;
	enum VoltageScalingReturnCode rc = voltage_scaling_get_percentage(&perc, SENSOR_TYPES_NAME_BSPS_R, VOLTAGE_SCALING_MIN_RAW_VALUE, VOLTAGE_SCALING_MAX_RAW_VALUE);
    if(rc == VOLTAGE_SCALING_RC_ERROR){
		brake_error_status = BRAKE_ERROR_STATUS_CALLBACK_FAILURE;
		return 0.0F;
	}
	return perc * BRAKE_BSPS_R_CONVERSION_VALUE;
}