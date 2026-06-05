#include "unity.h"
#include "fff.h"
#include "throttle-api.h"
#include "voltage-scaling-api.h"
#include "stdint.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(int32_t, VOLTAGE_SCALING_read_raw, enum SensorTypesName);
FAKE_VALUE_FUNC(enum ThrottleReturnCode, THROTTLE_start_timer);
FAKE_VALUE_FUNC(enum ThrottleReturnCode, THROTTLE_reset_timer);

extern struct VoltageScalingHandler voltage_scaling_handler;
extern struct ThrottleHandler throttle_handler;


void setUp(void) {
    RESET_FAKE(VOLTAGE_SCALING_read_raw);
    FFF_RESET_HISTORY();
	voltage_scaling_handler.read_raw = NULL;
	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;
	throttle_handler.last_throttle_value = 0.0f;
	throttle_handler.start_timer = NULL;
	throttle_handler.stop_timer = NULL;
	throttle_handler.throttle_status = THROTTLE_STATUS_OK;
}

/*void tearDown(void) {
    // clean stuff up here
}*/


void test_throttle_successful_init(void){
	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);

	TEST_ASSERT_EQUAL_MESSAGE(rc,THROTTLE_RC_NO_ERROR,"Error during initialisation");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.start_timer,THROTTLE_start_timer,"Start timer was not initialised");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.stop_timer,THROTTLE_reset_timer,"Reset timer was not initialised");
}

void test_throttle_init_failure(void){
	enum ThrottleReturnCode rc = throttle_init(NULL,NULL);
	TEST_ASSERT_EQUAL_MESSAGE(rc,THROTTLE_RC_CALLBACK_FAILURE,"Error return value was different than expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.start_timer,NULL,"Start timer was initialised");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.stop_timer,NULL,"Reset timer was not initialised");

	rc = throttle_init(THROTTLE_start_timer,NULL);
	TEST_ASSERT_EQUAL_MESSAGE(rc,THROTTLE_RC_CALLBACK_FAILURE,"Error return value was different than expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.start_timer,NULL,"Start timer was initialised");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.stop_timer,NULL,"Reset timer was not initialised");

	rc = throttle_init(NULL,THROTTLE_reset_timer);
	TEST_ASSERT_EQUAL_MESSAGE(rc,THROTTLE_RC_CALLBACK_FAILURE,"Error return value was different than expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.start_timer,NULL,"Start timer was initialised");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.stop_timer,NULL,"Reset timer was not initialised");
}

void test_throttle_timer_trigger(){
	throttle_timer_trigger();
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status,THROTTLE_RC_IMPLAUSIBILITY,"Error was not about implausibility");
}

void test_throttle_get_error_from_ok(){
	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;
	TEST_ASSERT_EQUAL_MESSAGE(throttle_get_error_status(),THROTTLE_RC_NO_ERROR,"An error status was found");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error status remained in the handler");
}

void test_throttle_get_error_from_callback_failure(){
	throttle_handler.error_status = THROTTLE_RC_CALLBACK_FAILURE;
	TEST_ASSERT_EQUAL_MESSAGE(throttle_get_error_status(),THROTTLE_RC_CALLBACK_FAILURE,"Error status was not about callback");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error status remained in the handler");
}

void test_throttle_get_error_from_implausibility(){
	throttle_handler.error_status = THROTTLE_RC_IMPLAUSIBILITY;
	throttle_handler.throttle_status = THROTTLE_STATUS_OK;
	throttle_handler.last_throttle_value = 0.5f;
	TEST_ASSERT_EQUAL_MESSAGE(throttle_get_error_status(),THROTTLE_RC_IMPLAUSIBILITY,"An error status was found");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error status remained in the handler");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.0f, "Last value was not reset");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_IMPLAUSIBLE_ERROR, "Status was not set to implausibility");
}

void test_throttle_internal_percentage_callback_failure(void){
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) / 2; //50%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) / 2; //50%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) / 2; //50%
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;

	int32_t return_values[6] = { CURRENT, apps1, CURRENT, apps2, CURRENT, apps3 };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 6);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(res,0.0f,"Result was not equal to zero");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_get_error_status(),THROTTLE_RC_CALLBACK_FAILURE,"Return was not about callback failure");
}
/*

void test_throttle_three_ok_sensors_valid(void) { 
	int32_t apps1 = THROTTLE_APPS_1_MIN_VALUE + (THROTTLE_APPS_1_MAX_VALUE - THROTTLE_APPS_1_MIN_VALUE) / 2; //50%
	int32_t apps2 = THROTTLE_APPS_2_MIN_VALUE + (THROTTLE_APPS_2_MAX_VALUE - THROTTLE_APPS_2_MIN_VALUE) / 2; //50%
	int32_t apps3 = THROTTLE_APPS_3_MIN_VALUE + (THROTTLE_APPS_3_MAX_VALUE - THROTTLE_APPS_3_MIN_VALUE) / 2; //50%
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;

	int32_t return_values[6] = { CURRENT, apps1, CURRENT, apps2, CURRENT, apps3 };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 6);

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

	insert_values_in_array(VOLTAGE_SCALING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

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

	insert_values_in_array(VOLTAGE_SCALING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

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

	insert_values_in_array(VOLTAGE_SCALING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

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

	insert_values_in_array(VOLTAGE_SCALING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

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

	insert_values_in_array(VOLTAGE_SCALING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);
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

	insert_values_in_array(VOLTAGE_SCALING_MAX_RAW_VALUE,0,0,0,apps1,apps2,apps3);

	float res = throttle_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.07f, res, "Travel percentage result is not 7% as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(THROTTLE_APPS_EPSILON, 0.07f, throttle_handler.last_throttle_value, "Last throttle value is not 7% as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_STATUS_OK, throttle_handler.status, "Throttle status is not equal to OK");
	TEST_ASSERT_FALSE_MESSAGE(throttle_handler.is_changed_to_implausible, "Throttle trigger activated, even if it should not");
	TEST_ASSERT_EQUAL_MESSAGE(0, test_start_timer_fake.call_count, "Expected 0 calls to start timer");
	TEST_ASSERT_EQUAL_MESSAGE(1, test_reset_timer_fake.call_count, "Expected 1 call to reset timer");
}
*/

int main( int argc, char **argv) {
    UNITY_BEGIN();
	
	RUN_TEST(test_throttle_successful_init);
	RUN_TEST(test_throttle_init_failure);

	RUN_TEST(test_throttle_timer_trigger);

	RUN_TEST(test_throttle_get_error_from_ok);
	RUN_TEST(test_throttle_get_error_from_callback_failure);
	RUN_TEST(test_throttle_get_error_from_implausibility);

	RUN_TEST(test_throttle_internal_percentage_callback_failure);

	//RUN_TEST(test_throttle_get_error_status_false);
	//RUN_TEST(test_throttle_get_error_status_true);

	//RUN_TEST(test_throttle_three_ok_sensors_valid);
	//RUN_TEST(test_throttle_three_ok_sensors_implausible);
	//RUN_TEST(test_throttle_two_ok_sensors_valid);
	//RUN_TEST(test_throttle_two_ok_sensors_implausible);
	//RUN_TEST(test_throttle_no_ok_sensors);

	//RUN_TEST(test_throttle_is_timer_start);
	//RUN_TEST(test_throttle_is_timer_reset);
    UNITY_END();
}