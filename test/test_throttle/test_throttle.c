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
void test_throttle_update_to_recoverable_success(){
	float apps1 = 0.1F;
	float apps2 = 0.25F;
	float apps3 = 0.40F;

	throttle_handler.last_throttle_value = 0.4F;

	THROTTLE_start_timer_fake.return_val = THROTTLE_RC_NO_ERROR;

	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);
	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error was found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.40f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, "Status is not IMPLAUSIBLE_RECOVERABLE");
	TEST_ASSERT_EQUAL_MESSAGE(1, THROTTLE_start_timer_fake.call_count, "Start timer wasn't called exactly once");
}

void test_throttle_update_to_recoverable_no_callback(){
	float apps1 = 0.1F;
	float apps2 = 0.25F;
	float apps3 = 0.40F;

	throttle_handler.last_throttle_value = 0.4F;

	THROTTLE_start_timer_fake.return_val = THROTTLE_RC_NO_ERROR;

	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_CALLBACK_FAILURE, "CALLBACK_FAILURE was not found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.40f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, "Status is not IMPLAUSIBLE_RECOVERABLE");
	TEST_ASSERT_EQUAL_MESSAGE(0, THROTTLE_start_timer_fake.call_count, "Start timer was called");
}

void test_throttle_update_to_recoverable_callback_failure(){
	float apps1 = 0.1F;
	float apps2 = 0.25F;
	float apps3 = 0.40F;

	throttle_handler.last_throttle_value = 0.4F;

	THROTTLE_start_timer_fake.return_val = THROTTLE_RC_CALLBACK_FAILURE;

	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);
	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_CALLBACK_FAILURE, "CALLBACK_FAILURE was not found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.40f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE, "Status is not IMPLAUSIBLE_RECOVERABLE");
	TEST_ASSERT_EQUAL_MESSAGE(1, THROTTLE_start_timer_fake.call_count, "Start timer wasn't called exactly once");
}

// check IMPLAUSIBLE_RECOVERABLE -> OK behaviours
void test_throttle_update_to_ok_status_success(){
	float apps1 = 0.5F;
	float apps2 = 0.53F;
	float apps3 = 0.56F;

	throttle_handler.last_throttle_value = 0.4F;
	throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;

	THROTTLE_reset_timer_fake.return_val = THROTTLE_RC_NO_ERROR;

	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);
	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_NO_ERROR, "An error was found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.53f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_OK, "Status is not OK");
	TEST_ASSERT_EQUAL_MESSAGE(1, THROTTLE_reset_timer_fake.call_count, "Reset timer wasn't called exactly once");
}

void test_throttle_update_to_ok_status_no_callback(){
	float apps1 = 0.5F;
	float apps2 = 0.53F;
	float apps3 = 0.56F;

	throttle_handler.last_throttle_value = 0.4F;
	throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;

	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_CALLBACK_FAILURE, "CALLBACK_FAILURE was not found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.53f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_OK, "Status is not OK");
	TEST_ASSERT_EQUAL_MESSAGE(0, THROTTLE_reset_timer_fake.call_count, "Reset timer wasn't called exactly once");
}

void test_throttle_update_to_ok_status_callback_failure(){
	float apps1 = 0.5F;
	float apps2 = 0.53F;
	float apps3 = 0.56F;

	throttle_handler.last_throttle_value = 0.4F;
	throttle_handler.throttle_status = THROTTLE_STATUS_IMPLAUSIBLE_RECOVERABLE;

	THROTTLE_reset_timer_fake.return_val = THROTTLE_RC_CALLBACK_FAILURE;

	enum ThrottleReturnCode rc = throttle_init(THROTTLE_start_timer,THROTTLE_reset_timer);
	throttle_update_pedal_values(apps1,apps2,apps3);

	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.error_status, THROTTLE_RC_CALLBACK_FAILURE, "CALLBACK_FAILURE was not found");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(throttle_handler.last_throttle_value, 0.53f, "Value wasn't as expected");
	TEST_ASSERT_EQUAL_MESSAGE(throttle_handler.throttle_status, THROTTLE_STATUS_OK, "Status is not OK");
	TEST_ASSERT_EQUAL_MESSAGE(1, THROTTLE_reset_timer_fake.call_count, "Reset timer wasn't called exactly once");
}
//////////////////////////////////////

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

	RUN_TEST(test_throttle_update_to_recoverable_success);
	RUN_TEST(test_throttle_update_to_recoverable_no_callback);
	RUN_TEST(test_throttle_update_to_recoverable_callback_failure);

	RUN_TEST(test_throttle_update_to_ok_status_success);
	RUN_TEST(test_throttle_update_to_ok_status_no_callback);
	RUN_TEST(test_throttle_update_to_ok_status_callback_failure);

	RUN_TEST(test_throttle_get_travel_percentage_no_error);
	RUN_TEST(test_throttle_get_travel_percentage_error_found);
    UNITY_END();
}