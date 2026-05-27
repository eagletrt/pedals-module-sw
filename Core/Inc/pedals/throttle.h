#ifndef THROTTLE_H
#define THROTTLE_H

#include "stdbool.h"

// TODO insert exact values
#define THROTTLE_APPS_1_MIN_VALUE (100.0F)
#define THROTTLE_APPS_1_MAX_VALUE (1100.0F)
#define THROTTLE_APPS_2_MIN_VALUE (1000.0F)
#define THROTTLE_APPS_2_MAX_VALUE (2000.0F)
#define THROTTLE_APPS_3_MIN_VALUE (2000.0F)
#define THROTTLE_APPS_3_MAX_VALUE (3000.0F)

#define THROTTLE_APPS_IMPLAUSIBILITY_PERCENTAGE (0.1F) // equivalent of 10% of 1.0
#define THROTTLE_APPS_EPSILON (0.001F)                // equivalent to 0.1% of 1.0

#define THROTTLE_APPS_NUMBER 3

/*!
 * \brief THROTTLE_STATUS_OK if there are no errors in the sensors or can be solved by redundancy, THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE when errors can't be solved and persisted for less than 100ms, THROTTLE_STATUS_IMPLAUSIBLE_ERROR if they persisted for more than 100ms
 * 
 */
enum ThrottleStatus {
    THROTTLE_STATUS_OK,							/*!< Operations as normal*/
    THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE,	/*!< Implausbility detected but it lasted less than 100ms, can be reverted to OK*/
    THROTTLE_STATUS_IMPLAUSIBLE_ERROR			/*!< Implausbility detected and it exceeded 100ms, cannot be reverted to OK*/
};

/*!
 * \brief return code for throttle operations
 * 
 */
enum ThrottleReturnCode{
	THROTTLE_RC_NO_ERROR,			/*!< No error occured*/
	THROTTLE_RC_IMPLAUSIBILITY,		/*!< Implausibility as for the rules occurred, no new values will be read*/
	THROTTLE_RC_CALLBACK_FAILURE	/*!< Call to external functions failed*/
};

/*!
 * \brief Callback type to access timer (external) functions
 * 
 * \return enum ThrottleReturnCode to check if the call was successful
 * 
 */
typedef enum ThrottleReturnCode (*throttle_timer_callback) ();

/*!
 * \brief structure to handle the status of the throttle
 * 
 */
struct ThrottleHandler {
    float last_throttle_value;				/*!< Last valid value of the throttle, used when it can't reliably read the sensors*/
    enum ThrottleStatus throttle_status;	/*!< Internal status of the throttle to check for implausibility*/
    enum ThrottleReturnCode error_status;	/*!< Last type of error that occurred, can be retrieved*/
	throttle_timer_callback start_timer;	/*!< Pointer to external function to start the timer*/
	throttle_timer_callback stop_timer;		/*!< Pointer to external function to stop and reset the timer*/
};

#endif //THROTTLE_H