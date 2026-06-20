#include "throttle-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct ThrottleHandler throttle_handler = {
    .is_implausibility_timeout = false,
    .last_throttle_value = THROTTLE_MIN_VALUE,
    .throttle_status = THROTTLE_STATUS_OK,
    .error_status = THROTTLE_RC_NO_ERROR,
    .start_timer = NULL,
    .stop_timer = NULL
};

// internal functions ---------------------------------------

float prv_throttle_calculate_next_value(float apps1, float apps2, float apps3) {
    float values[] = { apps1, apps2, apps3 };
    int n_valid_pairs = 0;
    int last_good_idx = -1; // If > 0, it represents that pair i and i+1 (circular mode) is within ruleset

    // To know what sensors to exclude we need to know if they are in range and have at least 1 other sensor within max allowed difference
    // Cases can be hardcodable but even with three there are too many combinations of values, so we decide to count the number of good pairs
    // This works because there are only three sensors to check, if more a more sofisticated approach is required
    for (int i = 0; i < THROTTLE_APPS_NUMBER; i++) {
        // check if it's in range [0,1] as per T 11.9.2
        // We just need to care if the current sensors if invalid because only the difference between invalids would throw off the next check
        // Any other check, even if next sensor is invalid, would fail the next if
        if (values[i] != THROTTLE_ERROR_VALUE) {
            float diff = values[i] - values[(i + 1) % 3]; //circular mode,
            if (-THROTTLE_MAX_PERCENTAGE_DEVIATION <= diff && diff <= THROTTLE_MAX_PERCENTAGE_DEVIATION) {
                last_good_idx = i;
                n_valid_pairs++;
            }
        }
    }

    float result;
    // values are implausible if there are less than 2 working sensors or if there is no working pair of sensors that has less than 10% difference as per T 11.8.9 and T 11.9
    // if there are three valid pairs, every value is correct on its own and can be included in the result
    if (n_valid_pairs == 3) {
        result = (apps1 + apps2 + apps3) / (float)THROTTLE_APPS_NUMBER;
    } //if there are two or one valid pair, it means that we can choose whichever pair
    else if (n_valid_pairs > 0) {
        result = (values[last_good_idx] + values[(last_good_idx + 1) % 3]) / (float)THROTTLE_MIN_NUMBER_VALID_APPS;
    } // if zero valid pairs, you must return that values have become implausible
    else {
        result = THROTTLE_ERROR_VALUE;
    }

    return result;
}

void prv_throttle_update_error(enum ThrottleReturnCode error) {
    if (error == THROTTLE_RC_IMPLAUSIBILITY || (error == THROTTLE_RC_CALLBACK_FAILURE && throttle_handler.error_status != THROTTLE_RC_IMPLAUSIBILITY)) {
        throttle_handler.error_status = error;
    }
}

void prv_throttle_status_ok_routine(float new_value) {
    enum ThrottleReturnCode err = THROTTLE_RC_NO_ERROR;

    if (new_value != THROTTLE_ERROR_VALUE) {
        throttle_handler.last_throttle_value = new_value;
    } else {
        throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;
        if (throttle_handler.start_timer != NULL) {
            err = throttle_handler.start_timer();
        } else {
            err = THROTTLE_RC_CALLBACK_FAILURE;
        }
    }

    prv_throttle_update_error(err);
}

void prv_throttle_status_recoverable_routine(float new_value) {
    enum ThrottleReturnCode err = THROTTLE_RC_NO_ERROR;

    if (new_value != THROTTLE_ERROR_VALUE) {
        throttle_handler.throttle_status = THROTTLE_STATUS_OK;
        throttle_handler.last_throttle_value = new_value;
        if (throttle_handler.stop_timer != NULL) {
            err = throttle_handler.stop_timer();
        } else {
            err = THROTTLE_RC_CALLBACK_FAILURE;
        }
    }

    prv_throttle_update_error(err);
}

void prv_throttle_status_implausible_routine() {
    throttle_handler.last_throttle_value = THROTTLE_MIN_VALUE;
    throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_ERROR;
    throttle_handler.is_implausibility_timeout = false;
    prv_throttle_update_error(THROTTLE_RC_IMPLAUSIBILITY);
}

void prv_throttle_next_state(float new_value) {
    // regardless of the current state, if the flag is found activated you must set to IMPLAUSIBLE_ERROR and notify the user
    if (throttle_handler.is_implausibility_timeout) {
        prv_throttle_status_implausible_routine();
    }

    switch (throttle_handler.throttle_status) {
        case THROTTLE_STATUS_OK: {
            prv_throttle_status_ok_routine(new_value);
            break;
        }
        case THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE: {
            prv_throttle_status_recoverable_routine(new_value);
            break;
        }

        default: {
        } //do nothing, in IMPLAUSIBLE ERROR you can't change state anymore
    }
}

// actual api ---------------------------------------

enum ThrottleReturnCode throttle_init(throttle_timer_callback start_timer, throttle_timer_callback stop_timer) {
    if (start_timer == NULL || stop_timer == NULL) {
        return THROTTLE_RC_CALLBACK_FAILURE;
    }

    throttle_handler.start_timer = start_timer;
    throttle_handler.stop_timer = stop_timer;
    return THROTTLE_RC_NO_ERROR;
}

void throttle_update_pedal_values(float apps1, float apps2, float apps3) {
    // if status is THROTTLE_STATUS_IMPLAUSIBLE_ERROR you can't recover from the error, leave the throttle state as it is
    if (throttle_handler.throttle_status == THROTTLE_STATUS_IMPLAUSIBLE_ERROR) {
        return;
    }
    apps1 = ((apps1 > THROTTLE_MAX_VALUE) || (apps1 < THROTTLE_MIN_VALUE)) ? THROTTLE_ERROR_VALUE : apps1;
    apps2 = ((apps2 > THROTTLE_MAX_VALUE) || (apps2 < THROTTLE_MIN_VALUE)) ? THROTTLE_ERROR_VALUE : apps2;
    apps3 = ((apps3 > THROTTLE_MAX_VALUE) || (apps3 < THROTTLE_MIN_VALUE)) ? THROTTLE_ERROR_VALUE : apps3;

    float next_val = prv_throttle_calculate_next_value(apps1, apps2, apps3);

    prv_throttle_next_state(next_val);
}

struct ThrottleReturnValue throttle_get_travel_percentage() {
    struct ThrottleReturnValue ret = {
        .throttle_error = throttle_handler.error_status,
        .throttle_value = throttle_handler.last_throttle_value
    };
    throttle_handler.error_status = THROTTLE_RC_NO_ERROR;

    return ret;
}

void throttle_implausibility_timeout_trigger() {
    throttle_handler.is_implausibility_timeout = true;
}