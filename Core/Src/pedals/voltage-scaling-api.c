#include "voltage-scaling-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct VoltageScalingHandler voltage_scaling_handler;


enum VoltageScalingReturnCode voltage_scaling_init(voltage_scaling_sensor_read_callback read_callback){
	if(read_callback == NULL){
		return VOLTAGE_SCALING_RC_ERROR;
	}
	voltage_scaling_handler.read_raw = read_callback;
	return VOLTAGE_SCALING_RC_OK;
}

enum VoltageScalingReturnCode voltage_scaling_get_percentage(float* percentage, enum SensorTypesName sensor, int32_t normal_min, int32_t normal_max){
    if(voltage_scaling_handler.read_raw == NULL){
		return VOLTAGE_SCALING_RC_ERROR;
	}

	int32_t raw_current = voltage_scaling_handler.read_raw(SENSOR_TYPES_NAME_SENSE_5V);

    float current_percentage = (float)raw_current / VOLTAGE_SCALING_MAX_RAW_VALUE;

    float new_min = current_percentage * (float)normal_min;
    float new_max = current_percentage * (float)normal_max;

    int32_t value = voltage_scaling_handler.read_raw(sensor);

    *percentage = ((float)value - new_min) / (new_max - new_min);
	return VOLTAGE_SCALING_RC_OK;
}
