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

/*
void test_brake_bpps_percentage_in_range(void) {
	const float MIN_VAL = BRAKE_BPPS_MIN_VALUE, MAX_VAL = BRAKE_BPPS_MAX_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)(MIN_VAL + (MAX_VAL - MIN_VAL) / 2);

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode rc = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(rc,VOLTAGE_SCALING_RC_OK,"Initialisation failed");
	float res = brake_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001,0.50f,res,"Result is outside expected range"); // [49.9%,50.1%] range is valid
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_NO_ERROR,"Error was detected even if it should have been valid"); // Since the value is in range, it should be valid
}

void test_brake_bpps_percentage_outside_range_lower(void) {
	const float MIN_VAL = BRAKE_BPPS_MIN_VALUE, MAX_VAL = BRAKE_BPPS_MAX_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)MIN_VAL - 1; // Set sensor value outside the valid range

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode rc = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(rc,VOLTAGE_SCALING_RC_OK,"Initialisation failed");
	float res = brake_get_travel_percentage();

	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f,res,"Result is not truncated to 0.0 to expected"); // Should be truncated to 0.0f
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_VALUE_OUTSIDE_RANGE,"Error was not found as outside range");
}

void test_brake_bpps_percentage_outside_range_upper(void) {
	const float MIN_VAL = BRAKE_BPPS_MIN_VALUE, MAX_VAL = BRAKE_BPPS_MAX_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)MAX_VAL + 1; // Set sensor value outside the valid range

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode rc = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(rc,VOLTAGE_SCALING_RC_OK,"Initialisation failed");
	float res = brake_get_travel_percentage();

	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1.0f,res,"Result is not truncated as 1.0 to expected"); // Should be truncated to 1.0f
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_VALUE_OUTSIDE_RANGE,"Error was not found as outside range");
}

void test_brake_bpps_percentage_callback_failure(void){
	const float MIN_VAL = BRAKE_BPPS_MIN_VALUE, MAX_VAL = BRAKE_BPPS_MAX_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)(MIN_VAL + (MAX_VAL - MIN_VAL) / 2);

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	float res = brake_get_travel_percentage();

	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f,res,"Result is not 0.0 as expected");
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_CALLBACK_FAILURE,"Error was not found as callback failure");
}


void test_brake_front_pressure_success() {
	const int32_t MIN_VAL = VOLTAGE_SCALING_MIN_RAW_VALUE, MAX_VAL = VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)(MIN_VAL + (MAX_VAL - MIN_VAL) / 2);

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode rc = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(rc,VOLTAGE_SCALING_RC_OK,"Initialisation failed");
	float res = brake_get_front_pressure();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001 * BRAKE_BSPS_F_CONVERSION_VALUE, 0.50f * BRAKE_BSPS_F_CONVERSION_VALUE, res,"Pressure is not as expected"); // [49.9%,50.1%] range is valid
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_NO_ERROR,"Error was detected even if it should have been valid");
}

void test_brake_front_pressure_failure() {
	const int32_t MIN_VAL = VOLTAGE_SCALING_MIN_RAW_VALUE, MAX_VAL = VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)(MIN_VAL + (MAX_VAL - MIN_VAL) / 2);

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	float res = brake_get_front_pressure();

	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f,res,"Result is not 0.0 as expected");
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_CALLBACK_FAILURE,"Error was not found as callback failure");
}

void test_brake_rear_pressure_success() {
	const int32_t MIN_VAL = VOLTAGE_SCALING_MIN_RAW_VALUE, MAX_VAL = VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)(MIN_VAL + (MAX_VAL - MIN_VAL) / 2);

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode rc = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(rc,VOLTAGE_SCALING_RC_OK,"Initialisation failed");
	float res = brake_get_rear_pressure();

	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001 * BRAKE_BSPS_R_CONVERSION_VALUE, 0.50f * BRAKE_BSPS_R_CONVERSION_VALUE, res,"Pressure is not as expected"); // [49.9%,50.1%] range is valid
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_NO_ERROR,"Error was detected even if it should have been valid");
}

void test_brake_rear_pressure_failure() {
	const int32_t MIN_VAL = VOLTAGE_SCALING_MIN_RAW_VALUE, MAX_VAL = VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = (int32_t)(MIN_VAL + (MAX_VAL - MIN_VAL) / 2);

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	float res = brake_get_rear_pressure();

	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.0f,res,"Result is not 0.0 as expected");
	TEST_ASSERT_EQUAL_MESSAGE(brake_get_error_status(),BRAKE_RC_CALLBACK_FAILURE,"Error was not found as callback failure");
}
*/

int main( int argc, char **argv) {
    UNITY_BEGIN();

	RUN_TEST(test_brake_init_handler_success);
	RUN_TEST(test_brake_init_handler_failure);

//	RUN_TEST(test_brake_bpps_percentage_in_range);
//	RUN_TEST(test_brake_bpps_percentage_outside_range_lower);
//	RUN_TEST(test_brake_bpps_percentage_outside_range_upper);
//	RUN_TEST(test_brake_bpps_percentage_callback_failure);
//	RUN_TEST(test_brake_front_pressure_success);
//	RUN_TEST(test_brake_front_pressure_failure);
//	RUN_TEST(test_brake_rear_pressure_success);
//	RUN_TEST(test_brake_rear_pressure_failure);
    UNITY_END();
}