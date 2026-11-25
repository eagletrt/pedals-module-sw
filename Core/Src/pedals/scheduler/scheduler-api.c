#include "scheduler-api.h"

#include "eagletrt.h"
#include "pedals-fsm-api.h"
#include "watchdogs-api.h"
#include "tasks-api.h"

#include <string.h>

EAGLETRT_STATIC struct WatchdogHandler watchdogs_handler;
EAGLETRT_STATIC struct TasksHandler tasks_handler;

EAGLETRT_STATIC TaskList tasks = {
    {
     .task_id = SCHEDULER_TASK_PEDALS_FSM_RUN,
     .state = TASKS_STATE_ENABLED,
     .function = pedals_fsm_run_task,
     .start = 0U,
     .interval = 5U,
     .repeats = 0,
     },
};

enum SchedulerReturnCode scheduler_api_init(scheduler_get_tick_callback get_tick) {
    if (get_tick == nullptr) {
        return SCHEDULER_RC_NULL_POINTER;
    }

    memset(&watchdogs_handler, 0, sizeof(watchdogs_handler));

    if (watchdogs_api_init_pool(&watchdogs_handler, get_tick) != WATCHDOG_RC_OK) {
        return SCHEDULER_RC_ERROR;
    }

    if (tasks_api_init(&tasks_handler, tasks, SCHEDULER_TASKS_COUNT, get_tick)) {
    }

    return SCHEDULER_RC_OK;
}

enum WatchdogReturnCode scheduler_api_watchdogs_routine(void) {
    return watchdogs_api_routine(&watchdogs_handler);
}

enum WatchdogReturnCode scheduler_api_watchdog_start(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_start(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_restart(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_restart(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_stop(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_stop(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_reset(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_reset(&watchdogs_handler, watchdog);
}

enum WatchdogReturnCode scheduler_api_watchdog_pet(struct Watchdog *watchdog) {
    return watchdogs_api_watchdog_pet(&watchdogs_handler, watchdog);
}

enum TasksReturnCode scheduler_api_tasks_routine(void) {
    return tasks_api_routine(&tasks_handler);
}

enum TasksReturnCode scheduler_api_enable_task(uint8_t task_id) {
    return tasks_api_enable_task(&tasks_handler, task_id);
}

enum TasksReturnCode scheduler_api_pause_task(uint8_t task_id) {
    return tasks_api_pause_task(&tasks_handler, task_id);
}

enum TasksReturnCode scheduler_api_disable_task(uint8_t task_id) {
    return tasks_api_disable_task(&tasks_handler, task_id);
}

enum TasksReturnCode scheduler_api_get_task(uint8_t task_id, struct Task *task) {
    return tasks_api_get_task(&tasks_handler, task_id, task);
}

enum TasksReturnCode scheduler_api_update_task(uint8_t task_id, uint16_t new_interval, uint16_t new_start, uint8_t repeats) {
    return tasks_api_update_task(&tasks_handler, task_id, new_interval, new_start, repeats);
}

uint16_t scheduler_api_get_dropped_task_executions(void) {
    return tasks_api_get_dropped_task_executions(&tasks_handler);
}
