#ifndef BRAKE_API_H
#define BRAKE_API_H

#include "brake.h"
#include "stdbool.h"

/*!
 * \brief Get percentage of brake travel from BPPS
 * 
 * \return float in range [0,1], if it gets from the sensors a value outside the range, it truncates to 0 or 1 and sets a flag checked with brake_get_error_status
 */
float brake_get_travel_percentage();

/*!
 * \brief Get return code to check what kind of error occured between this and the last call of the function
 * 
 * \retval BRAKE_RC_NO_ERROR if no error occured. After the error was retrieved, the state will default to this value
 * \retval BRAKE_RC_VALUE_OUTSIDE_RANGE one of the values was truncated because it was outside the range
 * \retval BRAKE_RC_CALLBACK_FAILURE if external called failed
 */
enum BrakeReturnCode brake_get_error_status();

/*!
 * \brief Get value of front brake pressure in [TODO unit of measure] 
 * 
 * \return float in [unit of measure]
 */
float brake_get_front_pressure();

/*!
 * \brief Get value of front brake pressure in [TODO unit of measure]
 * 
 * \return float in [unit of measure]
 */
float brake_get_rear_pressure();

#endif //BRAKE_API_H