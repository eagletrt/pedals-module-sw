#include "bots-api.h"
#include "eagletrt-api.h"
#include "eagletrt.h"

EAGLETRT_STATIC struct BotsHandler bots_handler;

enum BotsReturnCode bots_api_init(bots_update_hook_callback update_hook) {
    if (update_hook == nullptr) {
        return BOTS_RC_NULL_POINTER;
    }

    bots_handler = (struct BotsHandler) {
        .voltage = 0.0F,
        .update_hook = update_hook,
    };

    return BOTS_RC_OK;
}

void bots_api_set_voltage(float voltage) {
    bots_handler.voltage = voltage;
}

float bots_api_get_voltage(void) {
    return bots_handler.voltage;
}

bool bots_api_is_triggered(void) {
    return bots_handler.voltage < bots_threshold_voltage_triggered;
}

void bots_api_routine_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    bots_handler.update_hook();
}
