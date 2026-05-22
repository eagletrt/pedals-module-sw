#ifndef VOLTAGE_SCALING_H
#define VOLTAGE_SCALING_H

#include "stdint.h"
#include "sensor-types.h"

#define VOLTAGE_SCALING_MIN_RAW_VALUE (0.0F)
#define VOLTAGE_SCALING_MAX_RAW_VALUE (4095.0F)


enum VoltageScalingReturnCode {
	VOLTAGE_SCALING_RC_OK,
	VOLTAGE_SCALING_RC_ERROR
};

typedef int32_t (*voltage_scaling_sensor_read_callback)(
	enum SensorTypesName sensor_type);

struct VoltageScalingHandler {
	voltage_scaling_sensor_read_callback read_raw;
};

#endif /* VOLTAGE_SCALING_H */