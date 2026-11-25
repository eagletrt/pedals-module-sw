#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef uint32_t (*scheduler_get_tick_callback)(void);

/*!
 * \brief Return codes for the scheduler module functions
 */
enum SchedulerReturnCode {
    SCHEDULER_RC_OK = 0,       /*!< The function executed successfully */
    SCHEDULER_RC_NULL_POINTER, /*!< An argument of the function was a null pointer */
    SCHEDULER_RC_ERROR,        /*!< The function encountered an error */
};

/*!
 * \brief Ids of the module tasks
 */
enum SchedulerTaskName {
    SCHEDULER_TASK_PEDALS_FSM_RUN,
    SCHEDULER_TASKS_COUNT,
};

#endif // SCHEDULER_H
