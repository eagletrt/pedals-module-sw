#include "bots-api.h"
#include "eagletrt.h"

constexpr float bots_threshold_voltage_triggered = 0.5F;
EAGLETRT_STATIC float bots_voltage = 0.0F;

void bots_set_voltage(float voltage) {
    bots_voltage = voltage;
}

float bots_get_voltage(void) {
    return bots_voltage;
}

bool bots_is_triggered(void) {
    return bots_voltage < bots_threshold_voltage_triggered;
}
