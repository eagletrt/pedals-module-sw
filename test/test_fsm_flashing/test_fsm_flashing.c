#include "unity.h"
#include "arena-allocator-api.h"
#include "bootloader-api.h"
#include "can-communications-api.h"
#include "can-communications-router-api.h"
#include "fsm.h"
#include "pedals/timebase/timebase-api.h"
#include "pedals/watchdogs/watchdogs-api.h"
#include "post-api.h"
#include "throttle-api.h"

extern struct CanCommunicationsHandler handler;
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
    throttle_api_update_pedal_values(0.5F, 0.5F, 0.5F);
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

static void receive(uint32_t id, uint8_t command, uint8_t length) {
    const struct CanCommunicationFrame frame = {
        .id = id,
        .length = length,
        .data = { command, 0U },
    };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK,
                      can_communications_api_add_to_rx_buffer(&frame));
}

static void advance_time(uint32_t milliseconds) {
    const uint32_t ticks = TIMEBASE_CONVERT_MS_TO_TICKS(milliseconds);
    for (uint32_t i = 0U; i < ticks; ++i) {
        TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_tick());
    }
    tick += milliseconds;
}

static state_t run_operational_state(state_t state, struct FsmData *fsm_data) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_update());
    return run_state(state, fsm_data);
}

void setUp(void) {
    tick = 100U;
    tx_count = 0U;
    reset_count = 0U;
    update_count = 0U;
    fsm_last_module_update_tick = 0U;

    struct PostInit init = {
        .start_timer = timer_callback,
        .stop_timer = timer_callback,
        .config = {
            .send = send_frame,
            .on_receive = can_communications_router_api_receive_primary,
        },
    };
    TEST_ASSERT_EQUAL(STATE_IDLE, run_state(STATE_INIT, &init));
}

void tearDown(void) {
    arena_allocator_api_free(&handler.arena);
}

void test_flashing_silences_can_and_resumes_after_watchdog_timeout(void) {
    receive(BOOTLOADER_CAN_FLASH_ID_MIN, 0xFEU, 1U);
    state_t state = run_operational_state(STATE_IDLE, &data);
    TEST_ASSERT_EQUAL(STATE_FLASH, state);
    TEST_ASSERT_EQUAL_UINT(0U, tx_count);
    TEST_ASSERT_EQUAL_UINT(1U, update_count);

    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS - TIMEBASE_RESOLUTION_MS);
    receive(BOOTLOADER_CAN_FLASH_ID_MAX, 0xFEU, 1U);
    state = run_operational_state(state, &data);
    TEST_ASSERT_EQUAL(STATE_FLASH, state);
    TEST_ASSERT_EQUAL_UINT(0U, tx_count);
    TEST_ASSERT_EQUAL_UINT(2U, update_count);

    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS - TIMEBASE_RESOLUTION_MS);
    receive(BOOTLOADER_CAN_FLASH_ID_MAX + 1U, 0xFFU, 2U);
    TEST_ASSERT_EQUAL(STATE_FLASH, run_operational_state(state, &data));

    advance_time(TIMEBASE_RESOLUTION_MS);
    state = run_operational_state(state, &data);
    TEST_ASSERT_EQUAL(STATE_IDLE, state);
    TEST_ASSERT_EQUAL_UINT(0U, tx_count);

    TEST_ASSERT_EQUAL(STATE_IDLE, run_operational_state(state, &data));
    TEST_ASSERT_GREATER_THAN_UINT(0U, tx_count);
    TEST_ASSERT_EQUAL_UINT(0U, reset_count);
}

void test_local_connect_resets_from_idle_without_telemetry(void) {
    receive(BOOTLOADER_CAN_RX_ID,
            BOOTLOADER_XCP_CMD_CONNECT,
            BOOTLOADER_XCP_CONNECT_LENGTH);

    TEST_ASSERT_EQUAL(STATE_FLASH, run_operational_state(STATE_IDLE, &data));
    TEST_ASSERT_EQUAL_UINT(1U, reset_count);
    TEST_ASSERT_EQUAL_UINT(0U, tx_count);
}

void test_local_connect_resets_while_another_board_is_flashing(void) {
    receive(BOOTLOADER_CAN_FLASH_ID_MIN, 0xFEU, 1U);
    TEST_ASSERT_EQUAL(STATE_FLASH, run_operational_state(STATE_IDLE, &data));

    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS);
    receive(BOOTLOADER_CAN_RX_ID,
            BOOTLOADER_XCP_CMD_CONNECT,
            BOOTLOADER_XCP_CONNECT_LENGTH);

    TEST_ASSERT_EQUAL(STATE_FLASH, run_operational_state(STATE_FLASH, &data));
    TEST_ASSERT_EQUAL_UINT(1U, reset_count);
    TEST_ASSERT_EQUAL_UINT(0U, tx_count);
}

void test_flash_state_rejects_missing_callbacks(void) {
    TEST_ASSERT_EQUAL(STATE_ERROR, run_operational_state(STATE_FLASH, NULL));

    struct FsmData invalid_data = data;
    invalid_data.get_tick = NULL;
    TEST_ASSERT_EQUAL(STATE_ERROR, run_operational_state(STATE_FLASH, &invalid_data));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_flashing_silences_can_and_resumes_after_watchdog_timeout);
    RUN_TEST(test_local_connect_resets_from_idle_without_telemetry);
    RUN_TEST(test_local_connect_resets_while_another_board_is_flashing);
    RUN_TEST(test_flash_state_rejects_missing_callbacks);
    return UNITY_END();
}
