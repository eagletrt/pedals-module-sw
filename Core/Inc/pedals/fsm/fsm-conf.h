#ifndef FSMCONF_H
#define FSMCONF_H

#include "eagletrt-fsm.h"

struct FSMHandler fsm_handler;

enum FSMStateID {
    FSM_STATE_INIT = 0,
    FSM_STATE_IDLE,
    FSM_STATE_FLASH,
    FSM_STATE_ERROR,
    FSM_STATE_COUNT
};

void fsm_routine_init(void *data);
void fsm_routine_idle(void *data);
void fsm_routine_flash(void *data);
void fsm_routine_error(void *data);

void fsm_idle_to_flash(void *data);

void fsm_flash_to_idle(void *data);

struct Transition fsm_idle_transitions[] = {
    { .to = FSM_STATE_FLASH, .function = fsm_idle_to_flash }
};
struct Transition fsm_flash_transitions[] = {
    { .to = FSM_STATE_IDLE, .function = fsm_flash_to_idle }
};

struct State fsm_states[] = {
    { .id = FSM_STATE_INIT,
      .function = fsm_routine_init,
      .next_default = FSM_STATE_IDLE,
      .transitions = nullptr,
      .num_transitions = 0 },

    { .id = FSM_STATE_IDLE,
      .function = fsm_routine_idle,
      .next_default = FSM_STATE_IDLE,
      .transitions = fsm_idle_transitions,
      .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(fsm_idle_transitions) },

    { .id = FSM_STATE_FLASH,
      .function = fsm_routine_flash,
      .next_default = FSM_STATE_FLASH,
      .transitions = fsm_flash_transitions,
      .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(fsm_flash_transitions) },

    { .id = FSM_STATE_INIT,
      .function = fsm_routine_error,
      .next_default = FSM_STATE_ERROR,
      .transitions = nullptr,
      .num_transitions = 0 },
};

#endif //FSMCONF_H
