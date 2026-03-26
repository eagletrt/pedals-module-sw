#ifndef ADC_READING_H
#define ADC_READING_H

// this array will be used to collect data from the adc module
int adc_reading_raw_data[ADC_READING_SENSOR_NAME_NUMBER_OF_SENSORS];

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

int adc_reading_read_raw(enum ADCReadingSensorName sensor);

float adc_reading_return_percentage(enum ADCReadingSensorName sensor, int normal_min, int normal_max);

#endif ADC_READING_H