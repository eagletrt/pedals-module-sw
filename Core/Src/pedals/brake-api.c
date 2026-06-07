#include "brake-api.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct BrakeHandler brake_handler = {
	.get_pedal_travel = NULL,
	.get_front_pressure = NULL,
	.get_rear_pressure = NULL
};

/*!
 * \brief Utility function to truncate percentages
 * 
 * \param perc float to be checked
 * \retval BRAKE_RC_NO_ERROR the value wasn't truncated
 * \retval BRAKE_RC_VALUE_OUTSIDE_RANGE the value was truncated. No other return value possible
 */
enum BrakeReturnCode prv_brake_truncate_percentage(float* perc){
	enum BrakeReturnCode rc = BRAKE_RC_NO_ERROR;
	if(*perc > 1.0F){
		rc = BRAKE_RC_VALUE_OUTSIDE_RANGE;
		*perc = 1.0F;
	}
	else if(*perc < 0.0F){
		rc = BRAKE_RC_VALUE_OUTSIDE_RANGE;
		*perc = 0.0F;
	}
	return rc;
}

enum BrakeReturnCode brake_init_handler(
	brake_percentage_callback pedal_travel_cb, 
	brake_percentage_callback front_pressure_cb,
	brake_percentage_callback rear_pressure_cb
){
	if(pedal_travel_cb == NULL || front_pressure_cb == NULL || rear_pressure_cb == NULL){
		return BRAKE_RC_CALLBACK_FAILURE;
	}
	brake_handler.get_pedal_travel = pedal_travel_cb;
	brake_handler.get_front_pressure = front_pressure_cb;
	brake_handler.get_rear_pressure = rear_pressure_cb;
	return BRAKE_RC_NO_ERROR;
}

enum BrakeReturnCode brake_get_travel_percentage(float* travel) {
    if(brake_handler.get_pedal_travel == NULL){
		return BRAKE_RC_CALLBACK_FAILURE;
	}
	*travel = brake_handler.get_pedal_travel();
	return prv_brake_truncate_percentage(travel);
}


enum BrakeReturnCode brake_get_pressures(float* front, float* rear) {
    if(brake_handler.get_front_pressure == NULL || brake_handler.get_rear_pressure == NULL){
		return BRAKE_RC_CALLBACK_FAILURE;
	}
	*front = brake_handler.get_front_pressure();
	*rear = brake_handler.get_rear_pressure();

	enum BrakeReturnCode rc_front = prv_brake_truncate_percentage(front);
	enum BrakeReturnCode rc_rear = prv_brake_truncate_percentage(rear);
	enum BrakeReturnCode rc_combined = BRAKE_RC_NO_ERROR;
	if(rc_front != BRAKE_RC_NO_ERROR || rc_rear != BRAKE_RC_NO_ERROR){
		rc_combined = BRAKE_RC_VALUE_OUTSIDE_RANGE;
	}
	*front *= BRAKE_PRESSURE_CONVERSION_VALUE;
	*rear *= BRAKE_PRESSURE_CONVERSION_VALUE;
	return rc_combined;
}