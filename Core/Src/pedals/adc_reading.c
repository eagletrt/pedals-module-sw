#include "adc_reading.h"

int32_t adc_reading_read_raw (enum ADCReadingSensorName sensor){
	return adc_reading_raw_data[sensor];
} 

float adc_reading_return_percentage(enum ADCReadingSensorName sensor, int32_t normal_min, int32_t normal_max){
	int raw_current = adc_reading_read_raw(ADC_READING_SENSOR_NAME_SENSE_5V);

	float current_percentage = raw_current / 4095; // assuming 12 bits

	int new_min = current_percentage * normal_min;
	int new_max = current_percentage * normal_max;

	int value = adc_reading_read_raw(sensor);

	return (value - new_min) / (new_max - new_min);
}