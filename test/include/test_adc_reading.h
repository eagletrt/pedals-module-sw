#ifndef TEST_ADC_READING_H
#define TEST_ADC_READING_H

#include "adc-reading-api.h"

extern int32_t adc_reading_raw_data[];

// UTILITY FUNCTIONs
void insert_values_in_array(
	int32_t v5, int32_t b_f, int32_t b_r, int32_t bpps, int32_t t_1, int32_t t_2, int32_t t_3
) {
	adc_reading_raw_data[ADC_READING_SENSOR_NAME_SENSE_5V] = v5;
	adc_reading_raw_data[ADC_READING_SENSOR_NAME_BSPS_F] = b_f;
	adc_reading_raw_data[ADC_READING_SENSOR_NAME_BSPS_R] = b_r;
	adc_reading_raw_data[ADC_READING_SENSOR_NAME_BPPS] = bpps;
	adc_reading_raw_data[ADC_READING_SENSOR_NAME_APPS_1] = t_1;
	adc_reading_raw_data[ADC_READING_SENSOR_NAME_APPS_2] = t_2;
	adc_reading_raw_data[ADC_READING_SENSOR_NAME_APPS_3] = t_3;
}

#endif //TEST_ADC_READING_H