#include "identity-api.h"
#include "can-primary-api.h"
#include "can-communications-api.h"
#include "pedals-fsm-api.h"

void identity_api_send_pedals_version_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    constexpr uint16_t major_version = 0;
    constexpr uint16_t minor_version = 1;
    constexpr uint16_t patch_version = 0;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSION,
        .length = can_primary_byte_size_pedalsversion,
    };
    union CanPrimaryMessages status_msg = {
        .pedalsversion.major = major_version,
        .pedalsversion.minor = minor_version,
        .pedalsversion.patch = patch_version,
    };

    EAGLETRT_API_UNUSED(
        can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSION,
                                          &status_msg,
                                          frame.data));
    can_communications_api_add_to_tx_buffer(&frame);
}

void identity_api_send_pedals_version_info_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    constexpr uint32_t tmp_buildtime = 0;
    constexpr uint32_t tmp_commithash = 0;
    constexpr bool tmp_dirty = false;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSIONINFO,
        .length = can_primary_byte_size_pedalsversioninfo,
    };
    union CanPrimaryMessages status_msg = {
        .pedalsversioninfo.buildtime = tmp_buildtime,
        .pedalsversioninfo.commithash = tmp_commithash,
        .pedalsversioninfo.dirty = (uint8_t)tmp_dirty,
    };

    EAGLETRT_API_UNUSED(
        can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSIONINFO,
                                          &status_msg,
                                          frame.data));
    can_communications_api_add_to_tx_buffer(&frame);
}

void identity_api_send_pedals_fsm_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSFSM,
        .length = can_primary_byte_size_pedalsfsm
    };

    enum CanPrimaryPedalsfsmStatus can_pedals_fsm_status;
    switch (pedals_fsm_api_get_state()) {
        case FSM_STATE_INIT: {
            can_pedals_fsm_status = CAN_PRIMARY_PEDALSFSM_STATUS_INIT;
            break;
        }
        case FSM_STATE_IDLE: {
            can_pedals_fsm_status = CAN_PRIMARY_PEDALSFSM_STATUS_IDLE;
            break;
        }
        case FSM_STATE_FLASH: {
            can_pedals_fsm_status = CAN_PRIMARY_PEDALSFSM_STATUS_FLASH;
            break;
        }
        case FSM_STATE_ERROR:
        default:
            can_pedals_fsm_status = CAN_PRIMARY_PEDALSFSM_STATUS_ERROR;
    }
    union CanPrimaryMessages status_msg = {
        .pedalsfsm.status = can_pedals_fsm_status,
    };

    //logger_api_log(LOGGER_LEVEL_DEBUG, "Identity: Sending Pedals FSM: %d", fsm_state);

    EAGLETRT_API_UNUSED(
        can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSFSM,
                                          &status_msg,
                                          frame.data));
    can_communications_api_add_to_tx_buffer(&frame);
}

void identity_api_send_libcan_version_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSION,
        .length = can_primary_byte_size_pedalslibcanversion,
    };
    union CanPrimaryMessages status_msg = {
        .pedalslibcanversion.major = can_version_major,
        .pedalslibcanversion.minor = can_version_minor,
        .pedalslibcanversion.patch = can_version_patch,
    };

    EAGLETRT_API_UNUSED(
        can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSION,
                                          &status_msg,
                                          frame.data));
    can_communications_api_add_to_tx_buffer(&frame);
}

void identity_api_send_libcan_version_info_task(uint8_t task_id) {
    EAGLETRT_API_UNUSED(task_id);

    constexpr uint32_t tmp_commithash = 0;
    constexpr bool tmp_dirty = 0;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSIONINFO,
        .length = can_primary_byte_size_pedalslibcanversioninfo,
    };
    union CanPrimaryMessages status_msg = {
        .pedalslibcanversioninfo.generationtime = can_generation_time,
        .pedalslibcanversioninfo.commithash = tmp_commithash,
        .pedalslibcanversioninfo.dirty = (uint8_t)tmp_dirty,
    };

    EAGLETRT_API_UNUSED(
        can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSIONINFO,
                                          &status_msg,
                                          frame.data));
    can_communications_api_add_to_tx_buffer(&frame);
}
