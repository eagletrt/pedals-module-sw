#ifndef BRAKE_API_H
#define BRAKE_API_H

#include "brake.h"


/*!
 * \brief Get percentage of brake travel from BPPS
 * 
 * \return float ideally in range [0,1], if outside there's an error
 */
float brake_get_percentage(); //if outside the [0,1] range should it send error or shut motor down (does this influence wheel torque, if yes the latter as per T 11.9.5 )

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