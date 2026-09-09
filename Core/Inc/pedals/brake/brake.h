#ifndef BRAKE_H
#define BRAKE_H

#include <stdint.h>

enum BrakeReturnCode {
    BRAKE_RC_OK,
    BRAKE_RC_ERROR,
};

/*!
 * \brief Structure to collect all the newest read values from the sensors
 */
struct BrakeHandler {
    float pedal_travel;   /*!< Newest pedal brake travel percentage */
    float front_pressure; /*!< Newest front brake pressure value */
    float rear_pressure;  /*!< Newest rear brake pressure value */
    uint32_t last_tick;   /*!< Last tick in which a message for brake was sent*/
};

#endif //BRAKE_H
