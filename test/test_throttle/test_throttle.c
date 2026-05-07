#include "unity.h"
#include "fff.h"
#include "throttle-api.h"
#include "fake_functions.h"

#include "test_adc_reading.h"
#include "adc-reading-api.h"
#include "stdint.h"


extern struct ThrottleHandler throttle_handler;

void setUp(void) {
    RESET_FAKE(test_start_timer);
    RESET_FAKE(test_reset_timer);
    FFF_RESET_HISTORY();
	throttle_handler.is_changed_to_implausible = false;
	throttle_handler.last_throttle_value = 0.0;
	throttle_handler.status = THROTTLE_STATUS_OK;
}

void tearDown(void) {
    // clean stuff up here
}

void test_throttle_timer_trigger(void){
	throttle_timer_trigger();
	TEST_ASSERT_TRUE(throttle_handler.is_changed_to_implausible);
}

void test_throttle_has_error_occured_false(void){
	throttle_handler.last_throttle_value = 0.50f;
	TEST_ASSERT_FALSE(throttle_has_error_occured());
}

void test_throttle_has_error_occured_true(void){
	throttle_handler.last_throttle_value = 0.50f;
	throttle_timer_trigger();
	TEST_ASSERT_TRUE_MESSAGE(throttle_has_error_occured(), "Throttle trigger was not activated");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f,throttle_handler.last_throttle_value, "Last value is not 0% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_IMPLAUSIBLE_ERROR, throttle_handler.status, "Throttle state is not equal to BAD");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Trigger boolean is still true");
	TEST_ASSERT_FALSE_MESSAGE(throttle_has_error_occured(), "Throttle trigger activated, even if it should not");

}

void test_throttle_three_ok_sensors_valid(void) { 
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) / 2; //50%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) / 2; //50%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) / 2; //50%

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, res, "Travel is not 50% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value, "Last value is not 50% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_OK, throttle_handler.status, "Throttle state is equal to not OK");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");

}

void test_throttle_three_ok_sensors_implausible(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.85; //85%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.89f; //89%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.96f; //96%

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, res, "Travel is not 50% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value, "Last value is not 50% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, throttle_handler.status, "Throttle state is not equal to UNSTABLE");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");
}

void test_throttle_two_ok_sensors_valid(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.85; //85%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.89f; //89%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 1.02f; //102% - not valid

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.87f, res, "Travel is not 87% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.87f, throttle_handler.last_throttle_value, "Last value is not 87% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_OK, throttle_handler.status, "Throttle state is not equal to OK");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");
}

void test_throttle_two_ok_sensors_implausible(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.85; //85%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.97f; //97%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 1.02f; //102% - not valid

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, res, "Travel is not 50% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value, "Last value is not 50% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, throttle_handler.status, "Throttle state is not equal to UNSTABLE");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");
}

void test_throttle_no_ok_sensors(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 1.01; //101%  - not valid
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 1.03f; //103% - not valid
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 1.02f; //102% - not valid

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, res, "Travel is not 50% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value, "Last value is not 50% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, throttle_handler.status, "Throttle state is not equal to OK");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");
}

void test_throttle_is_timer_start(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.02; // 2% 
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.08f; // 8% 
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.14f; // 14%, now invalid because of 2%-14% difference

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);
	throttle_handler.last_throttle_value = 0.07f;

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.07f, res, "Travel percentage result is not 7% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value, "Last throttle value is not 7% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, throttle_handler.status, "Throttle status is not equal to IMPLAUSIBLE_RECOVERABLE");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");
	TEST_ASSERT_EQUAL_MESSAGE(1, test_start_timer_fake.call_count, "Expected 1 call to start timer");
	TEST_ASSERT_EQUAL_MESSAGE(0, test_reset_timer_fake.call_count, "Expected 0 calls to reset timer");
}

void test_throttle_is_timer_reset(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.02; // 2% 
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.08f; // 8% 
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.11f; // 11%

	throttle_handler.last_throttle_value = 0.50f;
	throttle_handler.status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.07f, res, "Travel percentage result is not 7% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value, "Last throttle value is not 7% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_OK, throttle_handler.status, "Throttle status is not equal to OK");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");
	TEST_ASSERT_EQUAL_MESSAGE(0, test_start_timer_fake.call_count, "Expected 0 calls to start timer");
	TEST_ASSERT_EQUAL_MESSAGE(1, test_reset_timer_fake.call_count, "Expected 1 call to reset timer");
}

int main( int argc, char **argv) {
    UNITY_BEGIN();
	RUN_TEST(test_throttle_timer_trigger);
	RUN_TEST(test_throttle_has_error_occured_false);
	RUN_TEST(test_throttle_has_error_occured_true);

	RUN_TEST(test_throttle_three_ok_sensors_valid);
	RUN_TEST(test_throttle_three_ok_sensors_implausible);
	RUN_TEST(test_throttle_two_ok_sensors_valid);
	RUN_TEST(test_throttle_two_ok_sensors_implausible);
	RUN_TEST(test_throttle_no_ok_sensors);

	RUN_TEST(test_throttle_is_timer_start);
	RUN_TEST(test_throttle_is_timer_reset);
    UNITY_END();
}