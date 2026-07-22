#include "unity.h"
#include "fff.h"
#include "throttle-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum ThrottleReturnCode, test_throttle_start_timer);
FAKE_VALUE_FUNC(enum ThrottleReturnCode, test_throttle_reset_timer);

extern struct ThrottleHandler throttle_handler;

void setUp(void) {
    RESET_FAKE(test_throttle_start_timer);
    RESET_FAKE(test_throttle_reset_timer)
    FFF_RESET_HISTORY();
    throttle_handler.is_implausibility_timeout = false;
    throttle_handler.apps_travel_percentages[0] = 0.0F;
    throttle_handler.apps_travel_percentages[1] = 0.0F;
    throttle_handler.apps_travel_percentages[2] = 0.0F;
    throttle_handler.travel_percentage = 0.0f;
    throttle_handler.start_timer = NULL;
    throttle_handler.stop_timer = NULL;
    throttle_handler.status = THROTTLE_STATUS_OK;
    throttle_handler.last_update_tick = 0;
}

/*void tearDown(void) {
    // clean stuff up here
}*/

// throttle_api_implausibility_timeout_trigger and throttle_api_update_pedal_values will not be tested because they're simple setters
// throttle_api_get_apps_NUMBER and throttle_api_get_travel_percentage will not be tested because they're simple getters

void test_throttle_api_successful_init(void) {
    enum ThrottleReturnCode rc = throttle_api_init(test_throttle_start_timer, test_throttle_reset_timer);

    TEST_ASSERT_EQUAL_MESSAGE(rc, THROTTLE_RC_OK, "Error during initialisation");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.start_timer, test_throttle_start_timer, "Start timer was not initialised");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.stop_timer, test_throttle_reset_timer, "Reset timer was not initialised");
}

void test_throttle_api_init_no_first_member(void) {

    enum ThrottleReturnCode rc = throttle_api_init(NULL, test_throttle_reset_timer);
    TEST_ASSERT_EQUAL_MESSAGE(rc, THROTTLE_RC_NULL_POINTER, "Error return value was different than expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.start_timer, NULL, "Start timer was initialised");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.stop_timer, NULL, "Reset timer was not initialised");
}

void test_throttle_api_init_no_second_member(void) {

    enum ThrottleReturnCode rc = throttle_api_init(test_throttle_start_timer, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(rc, THROTTLE_RC_NULL_POINTER, "Error return value was different than expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.start_timer, NULL, "Start timer was initialised");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.stop_timer, NULL, "Reset timer was not initialised");
}

// check if the implausible error state and related actions are as intended
void test_throttle_api_update_status_to_implausible_error() {
    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;

    throttle_handler.status = THROTTLE_STATUS_OK;
    throttle_handler.travel_percentage = 0.5f;

    throttle_api_implausibility_timeout_trigger();

    throttle_handler.apps_travel_percentages[0] = 0.3F;
    throttle_handler.apps_travel_percentages[1] = 0.32F;
    throttle_handler.apps_travel_percentages[2] = 0.31F; // actual values don't matter, will be reset to zero
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.5f, "Last value was not reset");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_IMPLAUSIBILITY_ERROR, "Status was not set to implausibility");
}

void test_throttle_api_update_status_stay_in_implausible_error() {
    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;

    throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBILITY_ERROR;
    throttle_handler.travel_percentage = 0.6f;

    throttle_handler.apps_travel_percentages[0] = 0.3F;
    throttle_handler.apps_travel_percentages[1] = 0.32F;
    throttle_handler.apps_travel_percentages[2] = 0.31F; // actual values don't matter, will be reset to zero
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.6f, "Last value was changed, even if it shouldn't have");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_IMPLAUSIBILITY_ERROR, "Status was changed from implausibility");
}

// check if these values are handled correctly and keep the throttle in NO_ERROR state

void test_throttle_api_update_status_all_values_valid() {
    float apps1 = 0.50F;
    float apps2 = 0.52F;
    float apps3 = 0.54F;
    //result should be 50+52+54/3=52

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.travel_percentage = 0.33f;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.52f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_OK, "Status was changed from NO_ERROR");
}
void test_throttle_api_update_status_all_values_valid_one_implausible_pair() {
    float apps1 = 0.50F;
    float apps2 = 0.62F;
    float apps3 = 0.54F;
    //result should be 50+54/2=52 -> 50-62 X - 62-54 OK - 54-50 OK -> last pair chosen

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.travel_percentage = 0.33f;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.52f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_OK, "Status was changed from NO_ERROR");
}
void test_throttle_api_update_status_all_values_valid_two_implausible_pair() {
    float apps1 = 0.60F;
    float apps2 = 0.64F;
    float apps3 = 0.81F;
    //result should be 60+64/2=62

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.travel_percentage = 0.33f;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.62f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_OK, "Status was changed from NO_ERROR");
}

void test_throttle_api_update_status_two_values_valid_one_out_of_range() {
    float apps1 = 0.93F;
    float apps2 = 0.97F;
    float apps3 = 1.01F; // out of range

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.travel_percentage = 0.33f;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.95f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_OK, "Status was changed from NO_ERROR");
}

// check that this values make the throttle go into recoverable state (implausible values) (result = -1)

void test_throttle_api_update_status_all_values_valid_all_implausible_pair() {
    float apps1 = 0.40F;
    float apps2 = 0.54F;
    float apps3 = 0.71F;

    throttle_handler.travel_percentage = 0.15F;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.15F, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE, "Status isn't in IMPLAUSIBLE_RECOVERABLE");
}

void test_throttle_api_update_status_two_values_valid_one_out_of_range_no_valid_pair() {
    float apps1 = -0.03F;
    float apps2 = 0.87F;
    float apps3 = 0.99F;

    throttle_handler.travel_percentage = 0.15F;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.15F, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE, "Status isn't in IMPLAUSIBLE_RECOVERABLE");
}

void test_throttle_api_update_status_one_value_valid() {
    float apps1 = -0.03F;
    float apps2 = 1.87F;
    float apps3 = 0.99F;

    throttle_handler.travel_percentage = 0.15F;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.15F, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE, "Status isn't in IMPLAUSIBLE_RECOVERABLE");
}

void test_throttle_api_update_status_no_value_valid() {
    float apps1 = -0.03F;
    float apps2 = 1.87F;
    float apps3 = 1.99F;

    throttle_handler.travel_percentage = 0.15F;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.15F, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE, "Status isn't in IMPLAUSIBLE_RECOVERABLE");
}

// check OK -> IMPLAUSIBLE_RECOVERABLE behaviours
void test_throttle_api_update_status_to_recoverable_success() {
    float apps1 = 0.1F;
    float apps2 = 0.25F;
    float apps3 = 0.40F;

    throttle_handler.travel_percentage = 0.4F;

    test_throttle_start_timer_fake.return_val = THROTTLE_RC_OK;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.40f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE, "Status is not IMPLAUSIBLE_RECOVERABLE");
    TEST_ASSERT_EQUAL_MESSAGE(1, test_throttle_start_timer_fake.call_count, "Start timer wasn't called exactly once");
}

void test_throttle_api_update_status_to_recoverable_no_callback() {
    float apps1 = 0.1F;
    float apps2 = 0.25F;
    float apps3 = 0.40F;

    throttle_handler.travel_percentage = 0.4F;

    test_throttle_start_timer_fake.return_val = THROTTLE_RC_OK;

    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.40f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_CALLBACK_ERROR, "Status is not CALLBACK_ERROR");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_throttle_start_timer_fake.call_count, "Start timer was called");
}

void test_throttle_api_update_status_to_recoverable_callback_failure() {
    float apps1 = 0.1F;
    float apps2 = 0.25F;
    float apps3 = 0.40F;

    throttle_handler.travel_percentage = 0.4F;

    test_throttle_start_timer_fake.return_val = THROTTLE_RC_CALLBACK_FAILURE;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.40f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_CALLBACK_ERROR, "Status is not CALLBACK_ERROR");
    TEST_ASSERT_EQUAL_MESSAGE(1, test_throttle_start_timer_fake.call_count, "Start timer wasn't called exactly once");
}

// check IMPLAUSIBLE_RECOVERABLE -> OK behaviours
void test_throttle_api_update_status_to_ok_status_success() {
    float apps1 = 0.5F;
    float apps2 = 0.53F;
    float apps3 = 0.56F;

    throttle_handler.travel_percentage = 0.4F;
    throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE;

    test_throttle_reset_timer_fake.return_val = THROTTLE_RC_OK;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.53f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_OK, "Status is not OK");
    TEST_ASSERT_EQUAL_MESSAGE(1, test_throttle_reset_timer_fake.call_count, "Reset timer wasn't called exactly once");
}

void test_throttle_api_update_status_to_ok_status_no_callback() {
    float apps1 = 0.5F;
    float apps2 = 0.53F;
    float apps3 = 0.56F;

    throttle_handler.travel_percentage = 0.4F;
    throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE;

    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.4f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_CALLBACK_ERROR, "Status is not CALLBACK_ERROR");
    TEST_ASSERT_EQUAL_MESSAGE(0, test_throttle_reset_timer_fake.call_count, "Reset timer wasn't called exactly once");
}

void test_throttle_api_update_status_to_ok_status_callback_failure() {
    float apps1 = 0.5F;
    float apps2 = 0.53F;
    float apps3 = 0.56F;

    throttle_handler.travel_percentage = 0.4F;
    throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBILITY_RECOVERABLE;

    test_throttle_reset_timer_fake.return_val = THROTTLE_RC_CALLBACK_FAILURE;

    throttle_handler.start_timer = test_throttle_start_timer;
    throttle_handler.stop_timer = test_throttle_reset_timer;
    throttle_handler.apps_travel_percentages[0] = apps1;
    throttle_handler.apps_travel_percentages[1] = apps2;
    throttle_handler.apps_travel_percentages[2] = apps3;
    throttle_api_update_internal_status(5);

    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.travel_percentage, 0.4f, "Value wasn't as expected");
    TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.status, THROTTLE_STATUS_CALLBACK_ERROR, "Status is not CALLBACK_ERROR");
    TEST_ASSERT_EQUAL_MESSAGE(1, test_throttle_reset_timer_fake.call_count, "Reset timer wasn't called exactly once");
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_throttle_api_successful_init);
    RUN_TEST(test_throttle_api_init_no_first_member);
    RUN_TEST(test_throttle_api_init_no_second_member);

    RUN_TEST(test_throttle_api_update_status_stay_in_implausible_error);
    RUN_TEST(test_throttle_api_update_status_to_implausible_error);

    RUN_TEST(test_throttle_api_update_status_all_values_valid);
    RUN_TEST(test_throttle_api_update_status_two_values_valid_one_out_of_range);
    RUN_TEST(test_throttle_api_update_status_all_values_valid_two_implausible_pair);
    RUN_TEST(test_throttle_api_update_status_all_values_valid_one_implausible_pair);

    RUN_TEST(test_throttle_api_update_status_all_values_valid_all_implausible_pair);
    RUN_TEST(test_throttle_api_update_status_two_values_valid_one_out_of_range_no_valid_pair);
    RUN_TEST(test_throttle_api_update_status_one_value_valid);
    RUN_TEST(test_throttle_api_update_status_no_value_valid);

    RUN_TEST(test_throttle_api_update_status_to_recoverable_success);
    RUN_TEST(test_throttle_api_update_status_to_recoverable_no_callback);
    RUN_TEST(test_throttle_api_update_status_to_recoverable_callback_failure);

    RUN_TEST(test_throttle_api_update_status_to_ok_status_success);
    RUN_TEST(test_throttle_api_update_status_to_ok_status_no_callback);
    RUN_TEST(test_throttle_api_update_status_to_ok_status_callback_failure);

    UNITY_END();
}