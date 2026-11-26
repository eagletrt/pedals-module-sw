/*!
 * \file bootloader-api.h
 * \author Matteo Grande [mattwexx2929@gmail.com]
 * \date 2026-11-19
 * \brief Implementation of the APIs of the pedals fsm module
 */

#include "pedals-fsm-api.h"
#include "eagletrt-fsm-api.h"

#include "eagletrt-api.h"
#include "can-communications-api.h"
#include "bootloader-api.h"
#include "post-api.h"
#include "scheduler-api.h"

/*! \brief Private Module FSMHandler */
EAGLETRT_STATIC struct FSMHandler pedals_fsm_handler;

/*!
 * \defgroup FSMRoutines
 * \{
 */

/*! \brief FSM init state routine */
EAGLETRT_STATIC void prv_fsm_routine_init(void *data) {
    struct PostInit *init_struct = (struct PostInit *)data;
    if (post_api_init(init_struct) != POST_RC_OK) {
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_ERROR);
    }
}

/*! \brief FSM idle state routine */
EAGLETRT_STATIC void prv_fsm_routine_idle(void *data) {
    EAGLETRT_API_UNUSED(data);

    can_communications_api_process_rx();

    if (bootloader_api_is_flashing()) {
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_FLASH);
    }

    can_communications_api_process_tx();
}

/*! \brief FSM flash state routine */
EAGLETRT_STATIC void prv_fsm_routine_flash(void *data) {
    EAGLETRT_API_UNUSED(data);

    if (bootloader_api_is_timedout()) {
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_IDLE);
    }

    can_communications_api_process_rx();
}

/*! \brief FSM error state routine */
EAGLETRT_STATIC void prv_fsm_routine_error(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*! \} */

/*!
 * \defgroup FSMTaskHelpers
 * \{
 */

/*! \brief enable all tasks */
EAGLETRT_STATIC void prv_fsm_enable_tasks(void) {
    for (enum SchedulerTaskName task_id = 0; task_id < SCHEDULER_TASKS_COUNT; task_id++) {
        scheduler_api_enable_task(task_id);
    }
}

/*! \brief disable noisy tasks */
EAGLETRT_STATIC void prv_fsm_disable_noisy_tasks(void) {
    bool is_task_noisy[SCHEDULER_TASKS_COUNT] = { true };
    is_task_noisy[SCHEDULER_TASK_IDENTITY_SEND_PEDALS_FSM] = false;

    for (enum SchedulerTaskName task_id = 0;
         task_id < SCHEDULER_TASKS_COUNT;
         task_id++) {
        if (is_task_noisy[task_id]) {
            scheduler_api_disable_task(task_id);
        }
    }
}

/*! \} */

/*!
 * \defgroup FSMTransitions
 * \{
 */

/*! \brief FSM transition callback from init to idle */
EAGLETRT_STATIC void prv_fsm_init_to_idle(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_fsm_enable_tasks();
}

/*! \brief FSM transition callback from idle to flash */
EAGLETRT_STATIC void prv_fsm_idle_to_flash(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_fsm_disable_noisy_tasks();
    bootloader_api_watchdog_restart();
}

/*! \brief FSM transition callback from flash to idle */
EAGLETRT_STATIC void prv_fsm_flash_to_idle(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_fsm_enable_tasks();
    bootloader_api_set_flashing(false);
}

/*! \brief FSM transition callback from any state to error */
EAGLETRT_STATIC void prv_fsm_to_error(void *data) {
    EAGLETRT_API_UNUSED(data);
    prv_fsm_disable_noisy_tasks();
}

/*! \} */

/*! \brief FSM transitions from init */
const struct Transition fsm_init_transitions[] = {
    { .to = FSM_STATE_IDLE,  .function = prv_fsm_init_to_idle },
    { .to = FSM_STATE_ERROR, .function = prv_fsm_to_error     },
};
/*! \brief FSM transitions from idle */
const struct Transition fsm_idle_transitions[] = {
    { .to = FSM_STATE_FLASH, .function = prv_fsm_idle_to_flash },
    { .to = FSM_STATE_ERROR, .function = prv_fsm_to_error      },
};
/*! \brief FSM transitions from flash */
const struct Transition fsm_flash_transitions[] = {
    { .to = FSM_STATE_IDLE,  .function = prv_fsm_flash_to_idle },
    { .to = FSM_STATE_ERROR, .function = prv_fsm_to_error      },
};

/*! \brief FSM possible states */
const struct State pedals_fsm_states[] = {
    {
     .id = FSM_STATE_INIT,
     .function = prv_fsm_routine_init,
     .next_default = FSM_STATE_IDLE,
     .transitions = fsm_init_transitions,
     .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(fsm_init_transitions),
     },

    {
     .id = FSM_STATE_IDLE,
     .function = prv_fsm_routine_idle,
     .next_default = FSM_STATE_IDLE,
     .transitions = fsm_idle_transitions,
     .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(fsm_idle_transitions),
     },
    {
     .id = FSM_STATE_FLASH,
     .function = prv_fsm_routine_flash,
     .next_default = FSM_STATE_FLASH,
     .transitions = fsm_flash_transitions,
     .num_transitions = EAGLETRT_FSM_TRANSITION_LEN(fsm_flash_transitions),
     },

    {
     .id = FSM_STATE_ERROR,
     .function = prv_fsm_routine_error,
     .next_default = FSM_STATE_ERROR,
     .transitions = nullptr,
     .num_transitions = 0,
     },
};

enum FSMReturnCode pedals_fsm_api_init(void) {
    return fsm_api_init(&pedals_fsm_handler, pedals_fsm_states, FSM_STATE_COUNT, FSM_STATE_INIT);
}

enum FSMReturnCode pedals_fsm_api_run_state(void *data) {
    return fsm_api_run_state(&pedals_fsm_handler, data);
}

enum FSMReturnCode pedals_fsm_api_transition_to(enum FSMStateID state_id) {
    // If the fsm wants to go in ERROR and someone else require a different
    // state from ERROR the fsm gives precedence to ERROR
    if (pedals_fsm_handler.requested_state == FSM_STATE_ERROR && state_id != FSM_STATE_ERROR) {
        return FSM_RC_INVALID_TRANSITION;
    }
    return fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_ERROR);
}

enum FSMStateID pedals_fsm_api_get_state(void) {
    return fsm_api_get_state(&pedals_fsm_handler);
}

void pedals_fsm_run_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);
    pedals_fsm_api_run_state(nullptr);
}
