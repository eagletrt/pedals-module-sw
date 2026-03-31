#ifndef BOTS_API_H
#define BOTS_API_H

#include "stdbool.h"

/*!
 * \brief Function to activate when BOTS is triggered
 * 
 */
void bots_trigger_function(void);

/*!
 * \brief Get bool to see if BOTS was triggered and, consequently, send an error message. It consumes true values, see below
 * 
 * \return true if bots_trigger_function was called. After returning true, successive calls will return false until bots_trigger_function is called again
 * \return false if bots_trigger_function wasn't called or if it already returned true without bots_trigger_function being called again
 */
bool bots_is_bots_triggered(void);

#endif //BOTS_API_H