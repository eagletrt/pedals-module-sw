#ifndef FSMCONF_H
#define FSMCONF_H

#include "eagletrt-fsm.h"

/*! \brief Global FSMHandler */
extern struct FSMHandler fsm_handler;

/*! \brief The ids of the possible fsm state */
enum FSMStateID {
    FSM_STATE_INIT = 0,
    FSM_STATE_IDLE,
    FSM_STATE_FLASH,
    FSM_STATE_ERROR,
    FSM_STATE_COUNT
};

/*!
 * \defgroup FSMRoutines
 * \{
 */

/*! \brief FSM init state routine */
void fsm_routine_init(void *data);

/*! \brief FSM init state routine */
void fsm_routine_idle(void *data);

/*! \brief FSM init state routine */
void fsm_routine_flash(void *data);

/*! \brief FSM init state routine */
void fsm_routine_error(void *data);

/*! \} */

/*!
 * \defgroup FSMTransitions
 * \{
 */

/*! \brief FSM transition callback from idle to flash */
void fsm_idle_to_flash(void *data);

/*! \brief FSM transition callback from flash to idle */
void fsm_flash_to_idle(void *data);

/*! \} */

/*! \brief FSM transitions from idle */
struct Transition fsm_idle_transitions[] = {
    { .to = FSM_STATE_FLASH, .function = fsm_idle_to_flash }
};
/*! \brief FSM transitions from flash */
struct Transition fsm_flash_transitions[] = {
    { .to = FSM_STATE_IDLE, .function = fsm_flash_to_idle }
};

/*!
 * \brief FSM possible states
 */
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
