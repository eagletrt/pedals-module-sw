#include "unity.h"
#include "bots-api.h" // instead of h to use static var

extern bool bots_is_trigger_activated;

void setUp(void) {
    bots_is_trigger_activated = false;
}

/*void tearDown(void) {
    // clean stuff up here
}*/

void test_bots_not_activated() {
    bool res = bots_is_triggered();

    TEST_ASSERT_FALSE(res);
    TEST_ASSERT_FALSE(bots_is_trigger_activated);
}

void test_bots_activated() {
    bots_trigger();
    bool res = bots_is_triggered();

    TEST_ASSERT_TRUE(res);
    TEST_ASSERT_FALSE(bots_is_trigger_activated);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_bots_activated);
    RUN_TEST(test_bots_not_activated);
    UNITY_END();
}