/*!
 * \file pedals-fsm-api.h
 * \author Matteo Grande [mattwexx2929@gmail.com]
 * \date 2026-11-19
 * \brief Public API for the module pedals fsm module
 */

#ifndef PEDALS_FSM_API_H
#define PEDALS_FSM_API_H

#include "pedals-fsm.h"
#include "eagletrt-fsm.h"

/*!
 * \brief Initialize the module handler
 *
 * \retval FSM_RC_OK Initialization successful
 * \retval FSM_RC_INVALID_MACHINE The FSM structure is invalid
 * \retval FSM_RC_INVALID_STATE The given state is invalid
 * \retval FSM_RC_NULL_POINTER The function recieved a null pointer
 */
enum FSMReturnCode pedals_fsm_api_init(void);

/*!
 * \brief Routine for the FSM
 *
 * \param[in] data The data to be passed to the state
 *            and to the transition function
 *
 * \retval FSM_RC_OK The routine completed succesfully
 * \retval FSM_RC_INVALID_TRANSITION The module attempted an invalid transition
 * \retval FSM_RC_NULL_POINTER The function recieved a null pointer
 */
enum FSMReturnCode pedals_fsm_api_run_state(void *data);

/*!
 * \brief Function to trigger a change of state in the FSM
 * \details This will take place at the end of the current state
 *          execution, if the transition is valid
 *
 * \param[in] enum FSMStateID The ID of the state to transition to
 *
 * \retval FSM_RC_OK The next state has been updated successfully
 * \retval FSM_RC_INVALID_TRANSITION The asked transition is not supported
 * \retval FSM_RC_INVALID_STATE The given state is invalid
 * \retval FSM_RC_NULL_POINTER The function recieved a null pointer
 */
enum FSMReturnCode pedals_fsm_api_transition_to(enum FSMStateID state_id);

/*!
 * \brief Function that returns the ID of the current state
 *
 * \returns enum FSMStateID The ID of the current state,
 *                          0 also if the handler is NULL
 */
enum FSMStateID pedals_fsm_api_get_state(void);

#endif //PEDALS_FSM_API_H
