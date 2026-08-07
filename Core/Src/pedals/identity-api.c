#include "identity-api.h"
#include "can-primary-api.h"
#include "can-communications-api.h"

EAGLETRT_STATIC struct IdentityHandler identity_handler = {
    .last_version_tick = 0,
    .last_status_tick = 0
};

enum IdentityReturnCode identity_api_send_pedals_version(uint32_t tick) {
    constexpr uint16_t major_version = 0;
    constexpr uint16_t minor_version = 1;
    constexpr uint16_t patch_version = 0;

    if (tick - identity_handler.last_version_tick < can_primary_cycle_time_pedalsversion) {
        return IDENTITY_RC_OK;
    }
    identity_handler.last_version_tick = tick;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSION,
        .length = can_primary_byte_size_pedalsversion,
    };
    union CanPrimaryMessages status_msg = {
        .pedalsversion.major = major_version,
        .pedalsversion.minor = minor_version,
        .pedalsversion.patch = patch_version,
    };
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSION, &status_msg, frame.data) == -1) {
        return IDENTITY_RC_ERROR;
    }
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return IDENTITY_RC_ERROR;
    }
    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_send_pedals_version_info(uint32_t tick) {
    constexpr uint32_t tmp_buildtime = 0;
    constexpr uint32_t tmp_commithash = 0;
    constexpr bool tmp_dirty = 0;

    if (tick - identity_handler.last_version_info_tick < can_primary_cycle_time_pedalsversioninfo) {
        return IDENTITY_RC_OK;
    }
    identity_handler.last_version_info_tick = tick;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSIONINFO,
        .length = can_primary_byte_size_pedalsversioninfo,
    };
    union CanPrimaryMessages status_msg = {
        .pedalsversioninfo.buildtime = tmp_buildtime,
        .pedalsversioninfo.commithash = tmp_commithash,
        .pedalsversioninfo.dirty = tmp_dirty,
    };
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSVERSIONINFO, &status_msg, frame.data) == -1) {
        return IDENTITY_RC_ERROR;
    }
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return IDENTITY_RC_ERROR;
    }
    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_send_pedals_fsm(uint32_t tick, state_t fsm_state) {
    if (tick - identity_handler.last_status_tick < can_primary_cycle_time_pedalsfsm) {
        return IDENTITY_RC_OK;
    }
    identity_handler.last_status_tick = tick;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSFSM,
        .length = can_primary_byte_size_pedalsfsm
    };
    union CanPrimaryMessages status_msg = {
        .pedalsfsm.status = fsm_state,
    };
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSFSM, &status_msg, frame.data) == -1) {
        return IDENTITY_RC_ERROR;
    }
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return IDENTITY_RC_ERROR;
    }
    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_send_libcan_version(uint32_t tick) {
    if (tick - identity_handler.last_libcan_version_tick < can_primary_cycle_time_pedalslibcanversion) {
        return IDENTITY_RC_OK;
    }
    identity_handler.last_libcan_version_tick = tick;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSION,
        .length = can_primary_byte_size_pedalslibcanversion,
    };
    union CanPrimaryMessages status_msg = {
        .pedalslibcanversion.major = can_version_major,
        .pedalslibcanversion.minor = can_version_minor,
        .pedalslibcanversion.patch = can_version_patch,
    };
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSION, &status_msg, frame.data) == -1) {
        return IDENTITY_RC_ERROR;
    }
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return IDENTITY_RC_ERROR;
    }
    return IDENTITY_RC_OK;
}

enum IdentityReturnCode identity_api_send_libcan_version_info(uint32_t tick) {
    constexpr uint32_t tmp_commithash = 0;
    constexpr bool tmp_dirty = 0;

    if (tick - identity_handler.last_libcan_version_info_tick < can_primary_cycle_time_pedalslibcanversioninfo) {
        return IDENTITY_RC_OK;
    }
    identity_handler.last_libcan_version_info_tick = tick;

    struct CanCommunicationFrame frame = {
        .id = CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSIONINFO,
        .length = can_primary_byte_size_pedalslibcanversioninfo,
    };
    union CanPrimaryMessages status_msg = {
        .pedalslibcanversioninfo.generationtime = can_generation_time,
        .pedalslibcanversioninfo.commithash = tmp_commithash,
        .pedalslibcanversioninfo.dirty = tmp_dirty,
    };
    if (can_primary_api_serialize_from_id(CAN_PRIMARY_MESSAGE_FRAME_ID_PEDALSLIBCANVERSIONINFO, &status_msg, frame.data) == -1) {
        return IDENTITY_RC_ERROR;
    }
    if (can_communications_api_add_to_tx_buffer(&frame) != CAN_COMMUNICATION_RC_OK) {
        return IDENTITY_RC_ERROR;
    }
    return IDENTITY_RC_OK;
}
