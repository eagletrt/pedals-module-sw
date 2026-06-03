#include <unity.h>
#include "fff.h"
#include "voltage-scaling-api.h"
//#include "test_adc_reading.h"
//#include "fake_functions.h" // to avoid linking errors

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(int32_t, VOLTAGE_SCALING_read_raw, enum SensorTypesName);

extern struct VoltageScalingHandler voltage_scaling_handler;

void setUp(void) {
    RESET_FAKE(VOLTAGE_SCALING_read_raw);
    FFF_RESET_HISTORY();
	voltage_scaling_handler.read_raw = NULL;
}

/*void tearDown(void) {
    // clean stuff up here
}*/

void test_init_successful(void){
	enum VoltageScalingReturnCode res = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(VOLTAGE_SCALING_read_raw,voltage_scaling_handler.read_raw,"Function was not initialised properly");
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
}

void test_init_failure(void){
	enum VoltageScalingReturnCode res = voltage_scaling_init(NULL);
	TEST_ASSERT_EQUAL_MESSAGE(NULL,voltage_scaling_handler.read_raw,"Function was surprisingly initialised");
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_ERROR,"Return value is not as expected");
}


void test_call_without_initialisation(void){
	const float MIN_VAL = 500.0f, MAX_VAL = 2500.0f, CURRENT = VOLTAGE_SCALING_MAX_RAW_VALUE;
	int32_t sensor = 1500;

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	float perc = 0.0f;
	enum VoltageScalingReturnCode res = voltage_scaling_get_percentage(&perc,SENSOR_TYPES_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_ERROR,"Return value is not as expected");
	TEST_ASSERT_EQUAL_MESSAGE(0.0f,perc,"Percentage value was somehow changed");
}


void test_get_percentage_full_current_valid_percentage(void){
	const float MIN_VAL = 500.0f, MAX_VAL = 2500.0f;
	int32_t sensor = 1500;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode res = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	float perc = 0.0f;
	res = voltage_scaling_get_percentage(&perc,SENSOR_TYPES_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	TEST_ASSERT_EQUAL_FLOAT_MESSAGE(0.50f,perc,"Percentage value is not as expected");
}

void test_get_percentage_full_current_low_percentage(void){
	const float MIN_VAL = 500.0f, MAX_VAL = 2500.0f;
	int32_t sensor = 499;
	int32_t CURRENT = (int32_t)VOLTAGE_SCALING_MAX_RAW_VALUE;

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode res = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	float perc = 0.0f;
	res = voltage_scaling_get_percentage(&perc,SENSOR_TYPES_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	TEST_ASSERT_LESS_THAN_FLOAT_MESSAGE(0.0f, perc, "Percentage value is not as expected");
}

void test_get_percentage_full_current_high_percentage(void){
	const float MIN_VAL = 500.0f, MAX_VAL = 2500.0f;
	int32_t sensor = 2501;
	int32_t CURRENT = VOLTAGE_SCALING_MAX_RAW_VALUE;

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode res = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	float perc = 0.0f;
	res = voltage_scaling_get_percentage(&perc,SENSOR_TYPES_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(1.0f,perc,"Percentage value is not as expected");
}

void test_get_percentage_low_current_valid_percentage(void){
	const float MIN_VAL = 500.0f, MAX_VAL = 2500.0f; 
	int32_t CURRENT = 3890; //95% of full current, approx is 94.9%
	int32_t sensor = 1425;

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode res = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	float perc = 0.0f;
	res = voltage_scaling_get_percentage(&perc,SENSOR_TYPES_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	TEST_ASSERT_FLOAT_WITHIN_MESSAGE(0.001,0.50f,perc,"Percentage value is not as expected"); // [49.9%,50.1%] range is valid
}

void test_get_percentage_low_current_low_percentage(void){
	const float MIN_VAL = 500.0f, MAX_VAL = 2500.0f; 
	int32_t CURRENT = 3890; //95% of full current
	int32_t sensor = 474; //94.9% of 500 is 474.9 

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode res = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	float perc = 0.0f;
	res = voltage_scaling_get_percentage(&perc,SENSOR_TYPES_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	TEST_ASSERT_LESS_THAN_FLOAT_MESSAGE(0.0f,perc,"Percentage value is not as expected");
}

void test_get_percentage_low_current_high_percentage(void){
	const float MIN_VAL = 500.0f, MAX_VAL = 2500.0f; 
	int32_t CURRENT = 3890; //95% of full current
	int32_t sensor = 2375; // 94.9% of 2500

	int32_t return_values[2] = { CURRENT, sensor };
    SET_RETURN_SEQ(VOLTAGE_SCALING_read_raw, return_values, 2);

	enum VoltageScalingReturnCode res = voltage_scaling_init(VOLTAGE_SCALING_read_raw);
	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	float perc = 0.0f;
	res = voltage_scaling_get_percentage(&perc,SENSOR_TYPES_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_MESSAGE(res,VOLTAGE_SCALING_RC_OK,"Return value is not as expected");
	TEST_ASSERT_GREATER_THAN_FLOAT_MESSAGE(1.0f,perc,"Percentage value is not as expected");
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
	RUN_TEST(test_init_successful);
	RUN_TEST(test_init_failure);
	RUN_TEST(test_call_without_initialisation);
	RUN_TEST(test_get_percentage_full_current_valid_percentage);
	RUN_TEST(test_get_percentage_full_current_low_percentage);
	RUN_TEST(test_get_percentage_full_current_high_percentage);
	RUN_TEST(test_get_percentage_low_current_valid_percentage);
	RUN_TEST(test_get_percentage_low_current_low_percentage);
	RUN_TEST(test_get_percentage_low_current_high_percentage);
    UNITY_END();
}