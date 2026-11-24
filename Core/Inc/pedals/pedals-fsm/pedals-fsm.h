/*!
 * \file pedals-fsm.h
 * \author Matteo Grande [mattwexx2929@gmail.com]
 * \date 2026-11-19
 * \brief Types definitions for the pedals fms module.
 */

#ifndef PEDALS_FSM_H
#define PEDALS_FSM_H

#include <stdint.h>

/*!
 * \brief Minimum interval, in milliseconds, between two module updates.
 */
#define FSM_MODULES_UPDATE_PERIOD_MS (3)

/*! \brief Identifiers of the possible FSM states. */
enum FSMStateID {
    FSM_STATE_INIT = 0, /*!< Init State id: entry point of the fsm */
    FSM_STATE_IDLE,     /*!< Idle state id */
    FSM_STATE_FLASH,    /*!< Flash state id */
    FSM_STATE_ERROR,    /*!< Error state id */
    FSM_STATE_COUNT,    /*!< Number of states */
};

/*!
 * \brief Callback used to read the current tick, in milliseconds.
 *
 * \return The current tick.
 */
typedef uint32_t (*fsm_get_tick_callback)(void);

/*!
 * \brief Callback used to write a string on the serial port.
 *
 * \param[in] str The NUL-terminated string to write.
 */
typedef void (*fsm_serial_write_callback)(const char *str);

/*! \brief Callback used to update the application modules managed by the FSM. */
typedef void (*fsm_update_module_callback)(void);

/*! \brief Callback used to reset the MCU. */
typedef void (*fsm_system_reset_callback)(void);

#endif //PEDALS_FSM_H
