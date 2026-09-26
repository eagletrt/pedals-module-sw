#ifndef CAN_COMMUNICATIONS_ROUTER_H
#define CAN_COMMUNICATIONS_ROUTER_H

#include "can-communications.h"

#include <stdbool.h>

/*!
 * \brief CAN identifier the OpenBLT bootloader receives on (host -> target).
 *
 * \details The flashing tool (e.g. BootCommander) keeps sending XCP CONNECT on this
 *     identifier. While the main firmware is running it reacts to it by resetting into
 *     the bootloader, which then answers the next CONNECT. Must match
 *     BOOT_COM_CAN_RX_MSG_ID in pedals-module-bootloader-sw/Core/Inc/blt_conf.h.
 */
#define CAN_COMMUNICATIONS_ROUTER_BOOTLOADER_RX_ID (20U)

/*! \brief XCP CONNECT command code, first byte of the bootloader request frame. */
#define CAN_COMMUNICATIONS_ROUTER_XCP_CMD_CONNECT (0xFFU)

/*! \brief Length of the XCP CONNECT frame (command code + connection mode). */
#define CAN_COMMUNICATIONS_ROUTER_XCP_CONNECT_LENGTH (2U)

/*!
 * \brief Router function for incoming CAN frames on primary network.
 *
 * \param[in] frame The frame just popped off the RX queue.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR if dispatch fails.
 */
enum CanCommunicationReturnCode can_communications_router_api_receive_primary(const struct CanCommunicationFrame *frame);

/*!
 * \brief Tells whether an XCP CONNECT for the bootloader has been received.
 *
 * \details Once set, the request stays pending until the MCU is reset.
 *
 * \retval true if the firmware should reset into the bootloader.
 * \retval false otherwise.
 */
bool can_communications_router_api_is_bootloader_requested(void);

#endif // CAN_COMMUNICATIONS_ROUTER_H
