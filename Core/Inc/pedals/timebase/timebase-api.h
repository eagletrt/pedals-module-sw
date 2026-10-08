#ifndef PEDALS_TIMEBASE_API_H
#define PEDALS_TIMEBASE_API_H

#include "pedals/timebase/timebase.h"

#include <stdbool.h>
#include <stdint.h>

/*! \brief Initialize and enable the board-wide timebase. */
enum TimebaseReturnCode timebase_init(void);

/*! \brief Increment the board-wide timebase by one tick. Called by TIM3. */
enum TimebaseReturnCode timebase_tick(void);

/*! \brief Return the current board-wide tick count. */
uint32_t timebase_get_current_tick(void);

/*! \brief Return the elapsed board-wide time in milliseconds. */
uint32_t timebase_get_current_time(void);

/*! \brief Tell whether the board-wide timebase has been initialized. */
bool timebase_is_initialized(void);

#endif // PEDALS_TIMEBASE_API_H
