#ifndef BLT_CONF_H
#define BLT_CONF_H

/****************************************************************************************
*   C P U   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* External crystal frequency (if you only use HSI = 8 MHz, just leave at 8000). */
#define BOOT_CPU_XTAL_SPEED_KHZ (16000) 

/* System clock frequency (typical STM32C0 max ~48 MHz). */
#define BOOT_CPU_SYSTEM_SPEED_KHZ (16000) // temporary frequency, tbd what to do

/* STM32C0 is little endian. */
#define BOOT_CPU_BYTE_ORDER_MOTOROLA (0)

/* Optional hook before jumping to user application. */
#define BOOT_CPU_USER_PROGRAM_START_HOOK (0)

/****************************************************************************************
*   C O M M U N I C A T I O N   I N T E R F A C E   C O N F I G U R A T I O N
****************************************************************************************/
/* CAN communication (C0 has FDCAN). */
#define BOOT_COM_CAN_ENABLE (1)
#define BOOT_COM_CAN_BAUDRATE (1000000) /* pick what matches your bus */
#define BOOT_COM_CAN_TX_MSG_ID (0x17u)
#define BOOT_COM_CAN_TX_MAX_DATA (8)
#define BOOT_COM_CAN_RX_MSG_ID (0x18u)
#define BOOT_COM_CAN_RX_MAX_DATA (8)
#define BOOT_COM_CAN_CHANNEL_INDEX (0)

/* UART (RS232) – disabled unless you want UART bootloader. */
#define BOOT_COM_RS232_ENABLE (0)
#define BOOT_COM_RS232_BAUDRATE (115200)
#define BOOT_COM_RS232_TX_MAX_DATA (64)
#define BOOT_COM_RS232_RX_MAX_DATA (64)
#define BOOT_COM_RS232_CHANNEL_INDEX (0)

/****************************************************************************************
*   B A C K D O O R   E N T R Y   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_BACKDOOR_HOOKS_ENABLE (0)

/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
/* STM32C0 devices have up to 64 KB Flash. Adjust if yours is smaller (e.g. 32). */
#define BOOT_NVM_HOOKS_ENABLE (0)
#define BOOT_NVM_SIZE_KB (256) //stm32c09fc has 256 kb
#define BOOT_NVM_CHECKSUM_HOOKS_ENABLE (0)

/****************************************************************************************
*   W A T C H D O G   D R I V E R   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_COP_HOOKS_ENABLE (0)

/****************************************************************************************
*   S E E D / K E Y   S E C U R I T Y   C O N F I G U R A T I O N
****************************************************************************************/
#define BOOT_XCP_SEED_KEY_ENABLE (0)

#endif /* BLT_CONF_H */
/*********************************** end of blt_conf.h *********************************/
