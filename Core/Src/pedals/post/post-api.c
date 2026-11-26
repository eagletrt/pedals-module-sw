#include "post-api.h"
#include "bots-api.h"
#include "brake-api.h"
#include "throttle-api.h"
#include "can-communications-api.h"
#include "bootloader-api.h"
#include "watchdogs-api.h"

enum PostReturnCode post_api_init(struct PostInit *init) {
    if (init == nullptr) {
        return POST_RC_ERROR;
    }

    if (bootloader_api_init(init->reboot) != BOOTLOADER_RC_OK) {
        return POST_RC_ERROR;
    }

    if (throttle_api_init(init->start_timer,
                          init->stop_timer,
                          init->throttle_update_hook) != THROTTLE_RC_OK) {
        return POST_RC_ERROR;
    }

    if (brake_api_init(init->brake_update_hook) != BRAKE_RC_OK) {
        return POST_RC_ERROR;
    }

    if (bots_api_init(init->bots_update_hook) != BOTS_RC_OK) {
        return POST_RC_ERROR;
    }

    if (can_communications_api_init(init->config) != CAN_COMMUNICATION_RC_OK) {
        return POST_RC_ERROR;
    }

    return POST_RC_OK;
}
