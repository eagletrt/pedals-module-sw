#ifndef ADC_READING_H
#define ADC_READING_H

#include "stdint.h"
#include "eagletrt-api.h"

enum ADCReadingSensorName {
	ADC_READING_SENSOR_NAME_SENSE_5V,
	ADC_READING_SENSOR_NAME_BSPS_F,
	ADC_READING_SENSOR_NAME_BSPS_R,
	ADC_READING_SENSOR_NAME_BPPS,
	ADC_READING_SENSOR_NAME_APPS_3,
	ADC_READING_SENSOR_NAME_APPS_2,
	ADC_READING_SENSOR_NAME_APPS_1,
	ADC_READING_SENSOR_NAME_NUMBER_OF_SENSORS
};

// this array will be used to collect data from the adc module
EAGLETRT_STATIC int32_t adc_reading_raw_data[ADC_READING_SENSOR_NAME_NUMBER_OF_SENSORS];

int32_t adc_reading_read_raw(enum ADCReadingSensorName sensor);

float adc_reading_return_percentage(enum ADCReadingSensorName sensor, int32_t normal_min, int32_t normal_max);

#endif //ADC_READING_H