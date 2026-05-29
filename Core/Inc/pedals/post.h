#ifndef POST_H
#define POST_H

#include "voltage-scaling.h"
#include "throttle.h"


/*!
 * \brief Return code for this module's operations
 * 
 */
enum PostReturnCode{
	POST_RC_OK,		/*!< Operation success*/
	POST_RC_ERROR	/*!< Either the initialisation subroutines failed or some field was NULL*/
};

/*!
 * \brief Structure with pointers to external functions, used for initialisation
 * 
 */
struct PostInit {
	voltage_scaling_sensor_read_callback read_raw_values;	/*!< External function to read sensors' raw values*/
	throttle_timer_callback start_timer;					/*!< External function to start implausibility timer*/
	throttle_timer_callback stop_timer;						/*!< External function to stop implausibility timer*/
};

#endif /* POST_H */