#ifndef THROTTLE_H
#define THROTTLE_H

#include "stdbool.h"

// TODO insert exact values
#define APPS_1_MIN_VALUE 100
#define APPS_1_MAX_VALUE 1100
#define APPS_2_MIN_VALUE 1000
#define APPS_2_MAX_VALUE 2000
#define APPS_3_MIN_VALUE 2000
#define APPS_3_MAX_VALUE 3000

#define APPS_IMPLAUSIBILITY_PERCENTAGE 0.1 // equivalent of 10% of 1.0
#define APPS_MAX_IMPLAUSIBILITY_TIME 100   // in ms
#define APPS_EPSILON 0.001                 // equivalent to 0.1% of 1.0

#define APPS_NUMBER 3

/*!
 * \brief THROTTLE_STATUS_OK if there are no errors in the sensors or can be solved by redundancy, THROTTLE_STATUS_UNSTABLE when errors can't be solved and persisted for less than 100ms, THROTTLE_STATUS_BAD if they persisted for more than 100ms
 * 
 */
enum ThrottleStatus {
    THROTTLE_STATUS_OK,
    THROTTLE_STATUS_UNSTABLE,
    THROTTLE_STATUS_BAD
};

/*!
 * \brief structure to handle the status of the throttle
 * 
 */
struct ThrottleHandler {
    float last_throttle_value;
    enum ThrottleStatus status;
    bool is_changed_to_implausible;
};

#endif //THROTTLE_H