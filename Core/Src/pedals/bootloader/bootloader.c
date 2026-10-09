#include "bootloader-api.h"
#include "eagletrt.h"
#include "pedals/timebase/timebase.h"
#include "pedals/watchdogs/watchdogs-api.h"

#include <string.h>

#if BOOTLOADER_CAN_FLASH_ID_MIN > BOOTLOADER_CAN_FLASH_ID_MAX
#error "Invalid flashing identifier range"
#endif
#if BOOTLOADER_CAN_RX_ID < BOOTLOADER_CAN_FLASH_ID_MIN || \
    BOOTLOADER_CAN_RX_ID > BOOTLOADER_CAN_FLASH_ID_MAX
#error "The flashing range must include the pedals bootloader identifier"
#endif
#if BOOTLOADER_INACTIVITY_TIMEOUT_MS < TIMEBASE_RESOLUTION_MS || \
    BOOTLOADER_INACTIVITY_TIMEOUT_MS % TIMEBASE_RESOLUTION_MS != 0U
#error "The flashing timeout must be an exact, positive number of timebase ticks"
#endif

struct BootloaderHandler {
    struct Watchdog inactivity_watchdog;
    bool initialized;
};

EAGLETRT_STATIC struct BootloaderHandler bootloader_handler;
EAGLETRT_STATIC bool bootloader_requested;
EAGLETRT_STATIC bool bootloader_flashing;

EAGLETRT_STATIC bool prv_bootloader_is_request(const struct CanCommunicationFrame *frame);

EAGLETRT_STATIC void prv_bootloader_inactivity_timeout(void) {
    bootloader_flashing = false;
}

enum BootloaderReturnCode bootloader_init(void) {
    memset(&bootloader_handler, 0, sizeof(bootloader_handler));
    bootloader_requested = false;
    bootloader_flashing = false;

    if (watchdogs_init_watchdog(&bootloader_handler.inactivity_watchdog,
                                BOOTLOADER_INACTIVITY_TIMEOUT_MS,
                                prv_bootloader_inactivity_timeout) != WATCHDOG_RC_OK) {
        return BOOTLOADER_RC_WATCHDOG_ERROR;
    }

    bootloader_handler.initialized = true;
    return BOOTLOADER_RC_OK;
}

enum CanCommunicationReturnCode bootloader_receive(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (!bootloader_handler.initialized) {
        return CAN_COMMUNICATION_RC_ERROR;
    }
    if (frame->length > CAN_COMMUNICATIONS_FRAME_DATA_SIZE) {
        return CAN_COMMUNICATION_RC_INVALID_LENGTH;
    }
    if (frame->id < BOOTLOADER_CAN_FLASH_ID_MIN || frame->id > BOOTLOADER_CAN_FLASH_ID_MAX) {
        return CAN_COMMUNICATION_RC_OK;
    }

    bootloader_flashing = true;
    if (watchdogs_restart(&bootloader_handler.inactivity_watchdog) != WATCHDOG_RC_OK) {
        bootloader_flashing = false;
        return CAN_COMMUNICATION_RC_ERROR;
    }

    if (prv_bootloader_is_request(frame)) {
        bootloader_requested = true;
    }
    return CAN_COMMUNICATION_RC_OK;
}

/*!
 * \brief Checks for the XCP CONNECT the flashing tool sends to the bootloader.
 *
 * \details Same check as the official OpenBLT demo applications: matching identifier,
 *     2 bytes long and XCP CONNECT command code. The connection mode byte is ignored.
 */
EAGLETRT_STATIC bool prv_bootloader_is_request(const struct CanCommunicationFrame *frame) {
    return (frame->id == BOOTLOADER_CAN_RX_ID) &&
           (frame->length == BOOTLOADER_XCP_CONNECT_LENGTH) &&
           (frame->data[0] == BOOTLOADER_XCP_CMD_CONNECT);
}

bool bootloader_is_requested(void) {
    return bootloader_requested;
}

bool bootloader_is_flashing(void) {
    return bootloader_flashing;
}
