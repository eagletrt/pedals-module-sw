#ifndef VOLTAGE_SCALING_API_H
#define VOLTAGE_SCALING_API_H

#include "voltage-scaling.h"

enum VoltageScalingReturnCode voltage_scaling_init(voltage_scaling_sensor_read_callback read_callback);

/*!
 * \brief Given an interval, it returns the value read in the adc module as a percentage of the interval 
 * 
 * \param percentage pointer to float that will contain the percentage
 * \param sensor name of the sensor of which you want the percentage of
 * \param normal_min lower bound of the interval
 * \param normal_max upper bound of the interval
 * \return float ideally in range [0,1], but can be outside depending on normal_min, normal_max and the values read in the adc module
 */
enum VoltageScalingReturnCode voltage_scaling_get_percentage(float* percentage, enum SensorTypesName sensor, int32_t normal_min, int32_t normal_max);

#endif /* VOLTAGE_SCALING_API_H */