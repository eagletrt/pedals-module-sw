#ifndef GENERAL_MESSAGES_API_H
#define GENERAL_MESSAGES_API_H

#include "general-messages.h"
#include "fsm.h"

// insert actual values for version
enum GeneralMessagesReturnCode general_messages_send_pedals_version(uint32_t tick);

enum GeneralMessagesReturnCode general_messages_send_pedals_status(uint32_t tick, state_t fsm_state);

#endif //GENERAL_MESSAGES_API_H