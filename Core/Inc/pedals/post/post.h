#ifndef POST_H
#define POST_H

#include "throttle.h"
#include "can-communications.h"

/*!
 * \brief Operation status for module's functions
 * 
 */
enum PostReturnCode {
    POST_RC_OK,   /*!< No error was found*/
    POST_RC_ERROR /*!< Some initialisations failed*/
};

/*!
 * \brief Wrapper for function pointers needed for initialisation
 * 
 */
struct PostInit {
    throttle_timer_callback start_timer; /*!< Throttle's start timer callback*/
    throttle_timer_callback stop_timer;  /*!< Throttle's stop timer callback*/

    struct CanCommunicationsNetworkConfig config;
};

#endif //POST_H
