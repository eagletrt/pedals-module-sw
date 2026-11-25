#include "brake-api.h"
#include "can-communications-api.h"
#include "can-primary-api.h"
#include "bots-api.h"
#include "eagletrt-api.h"
#include "eagletrt.h"

EAGLETRT_STATIC struct BrakeHandler brake_handler;

void brake_api_update_pedal_travel_percentage(float percentage) {
    if (percentage > -0.05F && percentage < 1.05F) {
        percentage = EAGLETRT_API_CLAMP(percentage, 0.0F, 1.0F);
        //percentage = -1.0F;
    } else if (percentage < -0.05F || percentage > 1.05F) {
        percentage = -1.0F;
    }
    brake_handler.pedal_travel = percentage;
}

void brake_api_update_front_pressure(float pressure) {
    brake_handler.front_pressure = pressure;
}

void brake_api_update_rear_pressure(float pressure) {
    brake_handler.rear_pressure = pressure;
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

void brake_api_send_status(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

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

    EAGLETRT_API_UNUSED(
        can_primary_api_serialize_from_id(
            CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSBRAKE,
            &data,
            frame.data));
    can_communications_api_add_to_tx_buffer(&frame);
}
