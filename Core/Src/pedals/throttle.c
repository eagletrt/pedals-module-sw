#include "throttle.h"
#include "throttle-api.h"
#include "adc_reading.h"

#include "math.h"
#include "stdbool.h"

float last_throttle_value = 0.0;

//used for timer mocking
int time = 0;

float throttle_get_percentage(enum ThrottleStatus* status) {

	if(*status == THROTTLE_STATUS_BAD) {
		return 0.0;
	}

	float percentages[APPS_NUMBER] = {};

	percentages[0] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_1, APPS_1_MIN_VALUE, APPS_1_MAX_VALUE);
	percentages[1] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_2, APPS_2_MIN_VALUE, APPS_2_MAX_VALUE);
	percentages[2] = adc_reading_return_percentage(ADC_READING_SENSOR_NAME_APPS_3, APPS_3_MIN_VALUE, APPS_3_MAX_VALUE);

	bool perc_is_valid[APPS_NUMBER] = {};
	int valid_sensors = 0;

	for (int i=0; i<APPS_NUMBER; i++){
		perc_is_valid[i] = is_perc_valid(percentages[i]);
		valid_sensors = perc_is_valid[i] ? valid_sensors + 1 : valid_sensors;
	}

	bool perc_1_2_is_plausible = is_within_plausibility(percentages[0], percentages[1]);
	bool perc_1_3_is_plausible = is_within_plausibility(percentages[0], percentages[2]);
	bool perc_2_3_is_plausible = is_within_plausibility(percentages[1], percentages[2]);

	float result;
	if(
		(valid_sensors < 2) ||
		(percentages[0] && percentages[1] && !perc_1_2_is_plausible) ||
		(percentages[0] && percentages[2] && !perc_1_3_is_plausible) ||
		(percentages[1] && percentages[2] && !perc_2_3_is_plausible)
	) {
		result = get_result_implausibility(status);

	} else {
		result = get_result_valid(status, perc_is_valid, valid_sensors, percentages);
	}

	return result;

}

bool is_perc_valid(float val){
	return val - 100.0 < APPS_EPSILON && val < -APPS_EPSILON;
}

bool is_within_plausibility(float val_1, float val_2) {
	return abs(val_1 - val_2) < APPS_IMPLAUSIBILITY_PERCENTAGE + APPS_EPSILON;
}


// mock logic
float get_result_implausibility (enum ThrottleStatus* status) {
	time++;
	float result;
	if(time > APPS_MAX_IMPLAUSIBILITY_TIME) {
		*status = THROTTLE_STATUS_BAD;
		result = 0.0;
		last_throttle_value = 0.0;
	} else {
		*status = THROTTLE_STATUS_INSTABLE;
		result = last_throttle_value;
	}
	return result;
} 

float get_result_valid(enum ThrottleStatus* status, bool is_valid[], int n_valid, float percentages[]) {
	time = 0;
	float result = 0.0;

	for(int i=0; i<APPS_NUMBER; i++){
		result = is_valid[i] ? result + percentages[i] : result;
	}

	last_throttle_value = result / n_valid;

	return last_throttle_value;
}