#ifndef IDENTITY_API_H
#define IDENTITY_API_H

#include "identity.h"
#include "fsm.h"

/*!
 * \brief Sends pedal's version to CAN at a certain rate
 * 
 * \param tick current tick, used to check if it waited enough time before sending
 * \return enum IdentityReturnCode if it was able to send the message or not
 */
enum IdentityReturnCode identity_api_send_pedals_version(uint32_t tick);

/*!
 * \brief Sends pedal's version info to CAN at a certain rate
 * 
 * \param tick current tick, used to check if it waited enough time before sending
 * \return enum IdentityReturnCode if it was able to send the message or not
 */
enum IdentityReturnCode identity_api_send_pedals_version_info(uint32_t tick);

/*!
 * \brief Sends pedal's FSM status to CAN at a certain rate
 * 
 * \param tick current tick, used to check if it waited enough time before sending
 * \return enum IdentityReturnCode if it was able to send the message or not
 */
enum IdentityReturnCode identity_api_send_pedals_fsm(uint32_t tick, state_t fsm_state);

#endif //IDENTITY_API_H