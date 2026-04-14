#include "unity.h"
#include "brake-api.h"
#include "adc_reading.h"

#include "test_adc_reading.h"
#include "stdint.h"

#include "fake_functions.h" // to avoid linking errors

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_test(void) {

}

void test_brake_bpps_percentage(void) {
	const int32_t MIN_VAL = BRAKE_BPPS_MIN_VALUE, MAX_VAL = BRAKE_BPPS_MAX_VALUE, CURRENT = 4095;
	int32_t sensor = MIN_VAL + (MAX_VAL - MIN_VAL) / 2;

	insert_values_in_array(CURRENT,0,0,sensor,0,0,0);

	float res = brake_get_travel_percentage();

	TEST_ASSERT_FLOAT_WITHIN(0.001,0.50f,res); // [49.9%,50.1%] range is valid
}

void test_brake_front_pressure() {
	const int32_t MIN_VAL = ADC_READING_MIN_RAW_VALUE, MAX_VAL = ADC_READING_MAX_RAW_VALUE, CURRENT = 4095;
	int32_t sensor = MIN_VAL + (MAX_VAL - MIN_VAL) / 2;

	insert_values_in_array(CURRENT,sensor,0,0,0,0,0);

	float res = brake_get_front_pressure();

	TEST_ASSERT_FLOAT_WITHIN(0.001 * BRAKE_BSPS_F_CONVERSION_VALUE, 0.50f * BRAKE_BSPS_F_CONVERSION_VALUE, res); // [49.9%,50.1%] range is valid
}

void test_brake_rear_pressure() {
	const int32_t MIN_VAL = ADC_READING_MIN_RAW_VALUE, MAX_VAL = ADC_READING_MAX_RAW_VALUE, CURRENT = 4095;
	int32_t sensor = MIN_VAL + (MAX_VAL - MIN_VAL) / 2;

	insert_values_in_array(CURRENT,0,sensor,0,0,0,0);

	float res = brake_get_rear_pressure();

	TEST_ASSERT_FLOAT_WITHIN(0.001 * BRAKE_BSPS_R_CONVERSION_VALUE, 0.50f * BRAKE_BSPS_R_CONVERSION_VALUE, res); // [49.85%,50.15%] range is valid
}

int main( int argc, char **argv) {
    UNITY_BEGIN();
	RUN_TEST(test_brake_bpps_percentage);
	RUN_TEST(test_brake_front_pressure);
	RUN_TEST(test_brake_rear_pressure);
    UNITY_END();
}