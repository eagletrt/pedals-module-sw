#ifndef THROTTLE_H
#define THROTTLE_H

#define THROTTLE_MIN_NUMBER_VALID_APPS (2)
#define THROTTLE_MAX_PERCENTAGE_DEVIATION (0.1F)

#define THROTTLE_ERROR_VALUE (-1.0F)

/*!
 * \brief Represents throttle internal status regarding implausibility, different from return code
 */
enum ThrottleStatus {
    THROTTLE_STATUS_OK,                         /*!< Operations as normal*/
    THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE, /*!< Implausbility detected but it lasted less than 100ms, can be reverted to OK*/
    THROTTLE_STATUS_IMPLAUSIBILITY_ERROR,       /*!< Implausbility detected and it exceeded 100ms, cannot be reverted to OK*/
    THROTTLE_STATUS_CALLBACK_ERROR              /*!< Calls to the timer failed, you can't ensure you aren't implausible*/
};

/*!
 * \brief return code for throttle operations, different from throttle status
 * 
 */
enum ThrottleReturnCode {
    THROTTLE_RC_OK,               /*!< No new error to signal*/
    THROTTLE_RC_CALLBACK_FAILURE, /*!< Call to external functions failed*/
    THROTTLE_RC_NULL_POINTER,      /*!< Pointer was null instead of pointing to function*/
    THROTTLE_RC_ERROR
};

/*!
 * \brief identifier for throttle apps's
 * 
 */
enum ThrottleId {
    THROTTLE_ID_APPS_1, /*!< First apps*/
    THROTTLE_ID_APPS_2, /*!< Second apps*/
    THROTTLE_ID_APPS_3, /*!< Third apps*/
    THROTTLE_ID_COUNT   /*!< Number of APPS present*/
};

/*!
 * \brief Callback type to access timer (external) functions
 * 
 * \return enum ThrottleReturnCode to check if the call was successful
 * 
 */
typedef enum ThrottleReturnCode (*throttle_timer_callback)(void);

/*!
 * \brief Structure to handle the status of the throttle
 * 
 */
struct ThrottleHandler {
    float travel_percentage;                          /*!< Last valid value of the throttle, used when it can't reliably read the sensors*/
    float apps_travel_percentages[THROTTLE_ID_COUNT]; /*!< Contains newest value of percentages from ADC*/
    enum ThrottleStatus status;                       /*!< Internal status of the throttle to check for implausibility*/
    throttle_timer_callback start_timer;              /*!< Pointer to external function to start the timer*/
    throttle_timer_callback stop_timer;               /*!< Pointer to external function to stop and reset the timer*/
    bool is_implausibility_timeout;                   /*!< Bool to set to true when implausibility timer runs out*/
};

#endif //THROTTLE_H
