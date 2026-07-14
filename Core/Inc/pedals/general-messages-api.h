#ifndef GENERAL_MESSAGES_API_H
#define GENERAL_MESSAGES_API_H

#include "general-messages.h"
#include "fsm.h"

/*!
 * \brief Sends pedal's version to CAN at a certain rate
 * 
 * \param tick current tick, used to check if it waited enough time before sending
 * \return enum GeneralMessagesReturnCode if it was able to send the message or not
 */
enum GeneralMessagesReturnCode general_messages_api_send_pedals_version(uint32_t tick);

/*!
 * \brief Sends pedal's FSM status to CAN at a certain rate
 * 
 * \param tick current tick, used to check if it waited enough time before sending
 * \return enum GeneralMessagesReturnCode if it was able to send the message or not
 */
enum GeneralMessagesReturnCode general_messages_api_send_pedals_status(uint32_t tick, state_t fsm_state);

#endif //GENERAL_MESSAGES_API_H