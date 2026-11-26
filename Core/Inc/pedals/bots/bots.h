#ifndef BOTS_H
#define BOTS_H

/*! \brief */
constexpr float bots_threshold_voltage_triggered = 0.5F;

/*! \brief */
typedef void (*bots_update_hook_callback)(void);

/*! \brief */
enum BotsReturnCode {
    BOTS_RC_OK = 0,
    BOTS_RC_NULL_POINTER,
    BOTS_RC_ERROR,
};

/*! \brief */
struct BotsHandler {
    float voltage;
    bots_update_hook_callback update_hook;
};

#endif // BOTS_H
