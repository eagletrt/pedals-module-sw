#ifndef POST_H
#define POST_H

#include "voltage-scaling.h"
#include "throttle.h"

enum PostReturnCode{
	POST_RC_OK,
	POST_RC_ERROR
};

struct PostInit {
	voltage_scaling_sensor_read_callback read_raw_values;
	throttle_timer_callback start_timer;
	throttle_timer_callback stop_timer;
};

#endif /* POST_H */