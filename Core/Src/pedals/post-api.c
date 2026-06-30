#include "post-api.h"
#include "throttle-api.h"
#include "can-communications-api.h"
#include <stddef.h>

enum PostReturnCode post_api_init(struct PostInit *init) {
    if (init == NULL) {
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
