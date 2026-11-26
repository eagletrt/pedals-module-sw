#ifndef BRAKE_H
#define BRAKE_H

#include <stdint.h>

typedef void(*brake_update_hook_callback)(void);

enum BrakeReturnCode {
    BRAKE_RC_OK,
    BRAKE_RC_NULL_POINTER,
    BRAKE_RC_ERROR,
};

/*!
 * \brief Structure to collect all the newest read values from the sensors
 */
struct BrakeHandler {
    float pedal_travel;   /*!< Newest pedal brake travel percentage */
    float front_pressure; /*!< Newest front brake pressure value */
    float rear_pressure;  /*!< Newest rear brake pressure value */
    brake_update_hook_callback update_hook;
};

#endif //BRAKE_H
