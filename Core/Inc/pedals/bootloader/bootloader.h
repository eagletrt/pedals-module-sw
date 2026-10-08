#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "can-communications.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*!
 * \brief Inclusive CAN identifier range reserved for flashing traffic.
 *
 * \details Currently covers the known OpenBLT identifiers. Keep both bounds
 * aligned with the flashing range assigned to the whole primary network.
 */
#define BOOTLOADER_CAN_FLASH_ID_MIN (0x19U)
#define BOOTLOADER_CAN_FLASH_ID_MAX (0x20U)

/*!
 * \brief CAN identifier the OpenBLT bootloader receives on (host -> target).
 *
 * \details The flashing tool (e.g. BootCommander) keeps sending XCP CONNECT on this
 *     identifier. While the main firmware is running it reacts to it by resetting into
 *     the bootloader, which then answers the next CONNECT. Must match
 *     BOOT_COM_CAN_RX_MSG_ID in pedals-module-bootloader-sw/Core/Inc/blt_conf.h.
 */
#define BOOTLOADER_CAN_RX_ID (0x20U)

/*! \brief XCP CONNECT command code, first byte of the bootloader request frame. */
#define BOOTLOADER_XCP_CMD_CONNECT (0xFFU)

/*! \brief Length of the XCP CONNECT frame (command code + connection mode). */
#define BOOTLOADER_XCP_CONNECT_LENGTH (2U)

/*!
 * \brief Return to normal operation after this many ms without flashing traffic.
 */
#define BOOTLOADER_INACTIVITY_TIMEOUT_MS (500U)

/*! \brief Resolution of the module-owned timebase, driven by TIM3. */
#define BOOTLOADER_TIMEBASE_RESOLUTION_MS (1U)

enum BootloaderReturnCode {
    BOOTLOADER_RC_OK,
    BOOTLOADER_RC_TIMEBASE_ERROR,
    BOOTLOADER_RC_WATCHDOG_ERROR,
};

#endif // BOOTLOADER_H
