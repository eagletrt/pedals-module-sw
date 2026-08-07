#ifndef IDENTITY_H
#define IDENTITY_H

#include <stdint.h>

#define IDENTITY_STATUS_CAN_PERIOD_MS (500)
#define IDENTITY_VERSION_CAN_PERIOD_MS (1000)

/*!
 * \brief Operation status for the module's functions
 * 
 */
enum IdentityReturnCode {
    IDENTITY_RC_OK,   /*!< No error was found*/
    IDENTITY_RC_ERROR /*!< Message could not be sent*/
};

struct IdentityHandler {
    uint32_t last_version_tick;             /*!< Last tick in which a message for pedals version was sent*/
    uint32_t last_version_info_tick;        /*!< Last tick in which a message for pedals version info was sent*/
    uint32_t last_status_tick;              /*!< Last tick in which a message for pedals status was sent*/
    uint32_t last_libcan_version_tick;      /*!< Last tick in which a message for libcan version was sent*/
    uint32_t last_libcan_version_info_tick; /*!< Last tick in which a message for libcan version info was sent*/
};

#endif //IDENTITY_H
