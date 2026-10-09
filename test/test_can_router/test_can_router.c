#include "unity.h"
#include "can-communications-router-api.h"
#include "bootloader-api.h"
#include "pedals/timebase/timebase-api.h"
#include "pedals/watchdogs/watchdogs-api.h"

static struct CanCommunicationFrame xcp_connect_frame(void) {
    struct CanCommunicationFrame frame = {
        .id = BOOTLOADER_CAN_RX_ID,
        .length = BOOTLOADER_XCP_CONNECT_LENGTH,
        .data = { BOOTLOADER_XCP_CMD_CONNECT, 0x00U },
    };
    return frame;
}

void setUp(void) {
    TEST_ASSERT_EQUAL(TIMEBASE_RC_OK, timebase_init());
    TEST_ASSERT_EQUAL(WATCHDOG_RC_OK, watchdogs_init());
    TEST_ASSERT_EQUAL(BOOTLOADER_RC_OK, bootloader_init());
}

void test_router_null_frame() {
    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_NULL_POINTER, can_communications_router_api_receive_primary(NULL));
    TEST_ASSERT_FALSE(bootloader_is_requested());
}

void test_router_bootloader_not_requested_by_default() {
    TEST_ASSERT_FALSE(bootloader_is_requested());
}

void test_router_xcp_connect_requests_bootloader() {
    struct CanCommunicationFrame frame = xcp_connect_frame();

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_TRUE(bootloader_is_requested());
}

void test_router_xcp_connect_mode_byte_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.data[1] = 0x01U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_TRUE(bootloader_is_requested());
}

void test_router_other_id_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.id = BOOTLOADER_CAN_RX_ID - 1U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_FALSE(bootloader_is_requested());
}

void test_router_other_xcp_command_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.data[0] = 0xFEU; // XCP DISCONNECT

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_FALSE(bootloader_is_requested());
}

void test_router_wrong_length_ignored() {
    struct CanCommunicationFrame frame = xcp_connect_frame();
    frame.length = 8U;

    TEST_ASSERT_EQUAL(CAN_COMMUNICATION_RC_OK, can_communications_router_api_receive_primary(&frame));
    TEST_ASSERT_FALSE(bootloader_is_requested());
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
    return UNITY_END();
}
