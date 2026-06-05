#ifndef THROTTLE_API_H
#define THROTTLE_API_H

#include "throttle.h"
#include "stdbool.h"

/*!
 * \brief function to initialise the external callbacks related to the timer
 * 
 * \param start_timer pointer to function that will start the timer to check for implausibility
 * \param stop_timer pointer to function that will reset the implausibility timer
 * \return enum ThrottleReturnCode status of the operation
 */
enum ThrottleReturnCode throttle_init(throttle_timer_callback start_timer, throttle_timer_callback stop_timer);

/*!
 * \brief Get the average percentage of throttle travel from the three APPS
 * 
 * \return float in range [0,1] when status in (THROTTLE_STATUS_OK, THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE), 0.0 if status is THROTTLE_STATUS_IMPLAUSIBLE_ERROR
 */
float throttle_get_travel_percentage();

/*!
 * \brief Function to activate when implausibility timer goes off
 * 
 */
void throttle_timer_trigger(void);

/*!
 * \brief Get bool to see if implausibility kept for more than 100ms and, consequently, send an error message. It consumes true values, see below
 * 
 * \retval true if throttle_timer_trigger was called. After returning true, successive calls will return false until throttle_timer_trigger is called again
 * \retval false if throttle_timer_trigger wasn't called or if it already returned true without throttle_timer_trigger being called again
 */
enum ThrottleReturnCode throttle_get_error_status(void);

#endif //THROTTLE_API_H