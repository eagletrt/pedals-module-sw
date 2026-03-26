#include "brake.h"
#include "brake-api.h"
#include "adc_reading.h"

float brake_get_percentage() {
	return adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS, BPPS_MIN_VALUE, BPPS_MAX_VALUE);
}

float brake_get_front_pressure() {
	return adc_reading_read_raw(ADC_READING_SENSOR_NAME_BSPS_F) * BSPS_F_CONVERSION_VALUE;
}

float brake_get_rear_pressure() {
	return adc_reading_read_raw(ADC_READING_SENSOR_NAME_BSPS_R) * BSPS_R_CONVERSION_VALUE;
}