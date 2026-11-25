#ifndef SCHEDULER_API_H
#define SCHEDULER_API_H

#include "scheduler.h"

#include "stdint.h"
#include "watchdogs.h"
#include "tasks.h"

/*!
 * \brief Initialize the board-wide scheduler and bind its tick getter.
 *
 * \retval SCHEDULER_RC_OK if the module initializated successfully
 * \retval SCHEDULER_RC_NULL_POINTER if `get_tick` was a null poiner
 * \retval SCHEDULER_RC_ERROR if during initialization any error was encountered
 */
enum SchedulerReturnCode scheduler_api_init(watchdog_tick_callback get_tick);

/*!
 * \brief Updates the module watchdogs status
 *
 * \retval WATCHDOG_RC_OK if the routine was successfull
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval WATCHDOG_RC_ERROR if the routine failed
 */
enum WatchdogReturnCode scheduler_api_watchdogs_routine(void);

/*!
 * \brief Starts an initialized watchdog.
 *
 * \retval WATCHDOG_RC_OK if no errors were encountered
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval WATCHDOG_RC_ERROR if the watchdog fails to start
 */
enum WatchdogReturnCode scheduler_api_watchdog_start(struct Watchdog *watchdog);

/*!
 * \brief Restarts an initialized watchdog.
 *
 * \retval WATCHDOG_RC_OK if no errors were encountered
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval WATCHDOG_RC_ERROR if the watchdog fails to restart
 */
enum WatchdogReturnCode scheduler_api_watchdog_restart(struct Watchdog *watchdog);

/*!
 * \brief Stops a running watchdog.
 *
 * \retval WATCHDOG_RC_OK if no errors were encountered
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval WATCHDOG_RC_ERROR if the watchdog fails to stop
 */
enum WatchdogReturnCode scheduler_api_watchdog_stop(struct Watchdog *watchdog);

/*!
 * \brief Return an initialized scheduler to NOT_RUNNING without starting it.
 * \details Also accepts timed-out or already stopped schedulers. Does not fire the callback.
 *
 * \retval WATCHDOG_RC_OK if no errors were encountered
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval WATCHDOG_RC_ERROR if the watchdog fails to reset
 */
enum WatchdogReturnCode scheduler_api_watchdog_reset(struct Watchdog *watchdog);

/*!
 * \brief Pets the watchdog
 *
 * \retval WATCHDOG_RC_OK if no errors were encountered
 * \retval WATCHDOG_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval WATCHDOG_RC_ERROR if the watchdog is not pettable
 */
enum WatchdogReturnCode scheduler_api_watchdog_pet(struct Watchdog *watchdog);

/*!
 * \brief Updates the module tasks status
 *
 * \retval TASKS_RC_OK The operation was successful
 * \retval TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval TASKS_RC_ERROR An error occurred during the operation
 * \retval TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_tasks_routine(void);

/*!
 * \brief Enables a single task
 *
 * \param task_id The identifier of the task
 *
 * \retval TASKS_RC_OK The operation was successful
 * \retval TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval TASKS_RC_ERROR An error occurred during the operation
 * \retval TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_enable_task(uint8_t task_id);

/*!
 * \brief Pauses a single task, the task can be resumed from where it was paused
 *
 * \param task_id The identifier of the task
 *
 * \retval TASKS_RC_OK The operation was successful
 * \retval TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval TASKS_RC_ERROR An error occurred during the operation
 * \retval TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_pause_task(uint8_t task_id);

/*!
 * \brief Disables a single task, the task will be restarted from the beginning when enabled again
 *
 * \param task_id The identifier of the task
 *
 * \retval TASKS_RC_OK The operation was successful
 * \retval TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval TASKS_RC_ERROR An error occurred during the operation
 * \retval TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_disable_task(uint8_t task_id);

/*!
 * \brief Get a single task information
 *
 * \param task_id The identifier of the task
 * \param task The pointer to the task structure where the information will be stored, must not be NULL
 *
 * \retval TASKS_RC_OK The operation was successful, the task information is stored in the provided pointer
 * \retval TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval TASKS_RC_INVALID_ID The given identifier does not exist
 */
enum TasksReturnCode scheduler_api_get_task(uint8_t task_id, struct Task *task);

/*!
 * \brief Update a single task information, the task will be rescheduled according to the new information
 * if any changes are made to the interval or start time of the task they will take effect
 * only after the next time the task is called by the routine.
 *
 * \param task_id The identifier of the task
 * \param new_interval The new interval of the task
 * \param new_start The new start time of the task
 * \param repeats The amount of times the task will fire (0 = infinite, 1 = one-shot, ecc...)
 *
 * \retval TASKS_RC_OK The operation was successful
 * \retval TASKS_RC_NULL_POINTER A null pointer was passed as argument
 * \retval TASKS_RC_INVALID_ID The given identifier does not exist
 * \retval TASKS_RC_ERROR An error occurred during the operation
 * \retval TASKS_RC_TEMPORAL_DISCONTINUITY The user tried to travel to the past
 */
enum TasksReturnCode scheduler_api_update_task(uint8_t task_id, uint16_t new_interval, uint16_t new_start, uint8_t repeats);

/*!
 * \brief Get the number of dropped task executions
 *
 * \returns The number of dropped task executions
 */
uint16_t scheduler_api_get_dropped_task_executions(void);
#endif //SCHEDULER_API_H
