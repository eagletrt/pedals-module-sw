#ifndef THROTTLE_API_H
#define THROTTLE_API_H

#include "throttle.h"

/*!
 * \brief function to initialise the external callbacks related to the timer
 * 
 * \param start_timer pointer to function that will start the timer to check for implausibility
 * \param stop_timer pointer to function that will reset the implausibility timer
 * \return enum ThrottleReturnCode status of the operation
 */
enum ThrottleReturnCode throttle_api_init(throttle_timer_callback start_timer, throttle_timer_callback stop_timer);

/*!
 * \brief Function to call when ADC completed its conversions. It saves values into the throttle
 * 
 * \param apps1 Percentage of pedal travel read from APPS1, must be in range [0,1] or -1 if the original sensor read an out of bound value
 * \param apps2 Percentage of pedal travel read from APPS2, must be in range [0,1] or -1 if the original sensor read an out of bound value
 * \param apps3 Percentage of pedal travel read from APPS3, must be in range [0,1] or -1 if the original sensor read an out of bound value
 */
void throttle_api_update_pedal_values(float apps1, float apps2, float apps3);

/*!
 * \brief Updates the combined throttle pedal travel percentage and its status as a consequence 
 * 
 */
void throttle_api_update_internal_status(void);

/*!
 * \brief Function to get the newest value of the throttle pedal travel
 * 
 * \return struct ThrottleReturnValue containing the percentage in range [0,1] and a return code specifying if the operation went ok, the external callbacks failed or it has reached implausible state
 */
struct ThrottleReturnValue throttle_api_get_travel_percentage(void);

/*!
 * \brief Returns last percentage read for APPS1
 * 
 * \return float percentage in range [0,1]
 */
float throttle_api_get_apps1(void);

/*!
 * \brief Returns last percentage read for APPS2
 * 
 * \return float percentage in range [0,1]
 */
float throttle_api_get_apps2(void);

/*!
 * \brief Returns last percentage read for APPS3
 * 
 * \return float percentage in range [0,1]
 */
float throttle_api_get_apps3(void);

/*!
 * \brief Function to activate when implausibility timer goes off
 * 
 */
void throttle_api_implausibility_timeout_trigger(void);

#endif //THROTTLE_API_H