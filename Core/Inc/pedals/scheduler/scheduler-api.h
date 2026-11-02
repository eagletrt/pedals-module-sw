#ifndef SCHEDULER_API_H
#define SCHEDULER_API_H

#include "scheduler.h"

#include "stdint.h"
#include "watchdogs.h"

/*!
 * \brief Initialize the board-wide scheduler scheduler and bind its tick getter.
 * \details Call after timebase_init(). All scheduler operations run in main-loop context.
 */
enum SchedulerReturnCode scheduler_api_init(watchdog_tick_callback get_tick);

/*!
 * \brief Execute all expired scheduler callbacks using the board-wide timebase.
 */
enum SchedulerReturnCode scheduler_api_routine(void);

/*!
 * \brief starts an initialized watchdog.
 */
enum SchedulerReturnCode scheduler_api_watchdog_start(struct Watchdog *watchdog);

/*!
 * \brief Restarts an initialized watchdog.
 */
enum SchedulerReturnCode scheduler_api_watchdog_restart(struct Watchdog *watchdog);

/*!
 * \brief Stop a running watchdog.
 */
enum SchedulerReturnCode scheduler_api_watchdog_stop(struct Watchdog *watchdog);

/*!
 * \brief Return an initialized scheduler to NOT_RUNNING without starting it.
 * \details Also accepts timed-out or already stopped schedulers. Does not fire the callback.
 */
enum SchedulerReturnCode scheduler_api_watchdog_reset(struct Watchdog *watchdog);

enum SchedulerReturnCode scheduler_api_watchdog_pet(struct Watchdog *watchdog);

#endif //SCHEDULER_API_H
