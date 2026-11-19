#include "pedals-fsm-api.h"
#include "eagletrt-fsm-api.h"

#include "eagletrt-api.h"
#include "can-communications-api.h"
#include "bootloader-api.h"
#include "brake-api.h"
#include "throttle-api.h"
#include "post-api.h"
#include "identity-api.h"
#include "scheduler-api.h"

/*! \brief Module FSMHandler */
struct FSMHandler pedals_fsm_handler;

EAGLETRT_STATIC uint32_t fsm_last_module_update_tick = 0;

EAGLETRT_STATIC bool prv_fsm_data_is_valid(const struct FsmData *data) {
    return data != NULL && data->get_tick != NULL &&
           data->update_module != NULL && data->system_reset != NULL;
}

EAGLETRT_STATIC void prv_fsm_update_modules(const struct FsmData *data, uint32_t tick) {
    if (tick - fsm_last_module_update_tick >= FSM_MODULES_UPDATE_PERIOD_MS) {
        fsm_last_module_update_tick = tick;
        data->update_module();
    }
    throttle_api_update_internal_status(tick);
}

/*!
 * \defgroup FSMRoutines
 * \{
 */

/*! \brief FSM init state routine */
EAGLETRT_STATIC void prv_fsm_routine_init(void *data) {
    struct PostInit *init_struct = (struct PostInit *)data;
    if (post_api_init(init_struct) != POST_RC_OK) {
        // STATE_ERROR
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_ERROR);
    }
}

/*! \brief FSM idle state routine */
EAGLETRT_STATIC void prv_fsm_routine_idle(void *data) {
    struct FsmData *idle_struct = (struct FsmData *)data;
    if (!prv_fsm_data_is_valid(idle_struct)) {
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_ERROR);
        return;
    }

    scheduler_api_routine();

    uint32_t current_tick = idle_struct->get_tick();

    can_communications_api_process_rx();

    prv_fsm_update_modules(idle_struct, current_tick);

    if (bootloader_api_is_flashing()) {
        bootloader_api_watchdog_restart(); // TODO: put this in transition state
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_FLASH);
    }

    EAGLETRT_API_UNUSED(identity_api_send_pedals_version(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_pedals_version_info(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_libcan_version(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_libcan_version_info(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_pedals_fsm(current_tick, fsm_api_get_state(&pedals_fsm_handler)));

    EAGLETRT_API_UNUSED(throttle_api_send_status(current_tick));

    EAGLETRT_API_UNUSED(brake_api_send_status(current_tick));

    can_communications_api_process_tx();
}

/*! \brief FSM idle state routine */
EAGLETRT_STATIC void prv_fsm_routine_flash(void *data) {
    struct FsmData *flash_struct = (struct FsmData *)data;
    if (!prv_fsm_data_is_valid(flash_struct)) {
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_ERROR);
        return;
    }

    scheduler_api_routine();

    if (can_communications_api_process_rx() != CAN_COMMUNICATION_RC_OK) {
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_ERROR);
        return;
    }

    prv_fsm_update_modules(flash_struct, flash_struct->get_tick());

    if (bootloader_api_is_timedout()) {
        bootloader_api_set_flashing(false); // TODO: put this in transition state
        fsm_api_transition_to(&pedals_fsm_handler, FSM_STATE_IDLE);
    }
}

/*! \brief FSM flash state routine */
EAGLETRT_STATIC void prv_fsm_routine_error(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*! \} */

/*!
 * \defgroup FSMTransitions
 * \{
 */

/*! \brief FSM transition callback from init to idle */
EAGLETRT_STATIC void prv_fsm_init_to_idle(void *data) {
    EAGLETRT_API_UNUSED(data);
}
/*! \brief FSM transition callback from init to error */
EAGLETRT_STATIC void prv_fsm_init_to_error(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*! \brief FSM transition callback from idle to flash */
EAGLETRT_STATIC void prv_fsm_idle_to_flash(void *data) {
    EAGLETRT_API_UNUSED(data);
    bootloader_api_watchdog_restart();
}
/*! \brief FSM transition callback from idle to error */
EAGLETRT_STATIC void prv_fsm_idle_to_error(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*! \brief FSM transition callback from flash to idle */
EAGLETRT_STATIC void prv_fsm_flash_to_idle(void *data) {
    EAGLETRT_API_UNUSED(data);
    bootloader_api_set_flashing(false);
}
/*! \brief FSM transition callback from flash to error */
EAGLETRT_STATIC void prv_fsm_flash_to_error(void *data) {
    EAGLETRT_API_UNUSED(data);
}

/*! \} */

/*! \brief FSM transitions from init */
const struct Transition fsm_init_transitions[] = {
    { .to = FSM_STATE_IDLE,  .function = prv_fsm_init_to_idle  },
    { .to = FSM_STATE_ERROR, .function = prv_fsm_init_to_error },
};
/*! \brief FSM transitions from idle */
const struct Transition fsm_idle_transitions[] = {
    { .to = FSM_STATE_FLASH, .function = prv_fsm_idle_to_flash },
    { .to = FSM_STATE_ERROR, .function = prv_fsm_idle_to_error },
};
/*! \brief FSM transitions from flash */
const struct Transition fsm_flash_transitions[] = {
    { .to = FSM_STATE_IDLE,  .function = prv_fsm_flash_to_idle  },
    { .to = FSM_STATE_ERROR, .function = prv_fsm_flash_to_error },
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
    return fsm_api_transition_to(&pedals_fsm_handler, state_id);
}

enum FSMStateID pedals_fsm_api_get_state(void) {
    return fsm_api_get_state(&pedals_fsm_handler);
}
