#ifndef BOOTLOADER_API_H
#define BOOTLOADER_API_H

#include "bootloader.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! \brief Initialize the flashing state and register its inactivity watchdog. */
enum BootloaderReturnCode bootloader_api_init(reboot_callback reboot);

/*!
 * \brief Calls the MCU reboot callback
 *
 * \retval BOOTLOADER_RC_OK if the MCU succesfully called the reboot callback
 * \retval BOOTLOADER_RC_NULL_POINTER if the reboot callback is a null pointer
 * \retval BOOTLOADER_RC_ERROR if the MCU didn't call the reboot callback
 */
enum BootloaderReturnCode bootloader_api_reboot(void);

/*!
 * \brief Restart the watchdog of the module
 *
 * \retval BOOTLOADER_RC_OK if the watchdog was successfully restarted
 * \retval BOOTLOADER_RC_NULL_POINTER if the watchdog is a null pointer
 * \retval BOOTLOADER_RC_ERROR if the watchdog was not successfully restarted
 */
enum BootloaderReturnCode bootloader_api_watchdog_restart(void);

/*!
 * \brief Reset the watchdog of the module
 *
 * \retval BOOTLOADER_RC_OK
 */
enum BootloaderReturnCode bootloader_api_watchdog_pet(void);

/*!
 * \brief Wheather the module's watchdog has timedout
 *
 * \retval true The module's watchdog has timedout
 * \retval false The module's watchdog has not timedout
 */
bool bootloader_api_is_timedout(void);

/*!
 * \brief Wheather the MCU is flashing
 *
 * \retval true The MCU is flashing
 * \retval false The MCU is not flashing
 */
bool bootloader_api_is_flashing(void);

/*!
 * \brief Set wheather the MCU is flashing
 *
 * \retval BOOTLOADER_RC_OK
 */
enum BootloaderReturnCode bootloader_api_set_flashing(bool flashing);

#endif // BOOTLOADER_API_H
