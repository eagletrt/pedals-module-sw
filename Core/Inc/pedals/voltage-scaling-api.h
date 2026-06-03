#ifndef VOLTAGE_SCALING_API_H
#define VOLTAGE_SCALING_API_H

#include "voltage-scaling.h"

/*!
 * \brief it initialises the handler in order to use external functions
 * 
 * \param read_callback function to be used by the function to read raw values from the sensors
 * \return enum VoltageScalingReturnCode status of the operation, ERROR if external call failed, OK otherwise
 */
enum VoltageScalingReturnCode voltage_scaling_init(voltage_scaling_sensor_read_callback read_callback);

/*!
 * \brief Given an interval, it returns the value read in the adc module as a percentage of the interval 
 * 
 * \param percentage pointer to float that will contain the percentage, the range is [0,1]
 * \param sensor name of the sensor of which you want the percentage of
 * \param normal_min lower bound of the interval
 * \param normal_max upper bound of the interval
 * \return enum VoltageScalingReturnCode operation's success status
 */
enum VoltageScalingReturnCode voltage_scaling_get_percentage(float* percentage, enum SensorTypesName sensor, float normal_min, float normal_max);

#endif /* VOLTAGE_SCALING_API_H */