#include "post-api.h"
#include "throttle-api.h"
#include "can-communications-api.h"
#include "bootloader-api.h"
#include "pedals/timebase/timebase-api.h"
#include "watchdogs-api.h"
#include "scheduler-api.h"

enum PostReturnCode post_api_init(struct PostInit *init) {
    if (init == nullptr) {
        return POST_RC_ERROR;
    }

    if (timebase_init() != TIMEBASE_RC_OK) {
        return POST_RC_ERROR;
    }

    if (scheduler_api_init(init->get_tick) != SCHEDULER_RC_OK) {
        return POST_RC_ERROR;
    }

    if (bootloader_api_init(init->reboot) != BOOTLOADER_RC_OK) {
        return POST_RC_ERROR;
    }

    if (throttle_api_init(init->start_timer, init->stop_timer) != THROTTLE_RC_OK) {
        return POST_RC_ERROR;
    }

    if (can_communications_api_init(init->config) != CAN_COMMUNICATION_RC_OK) {
        return POST_RC_ERROR;
    }

    return POST_RC_OK;
}
