#ifndef ADC_READING_H
#define ADC_READING_H

#include "stdint.h"

#define ADC_READING_MIN_RAW_VALUE (0.0F)
#define ADC_READING_MAX_RAW_VALUE (4095.0F)

/*!
 * \brief used to identify the different values read in the adc module
 * 
 */
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

/*!
 * \brief Given an interval, it returns the value read in the adc module as a percentage of the interval 
 * 
 * \param sensor name of the sensor of which you want the percentage of
 * \param normal_min lower bound of the interval
 * \param normal_max upper bound of the interval
 * \return float ideally in range [0,1], but can be outside depending on normal_min, normal_max and the values read in the adc module
 */
float adc_reading_return_percentage(enum ADCReadingSensorName sensor, int32_t normal_min, int32_t normal_max);

#endif //ADC_READING_H