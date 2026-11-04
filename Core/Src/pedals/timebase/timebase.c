#include "pedals/timebase/timebase-api.h"
#include "eagletrt.h"
#include <timebase/timebase-api.h>

EAGLETRT_STATIC struct TimebaseHandler timebase_handler;
EAGLETRT_STATIC bool timebase_initialized;

enum TimebaseReturnCode timebase_init(void) {
    timebase_initialized = false;

    enum TimebaseReturnCode result = timebase_api_init(&timebase_handler, TIMEBASE_RESOLUTION_MS);
    if (result != TIMEBASE_RC_OK) {
        return result;
    }
    result = timebase_set_enable(&timebase_handler, true);
    if (result != TIMEBASE_RC_OK) {
        return result;
    }

    timebase_initialized = true;
    return TIMEBASE_RC_OK;
}

enum TimebaseReturnCode timebase_tick(void) {
    if (!timebase_initialized) {
        return TIMEBASE_RC_DISABLED;
    }
    return timebase_inc_tick(&timebase_handler);
}

uint32_t timebase_get_current_tick(void) {
    return timebase_get_tick(&timebase_handler);
}

uint32_t timebase_get_current_time(void) {
    return timebase_get_time(&timebase_handler);
}

bool timebase_is_initialized(void) {
    return timebase_initialized;
}
