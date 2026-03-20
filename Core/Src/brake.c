#include "brake.h"

void send_brake_data(void){
	uint32_t scale = read_from_raw(ADC_CHANNEL_5V_SENSE);

	// contrary to throttle linear, brake pressure is not limited to a certain range
	float brake_linear = (float)read_from_raw(ADC_CHANNEL_BPPS) / scale;
	float brake_pressure_front = (float)read_from_raw(ADC_CHANNEL_BSPS_F) / scale; // conversions needed 
	float brake_pressure_rear = (float)read_from_raw(ADC_CHANNEL_BSPS_R) / scale;  // conversions needed
}