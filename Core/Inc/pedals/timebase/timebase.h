#ifndef PEDALS_TIMEBASE_H
#define PEDALS_TIMEBASE_H

#include <timebase/timebase.h>

/*! \brief Resolution of the board-wide timebase driven by TIM3. */
#define TIMEBASE_RESOLUTION_MS (1U)

/*! \brief Convert milliseconds to ticks of the board-wide timebase. */
#define TIMEBASE_CONVERT_MS_TO_TICKS(milliseconds) \
    TIMEBASE_MS_TO_TICKS((milliseconds), TIMEBASE_RESOLUTION_MS)

#endif // PEDALS_TIMEBASE_H
