#include "fsm-conf.h"
#include "eagletrt-fsm-api.h"

#include "eagletrt-api.h"
#include "can-communications-api.h"
#include "bootloader-api.h"
#include "brake-api.h"
#include "throttle-api.h"
#include "post-api.h"
#include "identity-api.h"
#include "scheduler-api.h"

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

void fsm_routine_init(void *data) {
    struct PostInit *init_struct = (struct PostInit *)data;
    if (post_api_init(init_struct) != POST_RC_OK) {
        // STATE_ERROR
        fsm_api_transition_to(&fsm_handler, FSM_STATE_ERROR);
    }
}

void fsm_routine_idle(void *data) {
    struct FsmData *idle_struct = (struct FsmData *)data;
    if (!prv_fsm_data_is_valid(idle_struct)) {
        fsm_api_transition_to(&fsm_handler, FSM_STATE_ERROR);
        return;
    }

    scheduler_api_routine();

    uint32_t current_tick = idle_struct->get_tick();

    can_communications_api_process_rx();

    prv_fsm_update_modules(idle_struct, current_tick);

    if (bootloader_api_is_flashing()) {
        bootloader_api_watchdog_restart(); // TODO: put this in transition state
        fsm_api_transition_to(&fsm_handler, FSM_STATE_FLASH);
    }

    EAGLETRT_API_UNUSED(identity_api_send_pedals_version(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_pedals_version_info(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_libcan_version(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_libcan_version_info(current_tick));

    EAGLETRT_API_UNUSED(identity_api_send_pedals_fsm(current_tick, fsm_api_get_state(&fsm_handler)));

    EAGLETRT_API_UNUSED(throttle_api_send_status(current_tick));

    EAGLETRT_API_UNUSED(brake_api_send_status(current_tick));

    can_communications_api_process_tx();
}

void fsm_routine_flash(void *data) {
    struct FsmData *flash_struct = (struct FsmData *)data;
    if (!prv_fsm_data_is_valid(flash_struct)) {
        fsm_api_transition_to(&fsm_handler, FSM_STATE_ERROR);
        return;
    }

    scheduler_api_routine();

    if (can_communications_api_process_rx() != CAN_COMMUNICATION_RC_OK) {
        fsm_api_transition_to(&fsm_handler, FSM_STATE_ERROR);
        return;
    }

    prv_fsm_update_modules(flash_struct, flash_struct->get_tick());

    if (bootloader_api_is_timedout()) {
        bootloader_api_set_flashing(false); // TODO: put this in transition state
        fsm_api_transition_to(&fsm_handler, FSM_STATE_IDLE);
    }
}
void fsm_routine_error(void *data) {
    EAGLETRT_API_UNUSED(data);
}

// Transition functions

void fsm_idle_to_flash(void *data) {
    EAGLETRT_API_UNUSED(data);
    bootloader_api_watchdog_restart();
}

void fsm_flash_to_idle(void *data) {
    EAGLETRT_API_UNUSED(data);
    bootloader_api_set_flashing(false);
}
