#ifndef PEDALS_WATCHDOGS_API_H
#define PEDALS_WATCHDOGS_API_H

#include <watchdogs/watchdogs.h>

#include <stdbool.h>
#include <stdint.h>

/*!
 * \brief Initialize the board-wide watchdog scheduler and bind its tick getter.
 * \details Call after timebase_init(). All watchdog operations run in main-loop context.
 */
enum WatchdogReturnCode watchdogs_init(void);

/*! \brief Execute all expired watchdog callbacks using the board-wide timebase. */
enum WatchdogReturnCode watchdogs_update(void);

/*! \brief Initialize a watchdog whose timeout is expressed in milliseconds. */
enum WatchdogReturnCode watchdogs_init_watchdog(struct Watchdog *watchdog, uint32_t timeout_ms, watchdog_timeout_callback callback);

/*! \brief Start a previously initialized watchdog. */
enum WatchdogReturnCode watchdogs_start(struct Watchdog *watchdog);

/*! \brief Stop a running watchdog. */
enum WatchdogReturnCode watchdogs_stop(struct Watchdog *watchdog);

/*! \brief Start or re-arm a watchdog regardless of its previous state. */
enum WatchdogReturnCode watchdogs_restart(struct Watchdog *watchdog);

/*!
 * \brief Return an initialized watchdog to NOT_RUNNING without starting it.
 * \details Also accepts timed-out or already stopped watchdogs. Does not fire the callback.
 */
enum WatchdogReturnCode watchdogs_reset(struct Watchdog *watchdog);

/*! \brief Re-arm a watchdog that is currently running. */
enum WatchdogReturnCode watchdogs_pet(struct Watchdog *watchdog);

/*! \brief Tell whether a watchdog is currently scheduled. */
bool watchdogs_is_running(struct Watchdog *watchdog);

/*! \brief Tell whether a watchdog has expired. */
bool watchdogs_is_timed_out(struct Watchdog *watchdog);

#endif // PEDALS_WATCHDOGS_API_H
