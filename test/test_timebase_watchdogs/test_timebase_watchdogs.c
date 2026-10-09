#include "unity.h"
#include "pedals/timebase/timebase-api.h"
#include "pedals/watchdogs/watchdogs-api.h"

#include <string.h>

static struct Watchdog watchdog_a;
static struct Watchdog watchdog_b;
static unsigned int callback_a_count;
static unsigned int callback_b_count;

static void callback_a(void) {
    ++callback_a_count;
}

static void callback_b(void) {
    ++callback_b_count;
}

static void advance_time(uint32_t milliseconds) {
    const uint32_t ticks = TIMEBASE_CONVERT_MS_TO_TICKS(milliseconds);
    for (uint32_t i = 0U; i < ticks; ++i) {
        TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_tick());
    }
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_update());
}

void setUp(void) {
    memset(&watchdog_a, 0, sizeof(watchdog_a));
    memset(&watchdog_b, 0, sizeof(watchdog_b));
    callback_a_count = 0U;
    callback_b_count = 0U;

    TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_init());
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init());
}

void tearDown(void) {
}

void test_timebase_is_shared_and_uses_configured_resolution(void) {
    TEST_ASSERT_TRUE(timebase_is_initialized());
    TEST_ASSERT_EQUAL_UINT32(0U, timebase_get_current_tick());
    TEST_ASSERT_EQUAL_UINT32(0U, timebase_get_current_time());

    advance_time(5U);

    TEST_ASSERT_EQUAL_UINT32(TIMEBASE_CONVERT_MS_TO_TICKS(5U), timebase_get_current_tick());
    TEST_ASSERT_EQUAL_UINT32(5U, timebase_get_current_time());
}

void test_multiple_modules_can_share_the_watchdog_scheduler(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_a, 3U, callback_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_b, 5U, callback_b));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_b));

    advance_time(3U);
    TEST_ASSERT_EQUAL_UINT(1U, callback_a_count);
    TEST_ASSERT_EQUAL_UINT(0U, callback_b_count);
    TEST_ASSERT_TRUE(watchdogs_is_timed_out(&watchdog_a));
    TEST_ASSERT_TRUE(watchdogs_is_running(&watchdog_b));

    advance_time(2U);
    TEST_ASSERT_EQUAL_UINT(1U, callback_b_count);
    TEST_ASSERT_TRUE(watchdogs_is_timed_out(&watchdog_b));
}

void test_restart_rearms_a_timed_out_watchdog(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_a, 2U, callback_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_a));
    advance_time(2U);
    TEST_ASSERT_EQUAL_UINT(1U, callback_a_count);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_restart(&watchdog_a));
    advance_time(1U);
    TEST_ASSERT_EQUAL_UINT(1U, callback_a_count);
    advance_time(1U);
    TEST_ASSERT_EQUAL_UINT(2U, callback_a_count);
}

void test_pet_and_stop_use_the_shared_current_tick(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_a, 4U, callback_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_a));
    advance_time(3U);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_pet(&watchdog_a));
    advance_time(3U);
    TEST_ASSERT_EQUAL_UINT(0U, callback_a_count);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_stop(&watchdog_a));
    advance_time(4U);
    TEST_ASSERT_EQUAL_UINT(0U, callback_a_count);
    TEST_ASSERT_FALSE(watchdogs_is_running(&watchdog_a));
}

void test_watchdog_initialization_rejects_invalid_arguments(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, watchdogs_init_watchdog(NULL, 1U, callback_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, watchdogs_init_watchdog(&watchdog_a, 1U, NULL));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_ERROR, watchdogs_init_watchdog(&watchdog_a, 0U, callback_a));
}

void test_reset_clears_timeout_without_restarting_watchdog(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_a, 2U, callback_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_a));
    advance_time(2U);
    TEST_ASSERT_TRUE(watchdogs_is_timed_out(&watchdog_a));
    TEST_ASSERT_EQUAL_UINT(1U, callback_a_count);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_reset(&watchdog_a));
    TEST_ASSERT_FALSE(watchdogs_is_timed_out(&watchdog_a));
    TEST_ASSERT_FALSE(watchdogs_is_running(&watchdog_a));
    advance_time(4U);
    TEST_ASSERT_EQUAL_UINT(1U, callback_a_count);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_a));
    advance_time(1U);
    TEST_ASSERT_EQUAL_UINT(1U, callback_a_count);
    advance_time(1U);
    TEST_ASSERT_EQUAL_UINT(2U, callback_a_count);
}

void test_reset_stops_only_the_selected_watchdog_and_is_repeatable(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_a, 3U, callback_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_b, 5U, callback_b));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_b));
    advance_time(1U);

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_reset(&watchdog_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_reset(&watchdog_a));
    TEST_ASSERT_FALSE(watchdogs_is_running(&watchdog_a));
    TEST_ASSERT_FALSE(watchdogs_is_timed_out(&watchdog_a));
    TEST_ASSERT_TRUE(watchdogs_is_running(&watchdog_b));
    advance_time(4U);
    TEST_ASSERT_EQUAL_UINT(0U, callback_a_count);
    TEST_ASSERT_EQUAL_UINT(1U, callback_b_count);
}

void test_reset_rejects_null_and_uninitialized_watchdogs(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_NULL_POINTER, watchdogs_reset(NULL));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_UNINITIALIZED, watchdogs_reset(&watchdog_a));
}

void test_start_reads_current_tick_without_a_scheduler_update(void) {
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init_watchdog(&watchdog_a, 3U, callback_a));
    for (uint32_t i = 0U; i < TIMEBASE_CONVERT_MS_TO_TICKS(5U); ++i) {
        TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_tick());
    }

    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_start(&watchdog_a));
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_update());
    TEST_ASSERT_EQUAL_UINT(0U, callback_a_count);
    advance_time(2U);
    TEST_ASSERT_EQUAL_UINT(0U, callback_a_count);
    advance_time(1U);
    TEST_ASSERT_EQUAL_UINT(1U, callback_a_count);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_timebase_is_shared_and_uses_configured_resolution);
    RUN_TEST(test_multiple_modules_can_share_the_watchdog_scheduler);
    RUN_TEST(test_restart_rearms_a_timed_out_watchdog);
    RUN_TEST(test_pet_and_stop_use_the_shared_current_tick);
    RUN_TEST(test_watchdog_initialization_rejects_invalid_arguments);
    RUN_TEST(test_reset_clears_timeout_without_restarting_watchdog);
    RUN_TEST(test_reset_stops_only_the_selected_watchdog_and_is_repeatable);
    RUN_TEST(test_reset_rejects_null_and_uninitialized_watchdogs);
    RUN_TEST(test_start_reads_current_tick_without_a_scheduler_update);
    return UNITY_END();
}
