#ifndef PEDALS_FSM_API_H
#define PEDALS_FSM_API_H

#include "pedals-fsm.h"
#include "eagletrt-fsm.h"

enum FSMReturnCode pedals_fsm_api_init(void);

enum FSMReturnCode pedals_fsm_api_run_state(void *data);

enum FSMReturnCode pedals_fsm_api_transition_to(enum FSMStateID state_id);

enum FSMStateID pedals_fsm_api_get_state(void);

#endif //PEDALS_FSM_API_H
