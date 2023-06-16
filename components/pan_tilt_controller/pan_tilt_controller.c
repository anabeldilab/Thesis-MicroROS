#include "pan_tilt_controller.h"

TimerState timers[4] = {0};
PanTiltState pan_tilt_state = {0};

void init_horizontal_servo(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num);
void init_vertical_servo(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num);
void init_pwm_timer(uint8_t timer_selected, uint8_t timer_freq_hz);
void configure_timer(uint8_t timer_selected, uint8_t timer_freq_hz);
void configure_servo(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num);
void update_timer_state(uint8_t timer_selected, uint8_t timer_freq_hz);
void update_horizontal_servo_state(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num);
void update_vertical_servo_state(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num);

void pan_tilt_init(void) {
  init_pwm_timer(0, 50);
  init_horizontal_servo(0, 0, 17);
  init_vertical_servo(1, 0, 16);
}

void set_horizontal_angle(int16_t angle) {
  if (angle < 0) {
    angle = 0;
    return;
  } else if (angle > 180) {
    angle = 180;
    return;
  } else if (!pan_tilt_state.horizontal_servo.is_initialized) {
    printf("Error: the horizontal servomotor isn't initialized.\n");
    return;
  }

  uint32_t duty = ((angle * (SERVO_MAX_DUTY - SERVO_MIN_DUTY)) / 180) + SERVO_MIN_DUTY;
  uint8_t channel = pan_tilt_state.horizontal_servo.channel;

  pan_tilt_state.horizontal_servo.angle = angle;
  pan_tilt_state.horizontal_servo.duty = duty;

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);  
}

void set_vertical_angle(int16_t angle) {
  if (angle < 0 || angle > 180) {
      printf("Error: the angle should be between 0 and 180 grades.\n");
      return;
  } else if (!pan_tilt_state.vertical_servo.is_initialized) {
    printf("Error: the vertical servomotor isn't initialized.\n");
    return;
  }

  uint32_t duty = ((angle * (SERVO_MAX_DUTY - SERVO_MIN_DUTY)) / 180) + SERVO_MIN_DUTY;
  uint8_t channel = pan_tilt_state.vertical_servo.channel;

  pan_tilt_state.vertical_servo.angle = angle;
  pan_tilt_state.vertical_servo.duty = duty;

  ledc_set_duty(LEDC_HIGH_SPEED_MODE, channel, duty);
  ledc_update_duty(LEDC_HIGH_SPEED_MODE, channel);  
}

void init_horizontal_servo(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num) {
  if (channel_selected > 7) {
    printf("Error: The channel selected must be between 0 and 7.\n");
    return;
  }

  if (timers[timer_selected].is_initialized) {
    configure_servo(channel_selected, timer_selected, gpio_num);
    update_horizontal_servo_state(channel_selected, timer_selected, gpio_num);
  }
}

void init_vertical_servo(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num) {
  if (channel_selected > 7) {
    printf("Error: The channel selected must be between 0 and 7.\n");
    return;
  }

  if (timers[timer_selected].is_initialized) {
    configure_servo(channel_selected, timer_selected, gpio_num);
    update_vertical_servo_state(channel_selected, timer_selected, gpio_num);
  }
}

void init_pwm_timer(uint8_t timer_selected, uint8_t timer_freq_hz) {
  if (timer_selected > 3) {
    printf("Error: The timer selected must be between 0 and 3.\n");
    return;
  }
  if (!timers[timer_selected].is_initialized) {
    configure_timer(timer_selected, timer_freq_hz);
    update_timer_state(timer_selected, timer_freq_hz);
  } else {
    printf("Error: The timer %d is already initialized at a frecuency of %d Hz.\n", timer_selected, timers[timer_selected].freq_hz);
    return;
  }
}

void configure_timer(uint8_t timer_selected, uint8_t timer_freq_hz) {
  ledc_timer_config_t timer_conf;
  timer_conf.duty_resolution = LEDC_TIMER_15_BIT; // resolution of PWM duty. Max duty is 32767
  timer_conf.freq_hz = timer_freq_hz;
  timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
  timer_conf.timer_num = timer_selected;
  ledc_timer_config(&timer_conf);
}

void configure_servo(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num) {
  ledc_channel_config_t ledc_conf;
  ledc_conf.channel = channel_selected;
  ledc_conf.duty = 0;
  ledc_conf.gpio_num = gpio_num;
  ledc_conf.intr_type = LEDC_INTR_DISABLE;
  ledc_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
  ledc_conf.timer_sel = timer_selected;
  ledc_channel_config(&ledc_conf);
}

void update_timer_state(uint8_t timer_selected, uint8_t timer_freq_hz) {
  timers[timer_selected].is_initialized = 1;
  timers[timer_selected].freq_hz = timer_freq_hz;
}

void update_horizontal_servo_state(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num) {
  pan_tilt_state.horizontal_servo.is_initialized = 1;
  pan_tilt_state.horizontal_servo.timer = timer_selected;
  pan_tilt_state.horizontal_servo.channel = channel_selected;
  pan_tilt_state.horizontal_servo.gpio_num = gpio_num;
  pan_tilt_state.horizontal_servo.angle = 0;
  pan_tilt_state.horizontal_servo.duty = SERVO_MIN_DUTY;
}

void update_vertical_servo_state(uint8_t channel_selected, uint8_t timer_selected, uint8_t gpio_num) {
  pan_tilt_state.vertical_servo.is_initialized = 1;
  pan_tilt_state.vertical_servo.timer = timer_selected;
  pan_tilt_state.vertical_servo.channel = channel_selected;
  pan_tilt_state.vertical_servo.gpio_num = gpio_num;
  pan_tilt_state.vertical_servo.angle = 0;
  pan_tilt_state.vertical_servo.duty = SERVO_MIN_DUTY;
}