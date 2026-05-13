#include "brake.h"
#include "brake-api.h"
#include "adc-reading-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC bool brake_is_percentage_truncated = false;

float brake_get_travel_percentage() {
    float res = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS, BRAKE_BPPS_MIN_VALUE, BRAKE_BPPS_MAX_VALUE);
	if (res < 0.0f) {
		brake_is_percentage_truncated = true;
		return 0.0f;
	} else if (res > 1.0f) {
		brake_is_percentage_truncated = true;
		return 1.0f;
	} else {
		return res;
	}
}

bool brake_is_percentage_outside_range() {
	if (brake_is_percentage_truncated) {
		brake_is_percentage_truncated = false;
		return true;
	}
	return false;
}

float brake_get_front_pressure() {
    float perc = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BSPS_F, ADC_READING_MIN_RAW_VALUE, ADC_READING_MAX_RAW_VALUE);
    return perc * BRAKE_BSPS_F_CONVERSION_VALUE;
}

float brake_get_rear_pressure() {
    float perc = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BSPS_R, ADC_READING_MIN_RAW_VALUE, ADC_READING_MAX_RAW_VALUE);
    return perc * BRAKE_BSPS_R_CONVERSION_VALUE;
}