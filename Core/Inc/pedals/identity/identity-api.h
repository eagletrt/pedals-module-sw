#ifndef IDENTITY_API_H
#define IDENTITY_API_H

#include "identity.h"

/*! \brief Sends pedal's version to CAN */
void identity_api_send_pedals_version_task(uint8_t task_id);

/*! \brief Sends pedal's version info to CAN */
void identity_api_send_pedals_version_info_task(uint8_t task_id);

/*! \brief Sends pedal's FSM status to CAN  */
void identity_api_send_pedals_fsm_task(uint8_t task_id);

/*! \brief Sends libcan's version to CAN */
void identity_api_send_libcan_version_task(uint8_t task_id);

/*! \brief Sends libcan's version info to CAN at a certain rate */
void identity_api_send_libcan_version_info_task(uint8_t task_id);

#endif //IDENTITY_API_H
