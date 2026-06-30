#include "general-messages-api.h"
#include "can-primary-api.h"
#include "can-communications-api.h"

enum GeneralMessagesReturnCode general_messages_send_pedals_version(void) {
    struct CanCommunicationFrame frame = { 0 };
    union CanPrimaryMessages status_msg = { 0 };
    status_msg.pedals_version.buildtime_s = 0;
    status_msg.pedals_version.canlibbuildtime_s = 0;
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_VERSION, &status_msg, frame.data) == -1) {
        return GENERAL_MESSAGES_RC_ERROR;
    }
    frame.id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALS_APPS;
    frame.length = can_primary_byte_size_pedals_version;
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return GENERAL_MESSAGES_RC_ERROR;
    }
    return GENERAL_MESSAGES_RC_OK;
}

enum GeneralMessagesReturnCode general_messages_send_pedals_status(state_t fsm_state) {
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