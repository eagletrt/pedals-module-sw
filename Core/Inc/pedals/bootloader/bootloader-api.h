#ifndef BOOTLOADER_API_H
#define BOOTLOADER_API_H

#include "can-communications.h"
#include "bootloader.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! \brief Initialize the flashing state and register its inactivity watchdog. */
enum BootloaderReturnCode bootloader_init(void);

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

#endif // BOOTLOADER_API_H
