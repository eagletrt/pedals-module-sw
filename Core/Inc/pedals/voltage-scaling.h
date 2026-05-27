#ifndef VOLTAGE_SCALING_H
#define VOLTAGE_SCALING_H

#include "stdint.h"
#include "sensor-types.h"

#define VOLTAGE_SCALING_MIN_RAW_VALUE (0.0F)
#define VOLTAGE_SCALING_MAX_RAW_VALUE (4095.0F)

/*!
 * \brief Return codes for the module's operations
 * 
 */
enum VoltageScalingReturnCode {
	VOLTAGE_SCALING_RC_OK,		/*!< Operation success*/
	VOLTAGE_SCALING_RC_ERROR	/*!< Operation failed (due to callback errors)*/
};

/*!
 * \brief Callback type for reading from the raw values from the sensors
 * 
 * \param sensor_type enum that identifies the sensor for which it must retrieve the raw value from
 * 
 * \return the raw value from the sensor 
 * 
 */
typedef int32_t (*voltage_scaling_sensor_read_callback)(
	enum SensorTypesName sensor_type);

/*!
 * \brief Handler for the module
 *
 * \details contains callback to read sensors' raw value, if it stays NULL when called, an error will occur
 * 
 */
struct VoltageScalingHandler {
	voltage_scaling_sensor_read_callback read_raw; /*!< pointer to function that retrieves raw values */
};

#endif /* VOLTAGE_SCALING_H */