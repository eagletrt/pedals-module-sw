#ifndef BRAKE_API_H
#define BRAKE_API_H

#include "brake.h"

enum BrakeReturnCode brake_init_handler(
	brake_percentage_callback pedal_travel_cb, 
	brake_percentage_callback front_pressure_cb,
	brake_percentage_callback rear_pressure_cb
);

/*!
 * \brief Get percentage of brake travel from BPPS
 * 
 * \param travel pointer to float to store percentage. Assured to be in range [0,1], if it gets from the sensors a value outside the range, it truncates it to 0 or 1
 * \retval BRAKE_RC_NO_ERROR if no error occured
 * \retval BRAKE_RC_VALUE_OUTSIDE_RANGE if the original percentage from the sensor was truncated because it was outside the range
 * \retval BRAKE_RC_CALLBACK_FAILURE if external call failed
 */
enum BrakeReturnCode brake_get_travel_percentage(float* travel);

/*!
 * \brief Get percentage of brake travel from BSPS_F and BSPS_R
 * 
 * \param travel pointer to float to store front brake pressure. Assured to be in range [0,1], if it gets from the sensors a value outside the range, it truncates it to 0 or 1
 * \param travel pointer to float to store rear brake pressure. It works the same as front 
 * \retval BRAKE_RC_NO_ERROR if no error occured
 * \retval BRAKE_RC_VALUE_OUTSIDE_RANGE if at least one of the two values from the sensor was truncated because it was outside the range
 * \retval BRAKE_RC_CALLBACK_FAILURE if external call failed
 */
enum BrakeReturnCode brake_get_pressures(float* front, float* rear);


#endif //BRAKE_API_H