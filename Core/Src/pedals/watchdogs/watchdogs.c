#include "pedals/watchdogs/watchdogs-api.h"
#include "arena-allocator-api.h"
#include "eagletrt.h"
#include "pedals/timebase/timebase-api.h"
#include <watchdogs/watchdogs-api.h>

#include <string.h>

struct WatchdogsHandler {
    struct WatchdogHandler scheduler;
    bool initialized;
};

EAGLETRT_STATIC struct WatchdogsHandler watchdogs_handler;

EAGLETRT_STATIC bool prv_watchdogs_is_ready(void) {
    return watchdogs_handler.initialized && timebase_is_initialized();
}

enum WatchdogReturnCode watchdogs_init(void) {
    if (!timebase_is_initialized()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    if (watchdogs_handler.initialized) {
        arena_allocator_api_free(&watchdogs_handler.scheduler.arena_handler);
    }
    memset(&watchdogs_handler, 0, sizeof(watchdogs_handler));

    const enum WatchdogReturnCode result =
        watchdogs_api_init_pool(&watchdogs_handler.scheduler, timebase_get_current_tick);
    if (result != WATCHDOG_RC_OK) {
        arena_allocator_api_free(&watchdogs_handler.scheduler.arena_handler);
        return result;
    }

    watchdogs_handler.initialized = true;
    return WATCHDOG_RC_OK;
}

enum WatchdogReturnCode watchdogs_update(void) {
    if (!prv_watchdogs_is_ready()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    return watchdogs_api_routine(&watchdogs_handler.scheduler);
}

enum WatchdogReturnCode watchdogs_init_watchdog(struct Watchdog *watchdog, uint32_t timeout_ms, watchdog_timeout_callback callback) {
    if (watchdog == NULL || callback == NULL) {
        return WATCHDOG_RC_NULL_POINTER;
    }
    if (!prv_watchdogs_is_ready()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    if (timeout_ms < TIMEBASE_RESOLUTION_MS ||
        timeout_ms % TIMEBASE_RESOLUTION_MS != 0U) {
        return WATCHDOG_RC_ERROR;
    }

    return watchdogs_api_init_watchdog(watchdog, TIMEBASE_CONVERT_MS_TO_TICKS(timeout_ms), callback);
}

enum WatchdogReturnCode watchdogs_start(struct Watchdog *watchdog) {
    if (!prv_watchdogs_is_ready()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    return watchdogs_api_watchdog_start(&watchdogs_handler.scheduler, watchdog);
}

enum WatchdogReturnCode watchdogs_stop(struct Watchdog *watchdog) {
    if (!prv_watchdogs_is_ready()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    return watchdogs_api_watchdog_stop(&watchdogs_handler.scheduler, watchdog);
}

enum WatchdogReturnCode watchdogs_restart(struct Watchdog *watchdog) {
    if (!prv_watchdogs_is_ready()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    return watchdogs_api_watchdog_restart(&watchdogs_handler.scheduler, watchdog);
}

enum WatchdogReturnCode watchdogs_reset(struct Watchdog *watchdog) {
    if (!prv_watchdogs_is_ready()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    return watchdogs_api_watchdog_reset(&watchdogs_handler.scheduler, watchdog);
}

enum WatchdogReturnCode watchdogs_pet(struct Watchdog *watchdog) {
    if (!prv_watchdogs_is_ready()) {
        return WATCHDOG_RC_UNINITIALIZED;
    }
    return watchdogs_api_watchdog_pet(&watchdogs_handler.scheduler, watchdog);
}

bool watchdogs_is_running(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_is_running(watchdog);
}

bool watchdogs_is_timed_out(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_is_timed_out(watchdog);
}
