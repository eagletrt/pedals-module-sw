#include "general-messages-api.h"
#include "can-primary-api.h"
#include "can-communications-api.h"

EAGLETRT_STATIC uint32_t gen_mess_last_version_tick = 0;
EAGLETRT_STATIC uint32_t gen_mess_last_status_tick = 0;

enum GeneralMessagesReturnCode general_messages_api_send_pedals_version(uint32_t tick) {
    if (tick - gen_mess_last_version_tick < GENERAL_MESSAGES_VERSION_CAN_PERIOD_MS) {
        return GENERAL_MESSAGES_RC_OK;
    }
    gen_mess_last_version_tick = tick;
    struct CanCommunicationFrame frame = { 0 };
    union CanPrimaryMessages status_msg = { 0 };
    status_msg.pedals_version.buildtime_s = 0;
    status_msg.pedals_version.canlibbuildtime_s = can_generation_time;
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_VERSION, &status_msg, frame.data) == -1) {
        return GENERAL_MESSAGES_RC_ERROR;
    }
    frame.id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_VERSION;
    frame.length = can_primary_byte_size_pedals_version;
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return GENERAL_MESSAGES_RC_ERROR;
    }
    return GENERAL_MESSAGES_RC_OK;
}

enum GeneralMessagesReturnCode general_messages_api_send_pedals_status(uint32_t tick, state_t fsm_state) {
    if (tick - gen_mess_last_status_tick < GENERAL_MESSAGES_STATUS_CAN_PERIOD_MS) {
        return GENERAL_MESSAGES_RC_OK;
    }
    gen_mess_last_status_tick = tick;
    struct CanCommunicationFrame frame = { 0 };
    union CanPrimaryMessages status_msg = { 0 };
    status_msg.pedals_status.name = fsm_state;
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_STATUS, &status_msg, frame.data) == -1) {
        return GENERAL_MESSAGES_RC_ERROR;
    }
    frame.id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_STATUS;
    frame.length = can_primary_byte_size_pedals_status;
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return GENERAL_MESSAGES_RC_ERROR;
    }
    return GENERAL_MESSAGES_RC_OK;
}
