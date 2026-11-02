#include "scheduler-api.h"

#include "eagletrt.h"
#include "pedals/timebase/timebase-api.h" // TODO: check how timebase is handled
#include <watchdogs/watchdogs-api.h>

#include <string.h>

EAGLETRT_STATIC struct WatchdogHandler watchdogs_handler;

enum SchedulerReturnCode scheduler_init(void) {
    // if (!timebase_is_initialized()) {
    //     return WATCHDOG_RC_UNINITIALIZED;
    // }

    memset(&watchdogs_handler, 0, sizeof(watchdogs_handler));

    if (watchdogs_api_init_pool(&watchdogs_handler, timebase_get_current_tick) != WATCHDOG_RC_OK) {
        return SCHEDULER_RC_ERROR;
    }

    return SCHEDULER_RC_OK;
}

enum SchedulerReturnCode scheduler_routine(void) {
    if (watchdogs_api_routine(&watchdogs_handler) != WATCHDOG_RC_OK) {
        return SCHEDULER_RC_ERROR;
    }

    return SCHEDULER_RC_OK;
}

enum SchedulerReturnCode scheduler_start_watchdog(struct Watchdog *watchdog) {
    switch (watchdogs_api_watchdog_start(&watchdogs_handler, watchdog)) {
        case WATCHDOG_RC_OK:
            return SCHEDULER_RC_OK;
        case WATCHDOG_RC_NULL_POINTER:
            return SCHEDULER_RC_NULL_POINTER;
        default:
            return SCHEDULER_RC_ERROR;
    }
}

enum SchedulerReturnCode scheduler_restart_watchdog(struct Watchdog *watchdog) {
    switch (watchdogs_api_watchdog_restart(&watchdogs_handler, watchdog)) {
        case WATCHDOG_RC_OK:
            return SCHEDULER_RC_OK;
        case WATCHDOG_RC_NULL_POINTER:
            return SCHEDULER_RC_NULL_POINTER;
        default:
            return SCHEDULER_RC_ERROR;
    }
}

enum SchedulerReturnCode scheduler_stop_watchdog(struct Watchdog *watchdog) {
    switch (watchdogs_api_watchdog_stop(&watchdogs_handler, watchdog)) {
        case WATCHDOG_RC_OK:
            return SCHEDULER_RC_OK;
        case WATCHDOG_RC_NULL_POINTER:
            return SCHEDULER_RC_NULL_POINTER;
        default:
            return SCHEDULER_RC_ERROR;
    }
}

enum SchedulerReturnCode scheduler_reset_watchdog(struct Watchdog *watchdog) {
    switch (watchdogs_api_watchdog_reset(&watchdogs_handler, watchdog)) {
        case WATCHDOG_RC_OK:
            return SCHEDULER_RC_OK;
        case WATCHDOG_RC_NULL_POINTER:
            return SCHEDULER_RC_NULL_POINTER;
        default:
            return SCHEDULER_RC_ERROR;
    }
}
