#ifndef BRAKE_H
#define BRAKE_H

/*!
 * \brief Structure to collect all the newest read values from the sensors
 */
struct BrakeHandler {
    float pedal_travel;   /*!< Newest pedal brake travel percentage */
    float front_pressure; /*!< Newest front brake pressure value */
    float rear_pressure;  /*!< Newest rear brake pressure value */
};

#endif //BRAKE_H
