#include "throttle.h"

uint32_t MIN_APPS_1 = 0; // to be defined
uint32_t MAX_APPS_1 = 0; // we assume for now that min and max values are fixed
uint32_t MIN_APPS_2 = 0;
uint32_t MAX_APPS_2 = 0;
uint32_t MIN_APPS_3 = 0;
uint32_t MAX_APPS_3 = 0;

void send_throttle_data(void){
	uint32_t scale = read_from_raw(ADC_CHANNEL_5V_SENSE);

	uint32_t raw_apps = read_from_raw(ADC_CHANNEL_APPS_1);
	uint32_t temp_min = raw_apps >= MIN_APPS_1 ? raw_apps - MIN_APPS_1 : 0;
	uint32_t temp_max;

	

	float throttle_1 = (float) / scale; // conversions needed
	float throttle_2 = (float)read_from_raw(ADC_CHANNEL_APPS_2) / scale; // conversions needed
	float throttle_3 = (float)read_from_raw(ADC_CHANNEL_APPS_3) / scale; // conversions needed

	float throttle_linear = (float)read_from_raw(ADC_CHANNEL_APPS_1) / scale; // conversions needed
}