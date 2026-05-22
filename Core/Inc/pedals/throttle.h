#ifndef THROTTLE_H
#define THROTTLE_H

#include "stdbool.h"

// TODO insert exact values
#define THROTTLE_APPS_1_MIN_VALUE 100
#define THROTTLE_APPS_1_MAX_VALUE 1100
#define THROTTLE_APPS_2_MIN_VALUE 1000
#define THROTTLE_APPS_2_MAX_VALUE 2000
#define THROTTLE_APPS_3_MIN_VALUE 2000
#define THROTTLE_APPS_3_MAX_VALUE 3000

#define THROTTLE_APPS_IMPLAUSIBILITY_PERCENTAGE (0.1F) // equivalent of 10% of 1.0
#define THROTTLE_APPS_EPSILON (0.001F)                // equivalent to 0.1% of 1.0

#define THROTTLE_APPS_NUMBER 3

/*!
 * \brief THROTTLE_STATUS_OK if there are no errors in the sensors or can be solved by redundancy, THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE when errors can't be solved and persisted for less than 100ms, THROTTLE_STATUS_IMPLAUSIBLE_ERROR if they persisted for more than 100ms
 * 
 */
enum ThrottleStatus {
    THROTTLE_STATUS_OK,
    THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE,
    THROTTLE_STATUS_IMPLAUSIBLE_ERROR
};

enum ThrottleErrorStatus{
	THROTTLE_ERROR_STATUS_NO_ERROR,
	THROTTLE_ERROR_STATUS_IMPLAUSIBILITY,
	THROTTLE_ERROR_STATUS_CALLBACK_FAILURE
};

/*!
 * \brief structure to handle the status of the throttle
 * 
 */
struct ThrottleHandler {
    float last_throttle_value;
    enum ThrottleStatus throttle_status;
    enum ThrottleErrorStatus error_status;
};

#endif //THROTTLE_H