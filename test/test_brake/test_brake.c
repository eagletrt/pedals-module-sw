#include "unity.h"
#include "fff.h"
#include "brake-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(float, BRAKE_pedal_travel_callback);
FAKE_VALUE_FUNC(float, BRAKE_front_pressure_callback);
FAKE_VALUE_FUNC(float, BRAKE_rear_pressure_callback);

extern struct BrakeHandler brake_handler;

void setUp(void) {
    RESET_FAKE(BRAKE_pedal_travel_callback);
    RESET_FAKE(BRAKE_front_pressure_callback);
	RESET_FAKE(BRAKE_rear_pressure_callback);
	FFF_RESET_HISTORY();
	brake_handler.get_pedal_travel = NULL;
	brake_handler.get_front_pressure = NULL;
	brake_handler.get_rear_pressure = NULL;
}

/*void tearDown(void) {}*/

void test_brake_init_handler_success(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, BRAKE_rear_pressure_callback);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_NO_ERROR, "Initialisation failed");
	TEST_ASSERT_EQUAL_MESSAGE(brake_handler.get_pedal_travel, BRAKE_pedal_travel_callback,"Not pointing to brake travel function");
	TEST_ASSERT_EQUAL_MESSAGE(brake_handler.get_front_pressure, BRAKE_front_pressure_callback, "Not pointing to front pressure function");
	TEST_ASSERT_EQUAL_MESSAGE(brake_handler.get_rear_pressure,BRAKE_rear_pressure_callback,"Not pointing to rear pressure function");
}

void test_brake_init_handler_failure(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, NULL);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_CALLBACK_FAILURE, "Initialisation succeeded even if it shouldn't have");
	TEST_ASSERT_EQUAL_MESSAGE(brake_handler.get_pedal_travel, NULL,"Not pointing to brake travel function");
	TEST_ASSERT_EQUAL_MESSAGE(brake_handler.get_front_pressure, NULL, "Not pointing to front pressure function");
	TEST_ASSERT_EQUAL_MESSAGE(brake_handler.get_rear_pressure, NULL, "Not pointing to rear pressure function");
}

void test_brake_travel_callback_failure(void){
	enum BrakeReturnCode res = brake_init_handler(NULL, BRAKE_front_pressure_callback, BRAKE_rear_pressure_callback);
	
	float result = 0.3F;
	res = brake_get_travel_percentage(&result);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_CALLBACK_FAILURE, "Callback should have failed");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(result, 0.3F, "Result of operation was touched");
}

void test_brake_travel_success(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, BRAKE_rear_pressure_callback);
	
	float ret_vals = 0.82F;
	SET_RETURN_SEQ(BRAKE_pedal_travel_callback, &ret_vals, 1);

	float result = 0.4F;
	res = brake_get_travel_percentage(&result);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_NO_ERROR, "Callback has failed");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(result, ret_vals, "Result is not as expected");
}

void test_brake_travel_low_perc(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, BRAKE_rear_pressure_callback);
	
	float ret_vals = -0.01F;
	SET_RETURN_SEQ(BRAKE_pedal_travel_callback, &ret_vals, 1);

	float result = 0.4F;
	res = brake_get_travel_percentage(&result);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_VALUE_OUTSIDE_RANGE, "Value was not truncated");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(result, 0.0F, "Result is not as expected");
}

void test_brake_travel_high_perc(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, BRAKE_rear_pressure_callback);
	
	float ret_vals = 1.03F;
	SET_RETURN_SEQ(BRAKE_pedal_travel_callback, &ret_vals, 1);

	float result = 0.4F;
	res = brake_get_travel_percentage(&result);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_VALUE_OUTSIDE_RANGE, "Value was not truncated");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(result, 1.0F, "Result is not as expected");
}

void test_brake_pressures_callback_failure(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, NULL);
	
	float front = 0.3F, rear = 0.5F;
	res = brake_get_pressures(&front, &rear);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_CALLBACK_FAILURE, "Callback should have failed");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(front, 0.3F, "Front was touched");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(rear, 0.5F, "Rear was touched");
}

void test_brake_pressures_success(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, BRAKE_rear_pressure_callback);
	
	float ret_front = 0.03F, ret_rear = 0.47F;
	SET_RETURN_SEQ(BRAKE_front_pressure_callback, &ret_front, 1);
	SET_RETURN_SEQ(BRAKE_rear_pressure_callback, &ret_rear, 1);

	float front = 0.3F, rear = 0.5F;
	res = brake_get_pressures(&front, &rear);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_NO_ERROR, "Callback should have failed");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(front, 0.03F * BRAKE_PRESSURE_CONVERSION_VALUE, "Front wasn't as expected");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(rear, 0.47F * BRAKE_PRESSURE_CONVERSION_VALUE, "Rear wasn't as expected");
}

void test_brake_pressures_outside_range(void){
	enum BrakeReturnCode res = brake_init_handler(BRAKE_pedal_travel_callback, BRAKE_front_pressure_callback, BRAKE_rear_pressure_callback);
	
	float ret_front = 0.15F, ret_rear = -0.04F;
	SET_RETURN_SEQ(BRAKE_front_pressure_callback, &ret_front, 1);
	SET_RETURN_SEQ(BRAKE_rear_pressure_callback, &ret_rear, 1);

	float front = 0.3F, rear = 0.5F;
	res = brake_get_pressures(&front, &rear);
	TEST_ASSERT_EQUAL_MESSAGE(res, BRAKE_RC_VALUE_OUTSIDE_RANGE, "Result is not as expected");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(front, 0.15F * BRAKE_PRESSURE_CONVERSION_VALUE, "Front wasn't as expected");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(rear, 0.0F, "Rear wasn't as expected");
}


int main( int argc, char **argv) {
    UNITY_BEGIN();

	RUN_TEST(test_brake_init_handler_success);
	RUN_TEST(test_brake_init_handler_failure);

	RUN_TEST(test_brake_travel_callback_failure);
	RUN_TEST(test_brake_travel_success);
	RUN_TEST(test_brake_travel_low_perc);
	RUN_TEST(test_brake_travel_high_perc);

	RUN_TEST(test_brake_pressures_callback_failure);
	RUN_TEST(test_brake_pressures_success);
	RUN_TEST(test_brake_pressures_outside_range);

    UNITY_END();
}