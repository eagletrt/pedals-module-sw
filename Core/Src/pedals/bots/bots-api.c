#include "bots-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC bool bots_is_trigger_activated;

void bots_trigger(void) {
    bots_is_trigger_activated = true;
}

bool bots_is_triggered(void) {
    if (bots_is_trigger_activated) {
        bots_is_trigger_activated = false;
        return true;
    }

    return false;
}