#ifndef PEDALS_FSM_H
#define PEDALS_FSM_H

#include "eagletrt-fsm.h"

#define FSM_MODULES_UPDATE_PERIOD_MS (3)

/*! \brief The ids of the possible fsm state */
enum FSMStateID {
    FSM_STATE_INIT = 0,
    FSM_STATE_IDLE,
    FSM_STATE_FLASH,
    FSM_STATE_ERROR,
    FSM_STATE_COUNT,
};

typedef uint32_t (*fsm_get_tick_callback)(void);
typedef void (*fsm_serial_write_callback)(const char *str);
typedef void (*fsm_update_module_callback)(void);
typedef void (*fsm_system_reset_callback)(void);

// TODO: not actually needed (need to rework the modules)
struct FsmData {
    fsm_get_tick_callback get_tick;
    fsm_update_module_callback update_module;
    fsm_system_reset_callback system_reset; /*!< Resets the MCU, used in STATE_FLASH to enter the bootloader. */
};

#endif //PEDALS_FSM_H
