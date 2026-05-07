#ifndef THROTTLE_API_H
#define THROTTLE_API_H

#include "throttle.h"
#include "stdbool.h"

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
bool throttle_is_throttle_bad(void);

#endif //THROTTLE_API_H