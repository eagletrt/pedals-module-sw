#ifndef BRAKE_API_H
#define BRAKE_API_H

#include "brake.h"

/*!
 * \brief Function to call when ADC finished its conversion of the pedal travel percentage
 *
 * \param percentage it represents the percentage of the brake pedal travel. Value must be either in range [0,1] or -1 if original signal is out of range
 *
 * \details This function will enforce the restriction in values of the 'percentage' parameter, if not in range [0,1] the saved value will be -1
 */
void brake_api_update_pedal_travel_percentage(float percentage);

/*!
 * \brief Function to call when ADC finished its conversion of the front brake liquid pressure
 *
 * \param pressure it represents the value in bar [TENTATIVE UNIT OF MEASURE]. Value must be either positive > 0 or -1 if original signal is out of range
 *
 * \details This function cannot check if the value is out of range, always check parameter value before calling the function
 */
void brake_api_update_front_pressure(float pressure);

/*!
 * \brief Function to call when ADC finished its conversion of the rear brake liquid pressure
 *
 * \param pressure it represents the value in bar [TENTATIVE UNIT OF MEASURE]. Value must be either positive > 0 or -1 if original signal is out of range
 *
 * \details This function cannot check if the value is out of range, always check parameter value before calling the function
 */
void brake_api_update_rear_pressure(float pressure);

/*!
 * \brief Returns value of the last read brake pedal travel percentage
 *
 * \return float either in range [0,1] or -1 if the original signal is out of range
 */
float brake_api_get_pedal_travel_percentage();

/*!
 * \brief Returns value of the last read front brake pressure
 *
 * \return float positive > 0 or -1 if the original signal is out of range
 */
float brake_api_get_front_pressure();

/*!
 * \brief Returns value of the last read rear brake pressure
 *
 * \return float positive > 0 or -1 if the original signal is out of range
 */
float brake_api_get_rear_pressure();

/*!
 * \brief Send brake status (travel percentage, front and rear brake pressure) to CAN at a certain rate
 * 
 * \param tick current tick, used to check if it waited enough time before sending
 * \return enum BrakeReturnCode if it was able to send the message or not
 */
enum BrakeReturnCode brake_api_send_status(uint32_t tick);

#endif //BRAKE_API_H
