#include "can-communications-router-api.h"

#include "eagletrt-api.h"

EAGLETRT_STATIC bool can_communications_router_bootloader_requested = false;

/*!
 * \brief Checks for the XCP CONNECT the flashing tool sends to the bootloader.
 *
 * \details Same check as the official OpenBLT demo applications: matching identifier,
 *     2 bytes long and XCP CONNECT command code. The connection mode byte is ignored.
 */
EAGLETRT_STATIC bool prv_can_communications_router_is_bootloader_request(const struct CanCommunicationFrame *frame) {
    return (frame->id == CAN_COMMUNICATIONS_ROUTER_BOOTLOADER_RX_ID) &&
           (frame->length == CAN_COMMUNICATIONS_ROUTER_XCP_CONNECT_LENGTH) &&
           (frame->data[0] == CAN_COMMUNICATIONS_ROUTER_XCP_CMD_CONNECT);
}

enum CanCommunicationReturnCode can_communications_router_api_receive_primary(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    if (prv_can_communications_router_is_bootloader_request(frame)) {
        can_communications_router_bootloader_requested = true;
        return CAN_COMMUNICATION_RC_OK;
    }

    // TODO: add libcan deserialization and dispatch logic here

    return CAN_COMMUNICATION_RC_OK;
}

bool can_communications_router_api_is_bootloader_requested(void) {
    return can_communications_router_bootloader_requested;
}
