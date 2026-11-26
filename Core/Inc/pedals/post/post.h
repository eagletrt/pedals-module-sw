#ifndef POST_H
#define POST_H

#include "pedals-fsm.h"
#include "throttle.h"
#include "brake.h"
#include "bots.h"
#include "can-communications.h"
#include "watchdogs.h"
#include "bootloader.h"

/*!
 * \brief Operation status for module's functions
 *
 */
enum PostReturnCode {
    POST_RC_OK,    /*!< No error was found*/
    POST_RC_ERROR, /*!< Some initialisations failed*/
};

/*!
 * \brief Wrapper for function pointers needed for initialisation
 *
 */
struct PostInit {
    throttle_timer_callback start_timer; /*!< Throttle's start timer callback*/
    throttle_timer_callback stop_timer;  /*!< Throttle's stop timer callback*/
    struct CanCommunicationsNetworkConfig config;

    throttle_update_hook_callback throttle_update_hook;
    brake_update_hook_callback brake_update_hook;
    bots_update_hook_callback bots_update_hook;

    watchdog_tick_callback get_tick;
    reboot_callback reboot;
};

#endif //POST_H
