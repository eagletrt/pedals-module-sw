#include "throttle-api.h"
#include "adc_reading.h"
#include "eagletrt-api.h"

#include "test_definitions.h"

EAGLETRT_STATIC struct ThrottleHandler throttle_handler = {
    .is_changed_to_implausible = false,
    .last_throttle_value = 0.0,
    .status = THROTTLE_STATUS_OK
};

// internal functions ---------------------------------------

// checks if a percentage is in the range [0,1]
bool is_perc_valid(float val) {
    return val - 1.0 < APPS_EPSILON && val > -APPS_EPSILON;
}

// checks if there's more than 10% points of difference between two values
bool is_within_plausibility(float val_1, float val_2) {
    if (val_1 > val_2) {
        return (val_1 - val_2) < (APPS_IMPLAUSIBILITY_PERCENTAGE + APPS_EPSILON);
    } else {
        return (val_2 - val_1) < (APPS_IMPLAUSIBILITY_PERCENTAGE + APPS_EPSILON);
    }
}

// set the throttle to wait for implausibility error
void set_to_implausibility() {
    if (throttle_handler.status == THROTTLE_STATUS_OK) {
        test_start_timer();
        throttle_handler.status = THROTTLE_STATUS_UNSTABLE;
    }
}

// reset the throttle to its valid state and calculates new value
void set_to_valid(bool is_valid[], int n_valid, float percentages[]) {
    if (throttle_handler.status == THROTTLE_STATUS_UNSTABLE) {
        test_reset_timer();
        throttle_handler.status = THROTTLE_STATUS_OK;
    }

    float result = 0.0;

    for (int i = 0; i < APPS_NUMBER; i++) {
        result = is_valid[i] ? result + percentages[i] : result;
    }

    throttle_handler.last_throttle_value = result / (float)n_valid;
}

// actual api ---------------------------------------

float throttle_get_percentage() {

    // if status is THROTTLE_STATUS_BAD just return 0.0, as to shut down the power to the motor as per T 11.8.8
    if (throttle_handler.status == THROTTLE_STATUS_BAD) {
        return 0.0;
    }

    float percentages[APPS_NUMBER];

    percentages[0] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_1, APPS_1_MIN_VALUE, APPS_1_MAX_VALUE);
    percentages[1] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_2, APPS_2_MIN_VALUE, APPS_2_MAX_VALUE);
    percentages[2] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_3, APPS_3_MIN_VALUE, APPS_3_MAX_VALUE);

    bool perc_is_valid[APPS_NUMBER];
    int valid_sensors = 0;

    // check if it's in range [0,1] as per T 11.9.2
    for (int i = 0; i < APPS_NUMBER; i++) {
        perc_is_valid[i] = is_perc_valid(percentages[i]);
        valid_sensors = perc_is_valid[i] ? valid_sensors + 1 : valid_sensors;
    }

    // check if every pair has less than 10% difference
    bool perc_1_2_is_plausible = is_within_plausibility(percentages[0], percentages[1]);
    bool perc_1_3_is_plausible = is_within_plausibility(percentages[0], percentages[2]);
    bool perc_2_3_is_plausible = is_within_plausibility(percentages[1], percentages[2]);

    // values are implausible if there are less than 2 working sensors or if any of the working pair of sensors has more than 10% difference as per T 11.8.9 and T 11.9
    if (
        (valid_sensors < 2) ||
        (perc_is_valid[0] && perc_is_valid[1] && !perc_1_2_is_plausible) ||
        (perc_is_valid[0] && perc_is_valid[2] && !perc_1_3_is_plausible) ||
        (perc_is_valid[1] && perc_is_valid[2] && !perc_2_3_is_plausible)) {
        set_to_implausibility();

    } else {
        set_to_valid(perc_is_valid, valid_sensors, percentages);
    }

    return throttle_handler.last_throttle_value;
}

bool throttle_is_now_implausible() {
    if (throttle_handler.is_changed_to_implausible) {
        throttle_handler.status = THROTTLE_STATUS_BAD;
        throttle_handler.last_throttle_value = 0.0;
        throttle_handler.is_changed_to_implausible = false;
        return true;
    }

    return false;
}

void throttle_timer_trigger() {
    throttle_handler.is_changed_to_implausible = true;
}