#include "can-communications-router-api.h"
#include "bootloader-api.h"

enum CanCommunicationReturnCode can_communications_router_api_receive_primary(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    const enum CanCommunicationReturnCode bootloader_result = bootloader_receive(frame);
    if (bootloader_result != CAN_COMMUNICATION_RC_OK) {
        return bootloader_result;
    }

    // TODO: add libcan deserialization and dispatch logic here

    return CAN_COMMUNICATION_RC_OK;
}
