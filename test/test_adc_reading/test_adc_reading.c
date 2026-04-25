#include <unity.h>
#include "adc-reading-api.h"
#include "test_adc_reading.h"

#include "fake_functions.h" // to avoid linking errors

/*void setUp(void) {
    // set stuff up here
}*/

/*void tearDown(void) {
    // clean stuff up here
}*/

void test_return_percentage_full_current_valid_percentage(void){
	const int32_t MIN_VAL = 500, MAX_VAL = 2500, CURRENT = ADC_READING_MAX_RAW_VALUE;
	int32_t sensor = 1500;

	insert_values_in_array(CURRENT,0,0,sensor,0,0,0);

	float res = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_EQUAL_FLOAT(0.50f, res);
}

void test_return_percentage_full_current_low_percentage(void){
	const int32_t MIN_VAL = 500, MAX_VAL = 2500, CURRENT = ADC_READING_MAX_RAW_VALUE;
	int32_t sensor = 499;

	insert_values_in_array(CURRENT,0,0,sensor,0,0,0);

	float res = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_LESS_THAN_FLOAT(0.0f, res);
}

void test_return_percentage_full_current_high_percentage(void){
	const int32_t MIN_VAL = 500, MAX_VAL = 2500, CURRENT = ADC_READING_MAX_RAW_VALUE;
	int32_t sensor = 2501;

	insert_values_in_array(CURRENT,0,0,sensor,0,0,0);

	float res = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_GREATER_THAN_FLOAT(1.0f,res);
}

void test_return_percentage_low_current_valid_percentage(void){
	const int32_t MIN_VAL = 500, MAX_VAL = 2500, CURRENT = 3890; //95% of full current, approx is 94.9%
	int32_t sensor = 1425;

	insert_values_in_array(CURRENT,0,0,sensor,0,0,0);

	float res = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_FLOAT_WITHIN(0.001,0.50f,res); // [49.9%,50.1%] range is valid
}

void test_return_percentage_low_current_low_percentage(void){
	const int32_t MIN_VAL = 500, MAX_VAL = 2500, CURRENT = 3890; //95% of full current
	int32_t sensor = 474; //94.9% of 500 is 474.9 

	insert_values_in_array(CURRENT,0,0,sensor,0,0,0);
	float res = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_LESS_THAN_FLOAT(0.0f,res);
}

void test_return_percentage_low_current_high_percentage(void){
	const int32_t MIN_VAL = 500, MAX_VAL = 2500, CURRENT = 3890; //95% of full current
	int32_t sensor = 2375; // 94.9% of 2500

	insert_values_in_array(CURRENT,0,0,sensor,0,0,0);
	float res = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_BPPS,MIN_VAL,MAX_VAL);

	TEST_ASSERT_GREATER_THAN_FLOAT(1.0f,res);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
	RUN_TEST(test_return_percentage_full_current_valid_percentage);
	RUN_TEST(test_return_percentage_full_current_low_percentage);
	RUN_TEST(test_return_percentage_full_current_high_percentage);
	RUN_TEST(test_return_percentage_low_current_valid_percentage);
	RUN_TEST(test_return_percentage_low_current_low_percentage);
	RUN_TEST(test_return_percentage_low_current_high_percentage);
    UNITY_END();
}