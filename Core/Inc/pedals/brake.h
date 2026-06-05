#ifndef BRAKE_H
#define BRAKE_H

//TODO use correct values
#define BRAKE_BPPS_MIN_VALUE (0.0F)
#define BRAKE_BPPS_MAX_VALUE (1000.0F)

//TODO use correct values
#define BRAKE_BSPS_F_CONVERSION_VALUE (30.0F)
#define BRAKE_BSPS_R_CONVERSION_VALUE (40.0F)

/*!
 * \brief Return code for brake's operations
 * 
 */
enum BrakeReturnCode{
	BRAKE_RC_NO_ERROR,				/*!< Operation success*/
	BRAKE_RC_VALUE_OUTSIDE_RANGE,	/*!< The actual value was outside the established range and so the results had been clipped*/
	BRAKE_RC_CALLBACK_FAILURE		/*!< External function call failed, not possible to retrieve the true value*/
};

#endif //BRAKE_H