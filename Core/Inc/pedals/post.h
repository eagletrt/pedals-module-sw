#ifndef POST_H
#define POST_H

#include "throttle.h"
#include "can-communications.h"

enum PostReturnCode {
    POST_RC_OK,
    POST_RC_ERROR
};

struct PostInit {
    throttle_timer_callback start_timer;
    throttle_timer_callback stop_timer;

    struct CanCommunicationsNetworkConfig config;
};

#endif //POST_H
