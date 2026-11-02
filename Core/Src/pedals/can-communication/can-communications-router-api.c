#include "can-primary-api.h"
#include "can-communications-router-api.h"
#include "bootloader-api.h"

EAGLETRT_STATIC void prv_dispatch_rx(uint32_t id, union CanPrimaryMessages message) {
    switch (id) {
        case CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSFLASHINGRX: {
            bootloader_api_reboot();
            break;
        }
        default: {
            if (id >= BOOTLOADER_CAN_FLASH_ID_MIN && id <= BOOTLOADER_CAN_FLASH_ID_MAX) {
                bootloader_api_set_flashing(true);
                bootloader_api_watchdog_pet();
            }

            break;
        }
    }
}

enum CanCommunicationReturnCode can_communications_router_api_receive_primary(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    if (!can_primary_api_id_is_valid(frame->id)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    union CanPrimaryMessages message = { 0 };
    if (can_primary_api_deserialize_from_id(frame->id, (uint8_t *)frame->data, &message) != 0) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    prv_dispatch_rx(frame->id, message);

    return CAN_COMMUNICATION_RC_OK;
}
