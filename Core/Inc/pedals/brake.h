#ifndef BRAKE_H
#define BRAKE_H

#define BRAKE_PRESSURE_CONVERSION_VALUE 30.0F // to be checked

/*!
 * \brief Return code for brake's operations
 * 
 */
enum BrakeReturnCode{
	BRAKE_RC_NO_ERROR,				/*!< Operation success*/
	BRAKE_RC_VALUE_OUTSIDE_RANGE,	/*!< The actual value was outside the established range and so the results had been truncated*/
	BRAKE_RC_CALLBACK_FAILURE		/*!< External function call failed, not possible to retrieve the true value*/
};

/*!
 * \brief Callback type for external function to get percentage from ADC 
 * 
 * \return float the percentage of the sensor voltage w.r.t. to the micro's input voltage 
 * 
 */
typedef float (*brake_percentage_callback)();

/*!
 * \brief structure to collect all the external function of the module
 * 
 */
struct BrakeHandler{
	brake_percentage_callback get_pedal_travel;		/*!< Percentage of bpps - brake pedal travel*/
	brake_percentage_callback get_front_pressure;	/*!< Percentage of bsps_f - front brake pressure*/
	brake_percentage_callback get_rear_pressure;	/*!< Percentage of bsps_r - rear brake pressure*/
};

#endif //BRAKE_H