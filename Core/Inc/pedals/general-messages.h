#ifndef GENERAL_MESSAGES_H
#define GENERAL_MESSAGES_H

#define GENERAL_MESSAGES_STATUS_CAN_PERIOD_MS (500)
#define GENERAL_MESSAGES_VERSION_CAN_PERIOD_MS (1000)

/*!
 * \brief Operation status for the module's functions
 * 
 */
enum GeneralMessagesReturnCode {
    GENERAL_MESSAGES_RC_OK,   /*!< No error was found*/
    GENERAL_MESSAGES_RC_ERROR /*!< Message could not be sent*/
};

#endif //GENERAL_MESSAGES_H
