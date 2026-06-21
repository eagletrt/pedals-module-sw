#ifndef THROTTLE_H
#define THROTTLE_H

#define THROTTLE_APPS_NUMBER (3)
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
    THROTTLE_RC_NO_ERROR,        /*!< No new error to signal*/
    THROTTLE_RC_CALLBACK_FAILURE /*!< Call to external functions failed*/
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
    float last_throttle_value;                    /*!< Last valid value of the throttle, used when it can't reliably read the sensors*/
    float apps_percentages[THROTTLE_APPS_NUMBER]; /*!< Contains newest value of percentages from ADC*/
    enum ThrottleStatus throttle_status;          /*!< Internal status of the throttle to check for implausibility*/
    throttle_timer_callback start_timer;          /*!< Pointer to external function to start the timer*/
    throttle_timer_callback stop_timer;           /*!< Pointer to external function to stop and reset the timer*/
    bool is_implausibility_timeout;               /*!< Bool to set to true when implausibility timer runs out*/
};

/*!
 * \brief Structure that gets returned when throttle_api_get_travel_percentage is called
 * 
 */
struct ThrottleReturnValue {
    float throttle_value;                /*!< Percentage of the throttle pedal travel*/
    enum ThrottleStatus throttle_status; /*!< Internal status of throttle */
};

#endif //THROTTLE_H