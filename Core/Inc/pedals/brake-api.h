#ifndef BRAKE_API_H
#define BRAKE_API_H

#include "brake.h"
#include "stdbool.h"

/*!
 * \brief Get percentage of brake travel from BPPS
 * 
 * \return float in range [0,1], if it gets from the sensors a value outside the range, it truncates to 0 or 1 and sets a flag checked with brake_is_percentage_outside_range
 */
float brake_get_travel_percentage();

/*!
 * \brief Get bool to see if the percentages weren't truncated to 0 or 1 due to them being outside the expected range. It consumes true values
 * 
 * \retval true if the last percentage was truncated because it was outside range. After returning true, it will return false until a percentage is truncated again
 * \retval false if the last percentage was inside the expected range and so wasn't truncated
 */
bool brake_is_percentage_outside_range();

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