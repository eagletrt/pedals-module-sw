#ifndef SCHEDULER_API_H
#define SCHEDULER_API_H

#include "scheduler.h"

#include "stdint.h"
#include "watchdogs.h"

/*!
 * \brief Initialize the board-wide scheduler and bind its tick getter.
 *
 * \retval SCHEDULER_RC_OK if the module initializated successfully
 * \retval SCHEDULER_RC_NULL_POINTER if `get_tick` was a null poiner
 * \retval SCHEDULER_RC_ERROR if during initialization any error was encountered
 */
enum SchedulerReturnCode scheduler_api_init(watchdog_tick_callback get_tick);

/*!
 * \brief Updates the module status
 *
 * \retval SCHEDULER_RC_OK if the routine was successfull
 * \retval SCHEDULER_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval SCHEDULER_RC_ERROR if the routine failed
 */
enum SchedulerReturnCode scheduler_api_routine(void);

/*!
 * \brief Starts an initialized watchdog.
 *
 * \retval SCHEDULER_RC_OK if no errors were encountered
 * \retval SCHEDULER_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval SCHEDULER_RC_ERROR if the watchdog fails to start
 */
enum SchedulerReturnCode scheduler_api_watchdog_start(struct Watchdog *watchdog);

/*!
 * \brief Restarts an initialized watchdog.
 *
 * \retval SCHEDULER_RC_OK if no errors were encountered
 * \retval SCHEDULER_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval SCHEDULER_RC_ERROR if the watchdog fails to restart
 */
enum SchedulerReturnCode scheduler_api_watchdog_restart(struct Watchdog *watchdog);

/*!
 * \brief Stops a running watchdog.
 *
 * \retval SCHEDULER_RC_OK if no errors were encountered
 * \retval SCHEDULER_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval SCHEDULER_RC_ERROR if the watchdog fails to stop
 */
enum SchedulerReturnCode scheduler_api_watchdog_stop(struct Watchdog *watchdog);

/*!
 * \brief Return an initialized scheduler to NOT_RUNNING without starting it.
 * \details Also accepts timed-out or already stopped schedulers. Does not fire the callback.
 *
 * \retval SCHEDULER_RC_OK if no errors were encountered
 * \retval SCHEDULER_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval SCHEDULER_RC_ERROR if the watchdog fails to reset
 */
enum SchedulerReturnCode scheduler_api_watchdog_reset(struct Watchdog *watchdog);

/*!
 * \brief Pets the watchdog
 *
 * \retval SCHEDULER_RC_OK if no errors were encountered
 * \retval SCHEDULER_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval SCHEDULER_RC_ERROR if the watchdog is not pettable
 */
enum SchedulerReturnCode scheduler_api_watchdog_pet(struct Watchdog *watchdog);

#endif //SCHEDULER_API_H
