#ifndef SCHEDULER_API_H
#define SCHEDULER_API_H

#include "scheduler.h"

#include "stdint.h"
#include "watchdogs.h"

/*!
 * \brief Initialize the board-wide scheduler scheduler and bind its tick getter.
 * \details Call after timebase_init(). All scheduler operations run in main-loop context.
 */
enum SchedulerReturnCode scheduler_init(void);

/*!
 * \brief Execute all expired scheduler callbacks using the board-wide timebase.
 */
enum SchedulerReturnCode scheduler_routine(void);

/*!
 * \brief starts an initialized watchdog.
 */
enum SchedulerReturnCode scheduler_start_watchdog(struct Watchdog *watchdog);

/*!
 * \brief Restarts an initialized watchdog.
 */
enum SchedulerReturnCode scheduler_restart_watchdog(struct Watchdog *watchdog);

/*!
 * \brief Stop a running watchdog.
 */
enum SchedulerReturnCode scheduler_stop_watchdog(struct Watchdog *watchdog);

/*!
 * \brief Return an initialized scheduler to NOT_RUNNING without starting it.
 * \details Also accepts timed-out or already stopped schedulers. Does not fire the callback.
 */
enum SchedulerReturnCode scheduler_reset_watchdog(struct Watchdog *watchdog);

#endif //SCHEDULER_API_H
