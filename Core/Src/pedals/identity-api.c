#include "identity-api.h"
#include "can-primary-api.h"
#include "can-communications-api.h"

EAGLETRT_STATIC struct IdentityHandler identity_handler = {
    .last_version_tick = 0,
    .last_status_tick = 0
};

enum IdentityReturnCode identity_api_send_pedals_version(uint32_t tick) {
    if (tick - identity_handler.last_version_tick < IDENTITY_VERSION_CAN_PERIOD_MS) {
        return IDENTITY_RC_OK;
    }
    identity_handler.last_version_tick = tick;
    struct CanCommunicationFrame frame = { 0 };
    union CanPrimaryMessages status_msg = { 0 };
    status_msg.pedals_version.buildtime_s = 0;
    status_msg.pedals_version.canlibbuildtime_s = can_generation_time;
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_VERSION, &status_msg, frame.data) == -1) {
        return IDENTITY_RC_ERROR;
    }
    frame.id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_VERSION;
    frame.length = can_primary_byte_size_pedals_version;
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return IDENTITY_RC_ERROR;
    }
    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_send_pedals_status(uint32_t tick, state_t fsm_state) {
    if (tick - identity_handler.last_status_tick < IDENTITY_STATUS_CAN_PERIOD_MS) {
        return IDENTITY_RC_OK;
    }
    identity_handler.last_status_tick = tick;
    struct CanCommunicationFrame frame = { 0 };
    union CanPrimaryMessages status_msg = { 0 };
    status_msg.pedals_status.name = fsm_state;
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_STATUS, &status_msg, frame.data) == -1) {
        return IDENTITY_RC_ERROR;
    }
    frame.id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_STATUS;
    frame.length = can_primary_byte_size_pedals_status;
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return IDENTITY_RC_ERROR;
    }
    return IDENTITY_RC_OK;
}
