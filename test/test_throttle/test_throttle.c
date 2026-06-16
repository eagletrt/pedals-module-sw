#include "unity.h"
#include "fff.h"
#include "throttle-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum ThrottleReturnCode, THROTTLE_start_timer);
FAKE_VALUE_FUNC(enum ThrottleReturnCode, THROTTLE_reset_timer);

extern struct ThrottleHandler throttle_handler;


void setUp(void) {
    RESET_FAKE(THROTTLE_start_timer);
	RESET_FAKE(THROTTLE_reset_timer)
    FFF_RESET_HISTORY();
	throttle_handler.is_implausibility_timeout = false;
	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;
	throttle_handler.last_throttle_value = 0.0f;
	throttle_handler.start_timer = NULL;
	throttle_handler.stop_timer = NULL;
	throttle_handler.throttle_status = THROTTLE_STATUS_OK;
}

/*void tearDown(void) {
    // clean stuff up here
}*/

// throttle_implausibility_timeout_trigger will not be tested because it's a simple setter

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

// check if the implausible error state and related actions are as intended
void test_throttle_update_to_implausible_error(){
	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);

	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;
	throttle_handler.throttle_status = THROTTLE_STATUS_OK;
	throttle_handler.last_throttle_value = 0.5f;

	throttle_implausibility_timeout_trigger();

	throttle_update_pedal_values(0.3F, 0.32F, 0.31F); // actual values don't matter, will be reset to zero

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_IMPLAUSIBILITY, "Error was not implausibility");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.0f, "Last value was not reset");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_IMPLAUSIBLE_ERROR, "Status was not set to implausibility");
}

void test_throttle_update_stay_in_implausible_error(){
	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);

	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;
	throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_ERROR;
	throttle_handler.last_throttle_value = 0.0f;

	throttle_update_pedal_values(0.3F, 0.32F, 0.31F); // actual values don't matter, will be reset to zero

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error was found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.0f, "Last value was changed, even if it shouldn't have");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_IMPLAUSIBLE_ERROR, "Status was changed from implausibility");
}

// check if these values are handled correctly and keep the throttle in NO_ERROR state

void test_throttle_update_all_values_valid(){
	float apps1 = 0.50F;
	float apps2 = 0.52F;
	float apps3 = 0.54F;

	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);
	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error was found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.52f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_OK, "Status was changed from NO_ERROR");
}

void test_throttle_update_two_values_valid_one_out_of_range(){
	float apps1 = 0.93F;
	float apps2 = 0.97F;
	float apps3 = 1.01F; // out of range

	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);
	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error was found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.95f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_OK, "Status was changed from NO_ERROR");
}
//--------------------------------------------------
//--------------------------------------------------
// check OK -> IMPLAUSIBLE_RECOVERABLE behaviours
void test_throttle_update_to_implausible(){
	float apps1 = 0.1F;
	float apps2 = 0.25F;
	float apps3 = 0.40F;

	throttle_handler.last_throttle_value = 0.4F;

	THROTTLE_start_timer_fake.return_val = THROTTLE_RC_NO_ERROR;

	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);
	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error was found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.40f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, "Status was changed from NO_ERROR");
	TEST_ASSERT_EQUAL_MESSAGE(1, THROTTLE_start_timer_fake.call_count, "Start timer wasn't called exactly once");
}
/*
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

void test_throttle_get_travel_percentage_no_error(void){
	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);

	throttle_handler.last_throttle_value = 0.4F;
	throttle_handler.error_status = THROTTLE_RC_NO_ERROR;

	struct ThrottleReturnValue ret = throttle_get_travel_percentage();
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.4F,ret.throttle_value,"Return percentage was not as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_RC_NO_ERROR,ret.throttle_error,"An error was returned");
}

void test_throttle_get_travel_percentage_error_found(void){
	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);

	throttle_handler.last_throttle_value = 0.5F;
	throttle_handler.error_status = THROTTLE_RC_IMPLAUSIBILITY;

	struct ThrottleReturnValue ret = throttle_get_travel_percentage();
	TEST_ASSERT_EQUAL_MESSAGE(0.5F,ret.throttle_value,"Return percentage was not as expected");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_RC_IMPLAUSIBILITY,ret.throttle_error,"Error should have been implausibility");
	TEST_ASSERT_EQUAL_MESSAGE(THROTTLE_RC_NO_ERROR,throttle_handler.error_status,"Internal error status did not go back to NO_ERROR");
}

int main( int argc, char **argv) {
    UNITY_BEGIN();
	
	RUN_TEST(test_throttle_successful_init);
	RUN_TEST(test_throttle_init_failure);

	RUN_TEST(test_throttle_update_stay_in_implausible_error);
	RUN_TEST(test_throttle_update_to_implausible_error);

	RUN_TEST(test_throttle_update_all_values_valid);
	RUN_TEST(test_throttle_update_two_values_valid_one_out_of_range);

	RUN_TEST(test_throttle_update_to_implausible);

	//RUN_TEST(test_throttle_timer_trigger);

	//RUN_TEST(test_throttle_get_error_from_ok);
	//RUN_TEST(test_throttle_get_error_from_callback_failure);
	//RUN_TEST(test_throttle_get_error_from_implausibility);

	//RUN_TEST(test_throttle_internal_percentage_callback_failure);

	//RUN_TEST(test_throttle_get_error_status_false);
	//RUN_TEST(test_throttle_get_error_status_true);

	//RUN_TEST(test_throttle_three_ok_sensors_valid);
	//RUN_TEST(test_throttle_three_ok_sensors_implausible);
	//RUN_TEST(test_throttle_two_ok_sensors_valid);
	//RUN_TEST(test_throttle_two_ok_sensors_implausible);
	//RUN_TEST(test_throttle_no_ok_sensors);

	//RUN_TEST(test_throttle_is_timer_start);
	//RUN_TEST(test_throttle_is_timer_reset);

	RUN_TEST(test_throttle_get_travel_percentage_no_error);
	RUN_TEST(test_throttle_get_travel_percentage_error_found);
    UNITY_END();
}