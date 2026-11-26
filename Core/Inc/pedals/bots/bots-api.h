#ifndef BOTS_API_H
#define BOTS_API_H

#include "bots.h"

#include "stdint.h"

/*! \brief */
enum BotsReturnCode bots_api_init(bots_update_hook_callback update_hook);

/*!
 * \brief Set the voltage value for BOTS. If the voltage is below a certain threshold, it will trigger an error message
 *
 * \param voltage The voltage value to set for BOTS
 */
void bots_api_set_voltage(float voltage);

/*!
 * \brief Get the current voltage value for BOTS
 *
 * \return The current voltage value for BOTS
 */
float bots_api_get_voltage(void);

/*!
 * \brief Get bool to see if BOTS was triggered and, consequently, send an error message. It consumes true values, see below
 *
 * \retval true if bots_set_triggered was called. After returning true, successive calls will return false until bots_set_triggered is called again
 * \retval false if bots_set_triggered wasn't called or if it already returned true without bots_set_triggered being called again
 */
bool bots_api_is_triggered(void);

/*! \brief */
void bots_api_routine_task(uint8_t task_id);

#endif //BOTS_API_H
