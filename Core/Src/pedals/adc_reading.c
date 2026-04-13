#include "adc_reading.h"
#include "eagletrt-api.h"
#include "stdio.h"

// the idea is to collect data with this array from the actual adc module
EAGLETRT_STATIC int32_t adc_reading_raw_data[ADC_READING_SENSOR_NAME_NUMBER_OF_SENSORS];

int32_t adc_reading_read_raw(enum ADCReadingSensorName sensor) {
    return adc_reading_raw_data[sensor];
}

float adc_reading_return_percentage(enum ADCReadingSensorName sensor, int32_t normal_min, int32_t normal_max) {
    int32_t raw_current = adc_reading_read_raw(ADC_READING_SENSOR_NAME_SENSE_5V);

    float current_percentage = (float)raw_current / ADC_READING_MAX_RAW_VALUE;

    float new_min = current_percentage * (float)normal_min;
    float new_max = current_percentage * (float)normal_max;

    int32_t value = adc_reading_read_raw(sensor);

    return ((float)value - new_min) / (new_max - new_min);
}