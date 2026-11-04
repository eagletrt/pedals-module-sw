#include "unity.h"
#include "bootloader-api.h"
#include "pedals/timebase/timebase-api.h"
#include "pedals/watchdogs/watchdogs-api.h"

static struct CanCommunicationFrame connect_frame(void) {
    const struct CanCommunicationFrame frame = {
        .id = BOOTLOADER_CAN_RX_ID,
        .length = BOOTLOADER_XCP_CONNECT_LENGTH,
        .data = { BOOTLOADER_XCP_CMD_CONNECT, 0x00U },
    };
    return frame;
}

static void advance_time(uint32_t milliseconds) {
    const uint32_t ticks = TIMEBASE_CONVERT_MS_TO_TICKS(milliseconds);
    for (uint32_t i = 0U; i < ticks; ++i) {
        TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_tick());
    }
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_update());
}

static void init_system(void) {
    TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_init());
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init());
    TEST_ASSERT_EQUAL(BOOTLOADER_RC_OK, bootloader_init());
}

void setUp(void) {
    init_system();
}

void tearDown(void) {
}

void test_init_resets_module_state(void) {
    struct CanCommunicationFrame frame = connect_frame();
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    TEST_ASSERT_TRUE(bootloader_is_requested());
    TEST_ASSERT_TRUE(bootloader_is_flashing());

    init_system();
    TEST_ASSERT_FALSE(bootloader_is_requested());
    TEST_ASSERT_FALSE(bootloader_is_flashing());
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_update());
}

void test_invalid_frame_is_rejected_without_starting_watchdog(void) {
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_NULL_POINTER, bootloader_receive(NULL));

    struct CanCommunicationFrame frame = connect_frame();
    frame.length = CAN_COMMUNICATIONS_FRAME_DATA_SIZE + 1U;
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_INVALID_LENGTH, bootloader_receive(&frame));
    TEST_ASSERT_FALSE(bootloader_is_requested());
    TEST_ASSERT_FALSE(bootloader_is_flashing());
}

void test_local_connect_requests_bootloader_and_ignores_mode(void) {
    struct CanCommunicationFrame frame = connect_frame();
    frame.data[1] = 1U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    TEST_ASSERT_TRUE(bootloader_is_requested());
    TEST_ASSERT_TRUE(bootloader_is_flashing());

    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS);
    TEST_ASSERT_FALSE(bootloader_is_flashing());
    TEST_ASSERT_TRUE(bootloader_is_requested());
}

void test_only_exact_local_connect_requests_reset(void) {
    struct CanCommunicationFrame frame = connect_frame();
    frame.id = BOOTLOADER_CAN_RX_ID - 1U;
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    TEST_ASSERT_FALSE(bootloader_is_requested());

    frame = connect_frame();
    frame.data[0] = 0xFEU;
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    TEST_ASSERT_FALSE(bootloader_is_requested());

    const uint8_t invalid_lengths[] = { 0U, 1U, 8U };
    frame = connect_frame();
    for (size_t i = 0U; i < sizeof(invalid_lengths) / sizeof(invalid_lengths[0]); ++i) {
        frame.length = invalid_lengths[i];
        TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
        TEST_ASSERT_FALSE(bootloader_is_requested());
    }
}

void test_flashing_range_is_inclusive(void) {
    struct CanCommunicationFrame frame = { .length = 1U, .data = { 0xFEU } };
    const uint32_t limits[] = { BOOTLOADER_CAN_FLASH_ID_MIN, BOOTLOADER_CAN_FLASH_ID_MAX };

    for (size_t i = 0U; i < sizeof(limits) / sizeof(limits[0]); ++i) {
        init_system();
        frame.id = limits[i];
        TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
        TEST_ASSERT_TRUE(bootloader_is_flashing());
    }
}

void test_frames_outside_range_do_not_start_or_rearm_watchdog(void) {
    struct CanCommunicationFrame frame = { .id = BOOTLOADER_CAN_FLASH_ID_MIN, .length = 1U };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));

    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS - TIMEBASE_RESOLUTION_MS);
    TEST_ASSERT_TRUE(bootloader_is_flashing());

    frame.id = BOOTLOADER_CAN_FLASH_ID_MIN - 1U;
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    frame.id = BOOTLOADER_CAN_FLASH_ID_MAX + 1U;
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));

    advance_time(TIMEBASE_RESOLUTION_MS);
    TEST_ASSERT_FALSE(bootloader_is_flashing());

    init_system();
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    TEST_ASSERT_FALSE(bootloader_is_flashing());
}

void test_each_flashing_frame_restarts_watchdog(void) {
    const struct CanCommunicationFrame frame = { .id = BOOTLOADER_CAN_FLASH_ID_MIN, .length = 1U };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS - TIMEBASE_RESOLUTION_MS);

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));
    advance_time(BOOTLOADER_INACTIVITY_TIMEOUT_MS - TIMEBASE_RESOLUTION_MS);
    TEST_ASSERT_TRUE(bootloader_is_flashing());

    advance_time(TIMEBASE_RESOLUTION_MS);
    TEST_ASSERT_FALSE(bootloader_is_flashing());
}

void test_timebase_ticks_do_not_expire_watchdog_until_routine_runs(void) {
    const struct CanCommunicationFrame frame = { .id = BOOTLOADER_CAN_FLASH_ID_MIN, .length = 1U };
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, bootloader_receive(&frame));

    const uint32_t ticks = TIMEBASE_CONVERT_MS_TO_TICKS(BOOTLOADER_INACTIVITY_TIMEOUT_MS);
    for (uint32_t i = 0U; i < ticks; ++i) {
        TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_tick());
    }
    TEST_ASSERT_TRUE(bootloader_is_flashing());
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_update());
    TEST_ASSERT_FALSE(bootloader_is_flashing());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_resets_module_state);
    RUN_TEST(test_invalid_frame_is_rejected_without_starting_watchdog);
    RUN_TEST(test_local_connect_requests_bootloader_and_ignores_mode);
    RUN_TEST(test_only_exact_local_connect_requests_reset);
    RUN_TEST(test_flashing_range_is_inclusive);
    RUN_TEST(test_frames_outside_range_do_not_start_or_rearm_watchdog);
    RUN_TEST(test_each_flashing_frame_restarts_watchdog);
    RUN_TEST(test_timebase_ticks_do_not_expire_watchdog_until_routine_runs);
    return UNITY_END();
}
