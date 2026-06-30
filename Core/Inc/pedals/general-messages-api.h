#ifndef GENERAL_MESSAGES_API_H
#define GENERAL_MESSAGES_API_H

#include "general-messages.h"
#include "fsm.h"

// insert actual values for version
enum GeneralMessagesReturnCode general_messages_send_pedals_version(void);

enum GeneralMessagesReturnCode general_messages_send_pedals_status(state_t fsm_state);

#endif //GENERAL_MESSAGES_API_H