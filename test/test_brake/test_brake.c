#include "unity.h"
#include "brake-api.h"

/*void setUp(void) {}*/

/*void tearDown(void) {}*/

// other function won't be tested because they are just getter/setter

void test_brake_api_update_pedal_travel_percentage_in_range(void) {
    float perc = 0.3F;
    brake_api_update_pedal_travel_percentage(perc);

    TEST_ASSERT_EQUAL_FLOAT(brake_api_get_pedal_travel_percentage(), perc);
}

void test_brake_api_update_pedal_travel_percentage_high(void) {
    float perc = 1.2F;
    brake_api_update_pedal_travel_percentage(perc);

    TEST_ASSERT_EQUAL_FLOAT(brake_api_get_pedal_travel_percentage(), (-1.0F));
}

void test_brake_api_update_pedal_travel_percentage_low(void) {
    float perc = -0.8F;
    brake_api_update_pedal_travel_percentage(perc);

    TEST_ASSERT_EQUAL_FLOAT(brake_api_get_pedal_travel_percentage(), (-1.0F));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_brake_api_update_pedal_travel_percentage_in_range);
    RUN_TEST(test_brake_api_update_pedal_travel_percentage_high);
    RUN_TEST(test_brake_api_update_pedal_travel_percentage_low);

    UNITY_END();
}
