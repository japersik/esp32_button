#include <inttypes.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "japersik/esp32_button/virt_button.h"

#define INPUT_PIN_1 22	// connect the button to the 23 gpio pin and ground
#define INPUT_PIN_2 23	// connect the button to the 22 gpio pin and +3.3v

void print_state_callback(uint8_t id, ButtonEvent event) {
  static int counter = 0;
  counter++;
  printf("State event %d: Button %d:", counter, id);
  if (event & BUTTON_PRESS) {
    printf(" pressed");
  }
  if (event & BUTTON_PRESSING) {
    printf(" pressing");
  }
  if (event & BUTTON_RELEASE_CLICK) {
    printf(" released_click");
  }
  if (event & BUTTON_HOLD) {
    printf(" holded");
  }
  if (event & BUTTON_HOLDING) {
    printf(" holding");
  }
  if (event & BUTTON_RELEASE_HOLD) {
    printf(" released_hold");
  }
  if (event & BUTTON_STEP) {
    printf(" got_step");
  }
  printf("\n");
}

void hold_callback(uint8_t id, uint32_t hold_time) { printf("Button %d holded for %" PRIu32 " ms\n", id, hold_time); }
void multi_callback(uint8_t id, uint8_t counter) { printf("Button %d clicked %d times\n", id, counter); }

void app_main() {
  gpio_set_direction(INPUT_PIN_1, GPIO_MODE_INPUT);
  gpio_pulldown_dis(INPUT_PIN_1);
  gpio_pullup_en(INPUT_PIN_1);
  VirtButton *button1 = button_new(1);
  button_set_event_callback(button1, print_state_callback);
  button_set_hold_callback(button1, hold_callback);
  button_set_multiclick_callback(button1, multi_callback);
  button_set_inverse(button1, true);

  gpio_set_direction(INPUT_PIN_2, GPIO_MODE_INPUT);
  gpio_pullup_dis(INPUT_PIN_2);
  gpio_pulldown_en(INPUT_PIN_2);
  VirtButton *button2 = button_new(2);
  button_set_event_callback(button2, print_state_callback);
  button_set_hold_callback(button2, hold_callback);
  button_set_multiclick_callback(button2, multi_callback);
  button_set_inverse(button2, false);
  for (;;) {
    int level1 = gpio_get_level(INPUT_PIN_1);
    button_click_on_time(button1, level1, esp_timer_get_time() / 1000);
    int level2 = gpio_get_level(INPUT_PIN_2);
    button_click_on_time(button2, level2, esp_timer_get_time() / 1000);
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

