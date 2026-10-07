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

/*! \brief Initialize the module-owned timebase and flashing inactivity watchdog. */
enum BootloaderReturnCode bootloader_init(void);

/*!
 * \brief Increment the module timebase by one tick.
 * \details Called by the 1 ms TIM3 interrupt, independently from HAL SysTick.
 */
enum BootloaderReturnCode bootloader_timebase_tick(void);

/*!
 * \brief Run the watchdog scheduler using the current timebase tick.
 * \details Call from the main loop after draining the CAN RX queue.
 */
enum BootloaderReturnCode bootloader_update(void);

/*!
 * \brief Handle flashing traffic in main-loop context; ignore other identifiers.
 * \details Every frame in the flashing range refreshes the inactivity timer.
 * Only a two-byte XCP CONNECT addressed to pedals requests a MCU reset.
 */
enum CanCommunicationReturnCode bootloader_receive(const struct CanCommunicationFrame *frame);

/*!
 * \brief Tells whether an XCP CONNECT for the bootloader has been received.
 *
 * \details Once set, the request stays pending until the MCU is reset.
 *
 * \retval true if the firmware should reset into the bootloader.
 * \retval false otherwise.
 */
bool bootloader_is_requested(void);

/*! \brief True while the inactivity watchdog is running. */
bool bootloader_is_flashing(void);

#endif // BOOTLOADER_H
