#include "scheduler-api.h"

#include "eagletrt.h"
#include "watchdogs-api.h"

#include <string.h>

EAGLETRT_STATIC struct WatchdogHandler watchdogs_handler;

enum SchedulerReturnCode scheduler_api_init(watchdog_tick_callback get_tick) {
    if (get_tick == nullptr) {
        return SCHEDULER_RC_NULL_POINTER;
    }

    memset(&watchdogs_handler, 0, sizeof(watchdogs_handler));

    if (watchdogs_api_init_pool(&watchdogs_handler, get_tick) != WATCHDOG_RC_OK) {
        return SCHEDULER_RC_ERROR;
    }

    return SCHEDULER_RC_OK;
}

enum WatchdogReturnCode scheduler_api_routine(void) {
    return watchdogs_api_routine(&watchdogs_handler);
}

enum WatchdogReturnCode scheduler_api_watchdog_start(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_start(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_restart(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_restart(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_stop(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_stop(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_reset(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_reset(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_pet(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_pet(&watchdogs_handler, watchdog);
}
