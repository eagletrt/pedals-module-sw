#include "unity.h"
#include "bots-api.h"

extern float bots_voltage;

void setUp(void) {
    bots_voltage = 24.0F;
}

/*void tearDown(void) {
    // clean stuff up here
}*/

void test_bots_not_activated() {
    bots_voltage = 24.0F;
    bool res = bots_is_triggered();

    TEST_ASSERT_FALSE(res);
}

void test_bots_activated() {
    bots_voltage = 0.1F;
    bool res = bots_is_triggered();

    TEST_ASSERT_TRUE(res);
}

void test_bots_set_voltage() {
    bots_set_voltage(12.0F);
    TEST_ASSERT_EQUAL_FLOAT(12.0F, bots_voltage);
}

void test_bots_get_voltage() {
    bots_voltage = 5.0F;
    float voltage = bots_get_voltage();
    TEST_ASSERT_EQUAL_FLOAT(5.0F, voltage);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_bots_activated);
    RUN_TEST(test_bots_not_activated);
    RUN_TEST(test_bots_set_voltage);
    RUN_TEST(test_bots_get_voltage);
    UNITY_END();
}
