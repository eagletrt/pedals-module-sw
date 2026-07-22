#include "throttle-api.h"
#include "can-primary-api.h"
#include "can-communications-api.h"

EAGLETRT_STATIC struct ThrottleHandler throttle_handler;

// internal functions ---------------------------------------

float prv_throttle_calculate_next_value(const float apps[THROTTLE_ID_COUNT]) {
    int valid_apps_pair_count = 0;
    int last_valid_apps_pair_index = -1; // If > 0, it represents that pair i and i+1 (circular mode) is within ruleset

    // To know what sensors to exclude we need to know if they are in range and have at least 1 other sensor within max allowed difference
    // Cases can be hardcodable but even with three there are too many combinations of values, so we decide to count the number of good pairs
    // This works because there are only three sensors to check, if more a more sofisticated approach is required
    for (enum ThrottleId i = THROTTLE_ID_APPS_1; i < THROTTLE_ID_COUNT; i++) {
        // check if it's in range [0,1] as per T 11.9.2
        // We just need to care if the current sensors if invalid because only the difference between invalids would throw off the next check
        // Any other check, even if next sensor is invalid, would fail the next if
        if (apps[i] != THROTTLE_ERROR_VALUE) {
            float diff = apps[i] - apps[(i + 1) % 3];
            if (diff < 0.0F) {
                diff *= -1.0F;
            }
            if (diff <= THROTTLE_MAX_PERCENTAGE_DEVIATION) {
                last_valid_apps_pair_index = i;
                ++valid_apps_pair_count;
            }
        }
    }

    // values are implausible only if there are less than 2 working sensors or if there is no working pair of sensors that has less than 10% difference as per T 11.8.9 and T 11.9
    //     - If there are three valid pairs, every value is correct on its own and can be included in the result
    //     - If there are two or one valid pair, it means that we can choose whichever pair
    //     - If zero valid pairs, you must return that values have become implausible
    float result = THROTTLE_ERROR_VALUE;
    if (valid_apps_pair_count == 3) {
        result = (apps[THROTTLE_ID_APPS_1] + apps[THROTTLE_ID_APPS_2] + apps[THROTTLE_ID_APPS_3]) / (float)THROTTLE_ID_COUNT;
    } else if (valid_apps_pair_count > 0) {
        result = (apps[last_valid_apps_pair_index] + apps[(last_valid_apps_pair_index + 1) % 3]) / (float)THROTTLE_MIN_NUMBER_VALID_APPS;
    }
    return result;
}

void prv_throttle_calculate_next_state(float new_value) {
    // regardless of the current state, if the flag is found activated you must set to IMPLAUSIBLE_ERROR and notify the user
    if (throttle_handler.is_implausibility_timeout) {
        throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBILITY_ERROR;
        throttle_handler.is_implausibility_timeout = false;
        return;
    }

    switch (throttle_handler.status) {
        case THROTTLE_STATUS_OK: {
            if (new_value != THROTTLE_ERROR_VALUE) {
                throttle_handler.travel_percentage = new_value;
            } else if (throttle_handler.start_timer == NULL || throttle_handler.start_timer() != THROTTLE_RC_OK) {
                throttle_handler.status = THROTTLE_STATUS_CALLBACK_ERROR;
            } else {
                throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE;
            }
            break;
        }
        case THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE: {
            if (new_value != THROTTLE_ERROR_VALUE) {
                if (throttle_handler.stop_timer == NULL || throttle_handler.stop_timer() != THROTTLE_RC_OK) {
                    throttle_handler.status = THROTTLE_STATUS_CALLBACK_ERROR;
                } else {
                    throttle_handler.status = THROTTLE_STATUS_OK;
                    throttle_handler.travel_percentage = new_value;
                }
            }
            break;
        }

        default: {
        } //do nothing, in IMPLAUSIBLE ERROR and CALLBACK ERROR you can't change state anymore
    }
}

enum ThrottleReturnCode throttle_api_init(throttle_timer_callback start_timer, throttle_timer_callback stop_timer) {
    if (start_timer == NULL || stop_timer == NULL) {
        return THROTTLE_RC_NULL_POINTER;
    }

    memset(&throttle_handler, 0, sizeof(throttle_handler));

    throttle_handler.start_timer = start_timer;
    throttle_handler.stop_timer = stop_timer;
    return THROTTLE_RC_OK;
}

void throttle_api_update_pedal_values(float apps1, float apps2, float apps3) {
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
}

void throttle_api_update_internal_status(uint32_t tick) {
    constexpr float throttle_max_percentage = 1.0F;
    constexpr float throttle_min_percentage = 0.0F;

    if (tick - throttle_handler.last_update_tick < THROTTLE_UPDATE_PEDIOD_MS) {
        return;
    }
    throttle_handler.last_update_tick = tick;

    // if status is THROTTLE_STATUS_IMPLAUSIBILITY_ERROR you can't recover from the error, leave the throttle state as it is
    if (throttle_handler.status != THROTTLE_STATUS_OK && throttle_handler.status != THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE) {
        return;
    }

    float apps[THROTTLE_ID_COUNT];

    for (enum ThrottleId i = THROTTLE_ID_APPS_1; i < THROTTLE_ID_COUNT; i++) {
        apps[i] = throttle_handler.apps_travel_percentages[i];
        apps[i] = ((apps[i] > throttle_max_percentage) || (apps[i] < throttle_min_percentage)) ? THROTTLE_ERROR_VALUE : apps[i];
    }

    float next_val = prv_throttle_calculate_next_value(apps);

    prv_throttle_calculate_next_state(next_val);
}

float throttle_api_get_travel_percentage() {
    return throttle_handler.travel_percentage;
}

enum ThrottleStatus throttle_api_get_status() {
    return throttle_handler.status;
}

float throttle_api_get_apps(enum ThrottleId apps_id) {
    if (apps_id >= THROTTLE_ID_COUNT) {
        return THROTTLE_ERROR_VALUE;
    }
    return throttle_handler.apps_travel_percentages[apps_id];
}

void throttle_api_implausibility_timeout_trigger() {
    throttle_handler.is_implausibility_timeout = true;
}

enum ThrottleReturnCode throttle_api_send_status(uint32_t tick) {
    if (tick - throttle_handler.last_status_tick < THROTTLE_STATUS_CAN_PERIOD_MS) {
        return THROTTLE_RC_OK;
    }
    throttle_handler.last_status_tick = tick;
    struct CanCommunicationFrame frame = { 0 };
    union CanPrimaryMessages status_msg = {
        .pedals_throttle.status = throttle_handler.status,
        .pedals_throttle.travel_pct = throttle_handler.travel_percentage
    };
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_THROTTLE, &status_msg, frame.data) == -1) {
        return THROTTLE_RC_ERROR;
    }
    frame.id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_THROTTLE;
    frame.length = can_primary_byte_size_pedals_throttle;
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return THROTTLE_RC_ERROR;
    }
    return THROTTLE_RC_OK;
}

enum ThrottleReturnCode throttle_api_send_apps(uint32_t tick) {
    if (tick - throttle_handler.last_apps_tick < THROTTLE_APPS_CAN_PERIOD_MS) {
        return THROTTLE_RC_OK;
    }
    throttle_handler.last_apps_tick = tick;
    struct CanCommunicationFrame frame = { 0 };
    union CanPrimaryMessages status_msg = {
        .pedals_apps.travelfirst_pct = throttle_handler.apps_travel_percentages[0],
        .pedals_apps.travelsecond_pct = throttle_handler.apps_travel_percentages[1],
        .pedals_apps.travelthird_pct = throttle_handler.apps_travel_percentages[2]
    };
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_APPS, &status_msg, frame.data) == -1) {
        return THROTTLE_RC_ERROR;
    }
    frame.id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_APPS;
    frame.length = can_primary_byte_size_pedals_apps;
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return THROTTLE_RC_ERROR;
    }
    return THROTTLE_RC_OK;
}
