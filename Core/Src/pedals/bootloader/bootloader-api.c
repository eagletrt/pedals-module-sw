#include "bootloader-api.h"
#include "eagletrt.h"
#include "scheduler-api.h"
#include "watchdogs-api.h"

#include <string.h>

EAGLETRT_STATIC struct BootloaderHandler bootloader_handler;

EAGLETRT_STATIC void prv_bootloader_inactivity_timeout(void) {
    bootloader_handler.timedout = true;
}

enum BootloaderReturnCode bootloader_api_init(reboot_callback reboot) {
    memset(&bootloader_handler, 0, sizeof(bootloader_handler));

    if (reboot == nullptr) {
        return BOOTLOADER_RC_NULL_POINTER;
    }

    if (watchdogs_api_init_watchdog(&bootloader_handler.inactivity_watchdog,
                                    BOOTLOADER_INACTIVITY_TIMEOUT_MS,
                                    prv_bootloader_inactivity_timeout) != WATCHDOG_RC_OK) {
        return BOOTLOADER_RC_WATCHDOG_ERROR;
    }

    bootloader_handler.timedout = false;
    bootloader_handler.flashing = false;

    return BOOTLOADER_RC_OK;
}

enum BootloaderReturnCode bootloader_api_reboot(void) {
    if (bootloader_handler.reboot == nullptr) {
        return BOOTLOADER_RC_NULL_POINTER;
    }

    bootloader_handler.reboot();

    return BOOTLOADER_RC_OK;
}

enum BootloaderReturnCode bootloader_api_watchdog_restart(void) {
    bootloader_handler.timedout = false;
    switch (scheduler_api_watchdog_restart(&bootloader_handler.inactivity_watchdog)) {
        case WATCHDOG_RC_OK:
            return BOOTLOADER_RC_OK;
        case WATCHDOG_RC_NULL_POINTER:
            return BOOTLOADER_RC_NULL_POINTER;
        default:
            return BOOTLOADER_RC_ERROR;
    }
}

enum BootloaderReturnCode bootloader_api_watchdog_pet(void) {
    switch (scheduler_api_watchdog_pet(&bootloader_handler.inactivity_watchdog)) {
        case WATCHDOG_RC_OK:
            return BOOTLOADER_RC_OK;
        case WATCHDOG_RC_NULL_POINTER:
            return BOOTLOADER_RC_NULL_POINTER;
        default:
            return BOOTLOADER_RC_ERROR;
    }
}


bool bootloader_api_is_timedout(void) {
    return bootloader_handler.timedout;
}

bool bootloader_api_is_flashing(void) {
    return bootloader_handler.flashing;
}

enum BootloaderReturnCode bootloader_api_set_flashing(bool flashing) {
    bootloader_handler.flashing = flashing;

    return BOOTLOADER_RC_OK;
}
