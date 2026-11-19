/*
 * Note: the production state table currently declares no INIT->IDLE transition
 * and no IDLE/FLASH->ERROR transitions, so pedals_fsm_api_init() fails with
 * FSM_RC_INVALID_MACHINE and these behavioural tests stay red until the FSM
 * configuration is fixed.
 */

#include "unity.h"

#include "bootloader-api.h"
#include "bootloader.h"
#include "can-communications-api.h"
#include "can-communications-router-api.h"
#include "can-primary.h"
#include "pedals-fsm-api.h"
#include "throttle.h"
#include "post.h"

extern uint32_t fsm_last_module_update_tick;

static uint32_t tick;
static unsigned int tx_count;
static unsigned int reset_count;
static unsigned int update_count;

static uint32_t get_tick(void) {
    return tick;
}

static void reset(void) {
    ++reset_count;
}

static void update_modules(void) {
    ++update_count;
}

static enum ThrottleReturnCode timer_callback(void) {
    return THROTTLE_RC_OK;
}

static enum CanCommunicationReturnCode send_frame(const struct CanCommunicationFrame *frame) {
    TEST_ASSERT_NOT_NULL(frame);
    ++tx_count;
    return CAN_COMMUNICATION_RC_OK;
}

static struct FsmData data = {
    .get_tick = get_tick,
    .update_module = update_modules,
    .system_reset = reset,
};

static struct PostInit valid_post_init(void) {
    const struct PostInit init = {
        .start_timer = timer_callback,
        .stop_timer = timer_callback,
        .config = {
                   .send = send_frame,
                   .on_receive = can_communications_router_api_receive_primary,
                   },
        .get_tick = get_tick,
        .reboot = reset,
    };
    return init;
}

static void start_machine(void) {
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_init());
}

static void enter_idle(void) {
    struct PostInit init = valid_post_init();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&init));
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, pedals_fsm_api_get_state());
}

static void run_expected(enum FSMStateID expected) {
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&data));
    TEST_ASSERT_EQUAL(expected, pedals_fsm_api_get_state());
}

static void receive(uint32_t id, uint8_t length) {
    const struct CanCommunicationFrame frame = {
        .id = id,
        .length = length,
    };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK,
                      can_communications_api_add_to_rx_buffer(&frame));
}

static void advance_time(uint32_t milliseconds) {
    tick += milliseconds;
}

static void enter_flash(void) {
    start_machine();
    enter_idle();
    receive(BOOTLOADER_CAN_FLASH_ID_MIN, 1U);
    run_expected(FSM_STATE_FLASH);
}

void setUp(void) {
    tick = 100U;
    tx_count = 0U;
    reset_count = 0U;
    update_count = 0U;
    fsm_last_module_update_tick = 0U;
}

void tearDown(void) {
}

void test_fsm_starts_in_init_state(void) {
    start_machine();
    TEST_ASSERT_EQUAL(FSM_STATE_INIT, pedals_fsm_api_get_state());
}

void test_init_with_valid_configuration_reaches_idle(void) {
    start_machine();

    struct PostInit init = valid_post_init();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&init));
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, pedals_fsm_api_get_state());
    TEST_ASSERT_EQUAL_UINT(0U, reset_count);
}

void test_init_with_invalid_configuration_reaches_error(void) {
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_init());
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(NULL));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    struct PostInit init = valid_post_init();
    init.get_tick = NULL;
    start_machine();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&init));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    init = valid_post_init();
    init.reboot = NULL;
    start_machine();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&init));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    init = valid_post_init();
    init.start_timer = NULL;
    start_machine();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&init));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    init = valid_post_init();
    init.config.send = NULL;
    start_machine();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&init));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    init = valid_post_init();
    init.config.on_receive = NULL;
    start_machine();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&init));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());
}

void test_idle_sends_telemetry(void) {
    start_machine();
    enter_idle();

    run_expected(FSM_STATE_IDLE);
    TEST_ASSERT_GREATER_THAN_UINT(0U, tx_count);
}

void test_idle_updates_modules_at_configured_rate(void) {
    start_machine();
    enter_idle();
    TEST_ASSERT_EQUAL_UINT(0U, update_count);

    run_expected(FSM_STATE_IDLE);
    TEST_ASSERT_EQUAL_UINT(1U, update_count);

    advance_time(1U);
    run_expected(FSM_STATE_IDLE);
    TEST_ASSERT_EQUAL_UINT(1U, update_count);

    advance_time(1U);
    run_expected(FSM_STATE_IDLE);
    TEST_ASSERT_EQUAL_UINT(1U, update_count);

    advance_time(1U);
    run_expected(FSM_STATE_IDLE);
    TEST_ASSERT_EQUAL_UINT(2U, update_count);
}

void test_idle_rejects_invalid_fsm_data(void) {
    start_machine();
    enter_idle();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(NULL));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    struct FsmData invalid_data = data;
    invalid_data.get_tick = NULL;
    start_machine();
    enter_idle();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&invalid_data));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    invalid_data = data;
    invalid_data.update_module = NULL;
    start_machine();
    enter_idle();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&invalid_data));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    invalid_data = data;
    invalid_data.system_reset = NULL;
    start_machine();
    enter_idle();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&invalid_data));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());
}

void test_flashing_id_range_is_inclusive(void) {
    const uint32_t limits[] = { BOOTLOADER_CAN_FLASH_ID_MIN, BOOTLOADER_CAN_FLASH_ID_MAX };

    for (size_t i = 0U; i < sizeof(limits) / sizeof(limits[0]); ++i) {
        start_machine();
        enter_idle();

        receive(limits[i], 1U);
        run_expected(FSM_STATE_FLASH);
    }
}

void test_flash_silences_telemetry_but_keeps_modules_updated(void) {
    enter_flash();
    const unsigned int tx_in_flash = tx_count;
    const unsigned int updates_in_flash = update_count;

    advance_time(FSM_MODULES_UPDATE_PERIOD_MS);
    run_expected(FSM_STATE_FLASH);

    TEST_ASSERT_EQUAL_UINT(tx_in_flash, tx_count);
    TEST_ASSERT_GREATER_THAN_UINT(updates_in_flash, update_count);
}

void test_flash_watchdog_rearms_on_flashing_frames(void) {
    enter_flash();
    const unsigned int tx_in_flash = tx_count;

    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS - 1U);
    receive(BOOTLOADER_CAN_FLASH_ID_MAX, 1U);
    run_expected(FSM_STATE_FLASH);

    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS - 1U);
    run_expected(FSM_STATE_FLASH);

    advance_time(1U);
    run_expected(FSM_STATE_IDLE);

    run_expected(FSM_STATE_IDLE);
    TEST_ASSERT_GREATER_THAN_UINT(tx_in_flash, tx_count);
    TEST_ASSERT_EQUAL_UINT(0U, reset_count);
}

void test_flash_rejects_invalid_can_frame(void) {
    enter_flash();

    receive(0x7FFU, 1U);
    run_expected(FSM_STATE_ERROR);
}

void test_flash_rejects_invalid_fsm_data(void) {
    enter_flash();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(NULL));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    enter_flash();
    struct FsmData invalid_data = data;
    invalid_data.get_tick = NULL;
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&invalid_data));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());
}

void test_local_flashing_frame_reboots_in_idle(void) {
    start_machine();
    enter_idle();

    receive(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSFLASHINGRX,
            can_primary_byte_size_pedalsflashingrx);
    run_expected(FSM_STATE_IDLE);

    TEST_ASSERT_EQUAL_UINT(1U, reset_count);
}

void test_local_flashing_frame_reboots_while_flashing(void) {
    enter_flash();

    receive(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSFLASHINGRX,
            can_primary_byte_size_pedalsflashingrx);
    run_expected(FSM_STATE_FLASH);

    TEST_ASSERT_EQUAL_UINT(1U, reset_count);
}

void test_error_state_is_terminal(void) {
    start_machine();
    enter_idle();
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(NULL));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());

    const unsigned int tx_in_error = tx_count;
    const unsigned int updates_in_error = update_count;

    advance_time(10U);
    receive(BOOTLOADER_CAN_FLASH_ID_MIN, 1U);
    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(&data));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());
    TEST_ASSERT_EQUAL_UINT(tx_in_error, tx_count);
    TEST_ASSERT_EQUAL_UINT(updates_in_error, update_count);
    TEST_ASSERT_FALSE(bootloader_api_is_flashing());

    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_run_state(NULL));
    TEST_ASSERT_EQUAL(FSM_STATE_ERROR, pedals_fsm_api_get_state());
}

void test_transition_api_validates_requests(void) {
    start_machine();

    TEST_ASSERT_EQUAL(FSM_RC_INVALID_TRANSITION,
                      pedals_fsm_api_transition_to(FSM_STATE_FLASH));
    TEST_ASSERT_EQUAL(FSM_STATE_INIT, pedals_fsm_api_get_state());

    TEST_ASSERT_EQUAL(FSM_RC_INVALID_STATE,
                      pedals_fsm_api_transition_to(FSM_STATE_COUNT));

    enter_idle();

    TEST_ASSERT_EQUAL(FSM_RC_OK, pedals_fsm_api_transition_to(FSM_STATE_FLASH));
    TEST_ASSERT_EQUAL(FSM_STATE_IDLE, pedals_fsm_api_get_state());

    run_expected(FSM_STATE_FLASH);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_fsm_starts_in_init_state);
    RUN_TEST(test_init_with_valid_configuration_reaches_idle);
    RUN_TEST(test_init_with_invalid_configuration_reaches_error);
    RUN_TEST(test_idle_sends_telemetry);
    RUN_TEST(test_idle_updates_modules_at_configured_rate);
    RUN_TEST(test_idle_rejects_invalid_fsm_data);
    RUN_TEST(test_flashing_id_range_is_inclusive);
    RUN_TEST(test_flash_silences_telemetry_but_keeps_modules_updated);
    RUN_TEST(test_flash_watchdog_rearms_on_flashing_frames);
    RUN_TEST(test_flash_rejects_invalid_can_frame);
    RUN_TEST(test_flash_rejects_invalid_fsm_data);
    RUN_TEST(test_local_flashing_frame_reboots_in_idle);
    RUN_TEST(test_local_flashing_frame_reboots_while_flashing);
    RUN_TEST(test_error_state_is_terminal);
    RUN_TEST(test_transition_api_validates_requests);
    return UNITY_END();
}
