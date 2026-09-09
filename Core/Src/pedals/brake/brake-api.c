#include "brake-api.h"
#include "can-communications-api.h"
#include "can-primary-api.h"
#include "bots-api.h"
#include "eagletrt.h"

EAGLETRT_STATIC struct BrakeHandler brake_handler;

void brake_api_update_pedal_travel_percentage(float percentage) {
    if (percentage < 0.0F || percentage > 1.0F) {
        percentage = -1.0F;
    }
    brake_handler.pedal_travel = percentage;
}

void brake_api_update_front_pressure(float pressure) {
    brake_handler.front_pressure = pressure;
}

void brake_api_update_rear_pressure(float pressure) {
    brake_handler.front_pressure = pressure;
}

float brake_api_get_pedal_travel_percentage() {
    return brake_handler.pedal_travel;
}

float brake_api_get_front_pressure() {
    return brake_handler.front_pressure;
}

float brake_api_get_rear_pressure() {
    return brake_handler.rear_pressure;
}

enum BrakeReturnCode brake_api_send_status(uint32_t tick) {
    if (tick - brake_handler.last_tick < can_primary_cycle_time_pedalsbrake) {
        return BRAKE_RC_OK;
    }
    brake_handler.last_tick = tick;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSBRAKE,
        .length = can_primary_byte_size_pedalsbrake
    };
    union CanPrimaryMessages data = {
        .pedalsbrake.pressurefl = brake_handler.front_pressure,
        .pedalsbrake.pressurefr = brake_handler.front_pressure,
        .pedalsbrake.pressurerl = brake_handler.rear_pressure,
        .pedalsbrake.pressurerr = brake_handler.rear_pressure,
        .pedalsbrake.travel = brake_handler.pedal_travel,
        .pedalsbrake.botsvoltage = bots_get_voltage(),
    };

    //logger_api_log(LOGGER_LEVEL_DEBUG, "Brake: Sending Status: travel=%f, front_pressure=%f, rear_pressure=%f, bots_voltage=%f", brake_handler.pedal_travel, brake_handler.front_pressure, brake_handler.rear_pressure, bots_get_voltage());

    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSBRAKE, &data, frame.data) == -1) {
        return BRAKE_RC_ERROR;
    }
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return BRAKE_RC_ERROR;
    }
    return BRAKE_RC_OK;
}
