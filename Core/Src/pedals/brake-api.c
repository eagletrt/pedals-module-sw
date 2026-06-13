#include "brake-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct BrakeHandler brake_handler = {
	.pedal_travel = (0.0F),
	.front_pressure = (0.0F),
	.rear_pressure = (0.0F)
};


void brake_update_pedal_travel_percentage(float percentage){
	if(percentage < 0.0F || percentage > 1.0F){
		percentage = -1.0F;
	}
	brake_handler.pedal_travel = percentage;
}

void brake_update_front_pressure(float pressure){
	brake_handler.front_pressure = pressure;
}

void brake_update_rear_pressure(float pressure){
	brake_handler.front_pressure = pressure;
}


float brake_get_pedal_travel_percentage(){
	return brake_handler.pedal_travel;
}

float brake_get_front_pressure(){
	return brake_handler.front_pressure;
}

float brake_get_rear_pressure(){
	return brake_handler.rear_pressure;
}