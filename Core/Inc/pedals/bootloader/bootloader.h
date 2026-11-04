#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "watchdogs.h"

/*!
 * \brief Inclusive CAN identifier range reserved for flashing traffic.
 */
#define BOOTLOADER_CAN_FLASH_ID_MIN CAN_PRIMARY_MESSAGE_FRAME_ID_ECUFLASHINGTX
#define BOOTLOADER_CAN_FLASH_ID_MAX CAN_PRIMARY_MESSAGE_FRAME_ID_STEERINGBOARDFLASHINGRX

/*!
 * \brief Return to normal operation after this many ms without flashing traffic.
 */
#define BOOTLOADER_INACTIVITY_TIMEOUT_MS (500U)

/*!
 * \brief Reboot callback
 */
typedef void (*reboot_callback)(void);

/*!
 * \brief The handler containing the handlers for the watchdog and the state of the
 * module
 */
struct BootloaderHandler {
    struct Watchdog inactivity_watchdog;
    reboot_callback reboot;
    bool flashing;
    bool timedout;
};

/*!
 * \brief Return codes for the bootloader module functions
 */
enum BootloaderReturnCode {
    BOOTLOADER_RC_OK = 0,         /*!< The function executed succesfully */
    BOOTLOADER_RC_NULL_POINTER,   /*!< An argument of pointer type was null or a null pointer */
    BOOTLOADER_RC_WATCHDOG_ERROR, /*!< An error occured with the watchdog */
    BOOTLOADER_RC_ERROR,          /*!< The function encountered an error */
};

#endif // BOOTLOADER_H
