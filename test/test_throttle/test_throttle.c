#include "unity.h"
#include "fff.h"
#include "throttle-api.h"
#include "fake_functions.h"

#include "test_adc_reading.h"
#include "adc_reading.h"
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

void test_throttle_is_throttle_bad_false(void){
	throttle_handler.last_throttle_value = 0.50f;
	TEST_ASSERT_FALSE(throttle_is_throttle_bad());
}

void test_throttle_is_throttle_bad_true(void){
	throttle_handler.last_throttle_value = 0.50f;
	throttle_timer_trigger();
	TEST_ASSERT_TRUE(throttle_is_throttle_bad());
	TEST_ASSERT_EQUAL_FLOAT(0.0f,throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_BAD, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
	TEST_ASSERT_FALSE(throttle_is_throttle_bad());

}

void test_throttle_three_ok_sensors_valid(void) { 
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) / 2; //50%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) / 2; //50%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) / 2; //50%

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_OK, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);

}

void test_throttle_three_ok_sensors_implausible(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.85; //85%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.89f; //89%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.96f; //96%

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_UNSTABLE, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
}

void test_throttle_two_ok_sensors_valid(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.85; //85%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.89f; //89%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 1.02f; //102% - not valid

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.87f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.87f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_OK, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
}

void test_throttle_two_ok_sensors_implausible(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.85; //85%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.97f; //97%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 1.02f; //102% - not valid

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_UNSTABLE, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
}

void test_throttle_no_ok_sensors(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 1.01; //101%  - not valid
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 1.03f; //103% - not valid
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 1.02f; //102% - not valid

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.50f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_UNSTABLE, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
}

void test_throttle_sim_valid_unstable_valid(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.02; // 2% 
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.08f; // 8% 
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.11f; // 11%

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_OK, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
	TEST_ASSERT_EQUAL(0, test_start_timer_fake.call_count);
	TEST_ASSERT_EQUAL(0, test_reset_timer_fake.call_count);

	apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.14f; // 14%, now invalid because of 2%-14% difference

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);
	res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_UNSTABLE, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
	TEST_ASSERT_EQUAL(1, test_start_timer_fake.call_count);
	TEST_ASSERT_EQUAL(0, test_reset_timer_fake.call_count);

	apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.08f; // 8%, back to a normal alvalue

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);
	res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.06f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.06f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_OK, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
	TEST_ASSERT_EQUAL(1, test_start_timer_fake.call_count);
	TEST_ASSERT_EQUAL(1, test_reset_timer_fake.call_count);
}

void test_throttle_sim_valid_unstable_bad(void) {
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.02; // 2% 
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) * 0.08f; // 8% 
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) * 0.11f; // 11%

	throttle_handler.last_throttle_value = 0.50f;

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_OK, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
	TEST_ASSERT_EQUAL(0, test_start_timer_fake.call_count);
	TEST_ASSERT_EQUAL(0, test_reset_timer_fake.call_count);

	apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) * 0.0f; // 14%, now invalid because of 0%-11% difference

	insert_values_in_array(ADC_READING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);
	res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_UNSTABLE, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
	TEST_ASSERT_EQUAL(1, test_start_timer_fake.call_count);
	TEST_ASSERT_EQUAL(0, test_reset_timer_fake.call_count);

	throttle_timer_trigger(); // simulate timer going off
	res = throttle_get_travel_percentage();

	// this part should not change until throttle_is_throttle_bad gets called
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_UNSTABLE, throttle_handler.status);
	TEST_ASSERT_TRUE(throttle_handler.is_changed_to_implausible); // only this variable is triggered
	TEST_ASSERT_EQUAL(1, test_start_timer_fake.call_count);
	TEST_ASSERT_EQUAL(0, test_reset_timer_fake.call_count);

	bool tmp = throttle_is_throttle_bad();
	res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.0f, res);
	TEST_ASSERT_FLOAT_WITHIN(THROTTLE_APPS_EPSILON, 0.0f, throttle_handler.last_throttle_value);
	TEST_ASSERT_EQUAL(THROTTLE_STATUS_BAD, throttle_handler.status);
	TEST_ASSERT_FALSE(throttle_handler.is_changed_to_implausible);
	TEST_ASSERT_EQUAL(1, test_start_timer_fake.call_count);
	TEST_ASSERT_EQUAL(0, test_reset_timer_fake.call_count);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();
	RUN_TEST(test_throttle_timer_trigger);
	RUN_TEST(test_throttle_is_throttle_bad_false);
	RUN_TEST(test_throttle_is_throttle_bad_true);

	RUN_TEST(test_throttle_three_ok_sensors_valid);
	RUN_TEST(test_throttle_three_ok_sensors_implausible);
	RUN_TEST(test_throttle_two_ok_sensors_valid);
	RUN_TEST(test_throttle_two_ok_sensors_implausible);
	RUN_TEST(test_throttle_no_ok_sensors);

	RUN_TEST(test_throttle_sim_valid_unstable_valid);
	RUN_TEST(test_throttle_sim_valid_unstable_bad);
    UNITY_END();
}