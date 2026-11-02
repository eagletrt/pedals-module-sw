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

EAGLETRT_STATIC void prv_bootloader_inactivity_timeout(void) {
    bootloader_handler.flashing = false;
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

enum BootloaderReturnCode bootloader_api_reboot(void) {
    bootloader_handler.reboot();

    return BOOTLOADER_RC_OK;
}

enum BootloaderReturnCode bootloader_api_watchdog_restart(void) {
    // TODO
    return BOOTLOADER_RC_OK;
}

enum BootloaderReturnCode bootloader_api_watchdog_pet(void) {
    // TODO
    return BOOTLOADER_RC_OK;
}

bool bootloader_is_timedout(void) {
    return bootloader_handler.timedout;
}

bool bootloader_is_flashing(void) {
    return bootloader_handler.flashing;
}

enum BootloaderReturnCode bootloader_api_set_flashing(bool flashing) {
    bootloader_handler.flashing = flashing;

    return BOOTLOADER_RC_OK;
}
