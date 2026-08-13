#ifndef BOTS_API_H
#define BOTS_API_H

#include "stdbool.h"

/*!
 * \brief Set the voltage value for BOTS. If the voltage is below a certain threshold, it will trigger an error message
 *
 * \param voltage The voltage value to set for BOTS
 */
void bots_set_voltage(float voltage);

/*!
 * \brief Get bool to see if BOTS was triggered and, consequently, send an error message. It consumes true values, see below
 * 
 * \retval true if bots_set_triggered was called. After returning true, successive calls will return false until bots_set_triggered is called again
 * \retval false if bots_set_triggered wasn't called or if it already returned true without bots_set_triggered being called again
 */
bool bots_is_triggered(void);

#endif //BOTS_API_H
