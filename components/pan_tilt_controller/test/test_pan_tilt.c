#include "unity.h"

#include "pan_tilt_controller.h"

void setUp(void) {
  pan_tilt_init(0, 50, 0, 1, 17, 16);
}

void tearDown(void) {
  pan_tilt_deinit();
}

TEST_CASE("Test max duty and min duty", "[pan_tilt_controller]") {
  TEST_ASSERT_EQUAL(1638, SERVO_MIN_DUTY);
  TEST_ASSERT_EQUAL(3276, SERVO_MAX_DUTY);
}

TEST_CASE("Test pan-tilt initializated", "[pan_tilt_controller]") {
  int8_t timer_selected_state = timers[0].is_initialized;
  int8_t horizontal_servo_state = pan_tilt_state.horizontal_servo.is_initialized;
  int8_t vertical_servo_state = pan_tilt_state.vertical_servo.is_initialized;


  TEST_ASSERT_EQUAL(1, timer_selected_state * horizontal_servo_state * vertical_servo_state);
}

TEST_CASE("Test initial horizontal angle", "[pan_tilt_controller]") {
  set_horizontal_angle(0);

  int16_t horizontal_angle_state = pan_tilt_state.horizontal_servo.angle;
  int32_t horizontal_duty_state = pan_tilt_state.horizontal_servo.duty;

  TEST_ASSERT_EQUAL(0, horizontal_angle_state);
  TEST_ASSERT_EQUAL(SERVO_MIN_DUTY, horizontal_duty_state);
}

TEST_CASE("Test horizontal angle", "[pan_tilt_controller]") {

  set_horizontal_angle(90);

  int16_t horizontal_angle_state = pan_tilt_state.horizontal_servo.angle;
  int32_t horizontal_duty_state = pan_tilt_state.horizontal_servo.duty;

  TEST_ASSERT_EQUAL(90, horizontal_angle_state);
  TEST_ASSERT_EQUAL(SERVO_MIN_DUTY + 819, horizontal_duty_state);
}

TEST_CASE("Test initial vertical angle", "[pan_tilt_controller]") {

  set_vertical_angle(0);

  int16_t vertical_angle_state = pan_tilt_state.vertical_servo.angle;
  int32_t vertical_duty_state = pan_tilt_state.vertical_servo.duty;

  TEST_ASSERT_EQUAL(0, vertical_angle_state);
  TEST_ASSERT_EQUAL(SERVO_MIN_DUTY, vertical_duty_state);
}

TEST_CASE("Test vertical angle", "[pan_tilt_controller]") {

  set_vertical_angle(15);

  int16_t vertical_angle_state = pan_tilt_state.vertical_servo.angle;
  int32_t vertical_duty_state = pan_tilt_state.vertical_servo.duty;

  TEST_ASSERT_EQUAL(15, vertical_angle_state);
  TEST_ASSERT_EQUAL(SERVO_MIN_DUTY + 136, vertical_duty_state);
}
