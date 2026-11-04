#ifndef SCHEDULER_H
#define SCHEDULER_H

/*!
 * \brief Return codes for the scheduler module functions
 */
enum SchedulerReturnCode {
    SCHEDULER_RC_OK = 0,       /*!< The function executed successfully */
    SCHEDULER_RC_NULL_POINTER, /*!< An argument of the function was a null pointer */
    SCHEDULER_RC_ERROR,        /*!< The function encountered an error */
};

#endif // SCHEDULER_H
