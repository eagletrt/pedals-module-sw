#ifndef BLT_CONF_H
#define BLT_CONF_H

/* Configuration based on the official OpenBLT v1.22 ARMCM0_STM32C0 port
 * (Target/Demo/ARMCM0_STM32C0_Nucleo_C092RC_GCC/Boot/blt_conf.h), adapted to the
 * pedals module board (STM32C092FCP6, FDCAN1 on PA11/PA12).
 */

/****************************************************************************************
*   C P U   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* The board has a 16 MHz HSE crystal, but the bootloader runs from HSI48 exactly like the
 * main firmware does, because that is the clock configuration the CAN bus has been
 * validated with on this board. BOOT_CPU_XTAL_SPEED_KHZ is not used by the C0 port, it is
 * only kept here for documentation. BOOT_CPU_SYSTEM_SPEED_KHZ MUST match the actual
 * SYSCLK set in SystemClock_Config(), because timer.c uses it to configure the SysTick.
 */
/** \brief Frequency of the external crystal oscillator. */
#define BOOT_CPU_XTAL_SPEED_KHZ          (16000)
/** \brief Desired system speed. */
#define BOOT_CPU_SYSTEM_SPEED_KHZ        (48000)
/** \brief Motorola or Intel style byte ordering. */
#define BOOT_CPU_BYTE_ORDER_MOTOROLA     (0)
/** \brief Enable/disable hook function call right before user program start. */
#define BOOT_CPU_USER_PROGRAM_START_HOOK (0)


/****************************************************************************************
*   C O M M U N I C A T I O N   I N T E R F A C E   C O N F I G U R A T I O N
****************************************************************************************/
/* Must match the primary network of the main firmware: classic CAN @ 1 Mbit/s. The FDCAN
 * kernel clock is PCLK1 (48 MHz), see HAL_FDCAN_MspInit().
 *
 * BOOT_COM_CAN_RX_MSG_ID is also listened to by the main firmware: when it receives an
 * XCP CONNECT on this identifier it resets into the bootloader. Keep it in sync with
 * PEDALS_BOOTLOADER_CAN_RX_ID in the main firmware.
 */
/** \brief Enable/disable CAN transport layer. */
#define BOOT_COM_CAN_ENABLE             (1)
/** \brief Configure the desired CAN baudrate. */
#define BOOT_COM_CAN_BAUDRATE           (1000000)
/** \brief Configure CAN message ID target->host. */
#define BOOT_COM_CAN_TX_MSG_ID          (0x19U)
/** \brief Configure number of bytes in the target->host CAN message. */
#define BOOT_COM_CAN_TX_MAX_DATA        (8)
/** \brief Configure CAN message ID host->target. */
#define BOOT_COM_CAN_RX_MSG_ID          (0x20U)
/** \brief Configure number of bytes in the host->target CAN message. */
#define BOOT_COM_CAN_RX_MAX_DATA        (8)
/** \brief Configure CAN classic (0) or CAN FD (1). */
#define BOOT_COM_CAN_FD_ENABLE          (0)
/** \brief Select the desired CAN peripheral as a zero based index. */
#define BOOT_COM_CAN_CHANNEL_INDEX      (0)

/** \brief Enable/disable UART transport layer. */
#define BOOT_COM_RS232_ENABLE           (0)


/****************************************************************************************
*   B A C K D O O R   E N T R Y   C O N F I G U R A T I O N
****************************************************************************************/
/* After a reset the bootloader waits this long for an XCP CONNECT before starting a valid
 * user program. The host tool keeps sending CONNECT, so the window only has to be longer
 * than its retry period.
 */
/** \brief Time in milliseconds the bootloader waits for a connection after reset. */
#define BOOT_BACKDOOR_ENTRY_TIMEOUT_MS  (500)
/** \brief Enable/disable the backdoor override hook functions. */
#define BOOT_BACKDOOR_HOOKS_ENABLE      (0)


/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* A custom flash layout (Core/Src/flash_layout.c, same as the official C092 demo)
 * reserves the first 12 KB (0x08000000 - 0x08002FFF) for the bootloader, so the user
 * program starts at 0x08003000 (see STM32C092XX_FLASH_SHIFTED.ld in the main firmware).
 * The default layout of the C0 port only reserves 8 KB, which the CAN release build
 * fills up almost completely. The bootloader linker script limits FLASH to 12 KB so that
 * the build fails if it ever outgrows this area.
 */
/** \brief Enable/disable the NVM hook function for supporting additional memory devices. */
#define BOOT_NVM_HOOKS_ENABLE           (0)
/** \brief Configure the size of the default memory device (typically flash EEPROM). */
#define BOOT_NVM_SIZE_KB                (256)
/** \brief Enable/disable hooks functions to override the user program checksum handling. */
#define BOOT_NVM_CHECKSUM_HOOKS_ENABLE  (0)
/** \brief Enable support for a custom flash layout table. It is located in
 *         flash_layout.c, which is included by flash.c.
 */
#define BOOT_FLASH_CUSTOM_LAYOUT_ENABLE (1)


/****************************************************************************************
*   W A T C H D O G   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/** \brief Enable/disable the hook functions for controlling the watchdog. */
#define BOOT_COP_HOOKS_ENABLE           (0)


/****************************************************************************************
*   E V E N T S   C O N F I G U R A T I O N
****************************************************************************************/
/** \brief Enable/disable the events module. */
#define BOOT_EVENTS_ENABLE              (0)


/****************************************************************************************
*   S E E D / K E Y   S E C U R I T Y   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_XCP_SEED_KEY_ENABLE        (0)


#endif /* BLT_CONF_H */
/*********************************** end of blt_conf.h *********************************/
