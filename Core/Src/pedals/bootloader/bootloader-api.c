#include "bootloader-api.h"
#include "eagletrt.h"
#include "pedals/timebase/timebase.h"
#include "pedals/watchdogs/watchdogs-api.h"

#include <string.h>

#if BOOTLOADER_INACTIVITY_TIMEOUT_MS < TIMEBASE_RESOLUTION_MS || \
    BOOTLOADER_INACTIVITY_TIMEOUT_MS % TIMEBASE_RESOLUTION_MS != 0U
#error "The flashing timeout must be an exact, positive number of timebase ticks"
#endif

EAGLETRT_STATIC struct BootloaderHandler bootloader_handler;

EAGLETRT_STATIC bool prv_bootloader_is_request(const struct CanCommunicationFrame *frame);

EAGLETRT_STATIC void prv_bootloader_inactivity_timeout(void) {
    bootloader_handler.bootloader_flashing = false;
}

enum BootloaderReturnCode bootloader_init(reboot_callback reboot) {
    memset(&bootloader_handler, 0, sizeof(bootloader_handler));

    if (reboot == nullptr) {
            return BOOTLOADER_RC_NULL_POINTER;
    }

    if (watchdogs_init_watchdog(&bootloader_handler.inactivity_watchdog,
                                BOOTLOADER_INACTIVITY_TIMEOUT_MS,
                                prv_bootloader_inactivity_timeout) != WATCHDOG_RC_OK) {
        return BOOTLOADER_RC_WATCHDOG_ERROR;
    }

    bootloader_handler.timedout = false;
    bootloader_handler.flashing = false;

    return BOOTLOADER_RC_OK;
}

/*!
 * \brief Checks for the XCP CONNECT the flashing tool sends to the bootloader.
 *
 * \details Same check as the official OpenBLT demo applications: matching identifier,
 *     2 bytes long and XCP CONNECT command code. The connection mode byte is ignored.
 */
// EAGLETRT_STATIC bool prv_bootloader_is_request(const struct CanCommunicationFrame *frame) {
//     return (frame->id == BOOTLOADER_CAN_RX_ID) &&
//            (frame->length == BOOTLOADER_XCP_CONNECT_LENGTH) &&
//            (frame->data[0] == BOOTLOADER_XCP_CMD_CONNECT);
// }

bool bootloader_is_timedout(void) {
    return bootloader_handler.timedout;
}

bool bootloader_is_flashing(void) {
    return bootloader_handler.flashing;
}
