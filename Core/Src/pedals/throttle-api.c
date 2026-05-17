#include "throttle-api.h"
#include "adc.h"
#include "eagletrt-api.h"

#include "test-definitions.h"

EAGLETRT_STATIC struct ThrottleHandler throttle_handler = {
    .is_changed_to_implausible = false,
    .last_throttle_value = 0.0,
    .status = THROTTLE_STATUS_OK
};

// internal functions ---------------------------------------

/*!
 * \brief Checks if a percentage is within [0%-100%] with an epsilon
 * 
 * \param val
 * \retval true if it's between 0% and 100%
 * \retval false if outside the range
 */
bool throttle_is_percentage_valid(float val) {
    return val - 1.0 < THROTTLE_APPS_EPSILON && val > -THROTTLE_APPS_EPSILON;
}

/*!
 * \brief Checks if there is less than 10% between two APPS sensors
 * 
 * \param val_1
 * \param val_2
 * \retval true if within 10% of difference
 * \retval false if outside
 */
bool throttle_is_percentage_within_plausibility(float val_1, float val_2) {
    if (val_1 > val_2) {
        return (val_1 - val_2) < (THROTTLE_APPS_IMPLAUSIBILITY_PERCENTAGE + THROTTLE_APPS_EPSILON);
    } else {
        return (val_2 - val_1) < (THROTTLE_APPS_IMPLAUSIBILITY_PERCENTAGE + THROTTLE_APPS_EPSILON);
    }
}

/*!
 * \brief function called to start the timer for implausibility
 * 
 */
void throttle_set_to_implausibility() {
    if (throttle_handler.status == THROTTLE_STATUS_OK) {
        test_start_timer();
        throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;
    }
}

/*!
 * \brief resets the throttle to its good state, stops implausibility timer and calculates the next travel percentage 
 * 
 * \param is_valid bool array to indicate if a sensor is within the values [0%-100%]
 * \param n_valid number of valid sensors
 * \param percentages the percentages measured by the APPS sensors
 */
void throttle_set_to_valid(bool is_valid[], int n_valid, float percentages[]) {
    if (throttle_handler.status == THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE) {
        test_reset_timer();
        throttle_handler.status = THROTTLE_STATUS_OK;
    }

    float result = 0.0F;

    for (int i = 0; i < THROTTLE_APPS_NUMBER; i++) {
        result = is_valid[i] ? result + percentages[i] : result;
    }

    throttle_handler.last_throttle_value = result / (float)n_valid;
}

// actual api ---------------------------------------

float throttle_get_travel_percentage() {

    // if status is THROTTLE_STATUS_IMPLAUSIBLE_ERROR just return 0.0, as to shut down the power to the motor as per T 11.8.8
    if (throttle_handler.status == THROTTLE_STATUS_IMPLAUSIBLE_ERROR) {
        return 0.0F;
    }

    float percentages[THROTTLE_APPS_NUMBER];

    percentages[0] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_1, THROTTLE_APPS_1_MIN_VALUE, THROTTLE_APPS_1_MAX_VALUE);
    percentages[1] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_2, THROTTLE_APPS_2_MIN_VALUE, THROTTLE_APPS_2_MAX_VALUE);
    percentages[2] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_3, THROTTLE_APPS_3_MIN_VALUE, THROTTLE_APPS_3_MAX_VALUE);

    bool perc_is_valid[THROTTLE_APPS_NUMBER];
    int valid_sensors = 0;

    // check if it's in range [0,1] as per T 11.9.2
    for (int i = 0; i < THROTTLE_APPS_NUMBER; i++) {
        perc_is_valid[i] = throttle_is_percentage_valid(percentages[i]);
        valid_sensors = perc_is_valid[i] ? valid_sensors + 1 : valid_sensors;
    }

    // check if every pair has less than 10% difference
    bool perc_1_2_is_plausible = throttle_is_percentage_within_plausibility(percentages[0], percentages[1]);
    bool perc_1_3_is_plausible = throttle_is_percentage_within_plausibility(percentages[0], percentages[2]);
    bool perc_2_3_is_plausible = throttle_is_percentage_within_plausibility(percentages[1], percentages[2]);

    // values are implausible if there are less than 2 working sensors or if any of the working pair of sensors has more than 10% difference as per T 11.8.9 and T 11.9
    if (
        (valid_sensors < 2) ||
        (perc_is_valid[0] && perc_is_valid[1] && !perc_1_2_is_plausible) ||
        (perc_is_valid[0] && perc_is_valid[2] && !perc_1_3_is_plausible) ||
        (perc_is_valid[1] && perc_is_valid[2] && !perc_2_3_is_plausible)) {
        throttle_set_to_implausibility();

    } else {
        throttle_set_to_valid(perc_is_valid, valid_sensors, percentages);
    }

    return throttle_handler.last_throttle_value;
}

bool throttle_has_error_occured() {
    if (throttle_handler.is_changed_to_implausible) {
        throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBLE_ERROR;
        throttle_handler.last_throttle_value = 0.0F;
        throttle_handler.is_changed_to_implausible = false;
        return true;
    }

    return false;
}

void throttle_timer_trigger() {
    throttle_handler.is_changed_to_implausible = true;
}