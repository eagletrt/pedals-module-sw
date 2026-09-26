#include "unity.h"
#include "can-communications-router-api.h"

extern bool can_communications_router_bootloader_requested;

static struct CanCommunicationFrame xcp_connect_frame(void) {
    struct CanCommunicationFrame frame = {
        .id = CAN_COMMUNICATIONS_ROUTER_BOOTLOADER_RX_ID,
        .length = CAN_COMMUNICATIONS_ROUTER_XCP_CONNECT_LENGTH,
        .data = { CAN_COMMUNICATIONS_ROUTER_XCP_CMD_CONNECT, 0x00U },
    };
    return frame;
}

void setUp(void) {
    can_communications_router_bootloader_requested = false;
}

void test_router_null_frame() {
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_NULL_POINTER, can_communications_router_api_receive_primary(NULL));
    TEST_ASSERT_FALSE(can_communications_router_api_is_bootloader_requested());
}

void test_router_bootloader_not_requested_by_default() {
    TEST_ASSERT_FALSE(can_communications_router_api_is_bootloader_requested());
}

void test_router_xcp_connect_requests_bootloader() {
    struct CanCommunicationFrame frame = xcp_connect_frame();

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_TRUE(can_communications_router_api_is_bootloader_requested());
}

void test_router_xcp_connect_mode_byte_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.data[1] = 0x01U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_TRUE(can_communications_router_api_is_bootloader_requested());
}

void test_router_other_id_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.id = CAN_COMMUNICATIONS_ROUTER_BOOTLOADER_RX_ID - 1U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_FALSE(can_communications_router_api_is_bootloader_requested());
}

void test_router_other_xcp_command_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.data[0] = 0xFEU; // XCP DISCONNECT

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_FALSE(can_communications_router_api_is_bootloader_requested());
}

void test_router_wrong_length_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.length = 8U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_FALSE(can_communications_router_api_is_bootloader_requested());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_router_null_frame);
    RUN_TEST(test_router_bootloader_not_requested_by_default);
    RUN_TEST(test_router_xcp_connect_requests_bootloader);
    RUN_TEST(test_router_xcp_connect_mode_byte_ignored);
    RUN_TEST(test_router_other_id_ignored);
    RUN_TEST(test_router_other_xcp_command_ignored);
    RUN_TEST(test_router_wrong_length_ignored);
    UNITY_END();
}
