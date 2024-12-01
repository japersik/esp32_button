#include <inttypes.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "japersik/esp32_button/callback_funcs.h"
#include "japersik/esp32_button/virt_button.h"

#define INPUT_PIN_1 22	// connect the button to the 23 gpio pin and ground
#define INPUT_PIN_2 23	// connect the button to the 22 gpio pin and +3.3v

void app_main() {
  gpio_set_direction(INPUT_PIN_1, GPIO_MODE_INPUT);
  gpio_pulldown_dis(INPUT_PIN_1);
  gpio_pullup_en(INPUT_PIN_1);
  char *message1 = "1";
  VirtButton *button1 = button_new(NULL);
  button_set_event_callback(button1, print_state_callback, message1);
  button_set_hold_release_callback(button1, hold_callback, message1);
  button_set_multiclick_callback(button1, multi_callback, message1);
  button_set_inverse(button1, true);

  gpio_set_direction(INPUT_PIN_2, GPIO_MODE_INPUT);
  gpio_pullup_dis(INPUT_PIN_2);
  gpio_pulldown_en(INPUT_PIN_2);

  char *message2 = "2";
  VirtButton *button2 = button_new(NULL);
  button_set_event_callback(button2, print_state_callback, message2);
  button_set_hold_release_callback(button2, hold_callback, message2);
  button_set_multiclick_callback(button2, multi_callback, message2);
  button_set_inverse(button2, false);
  for (;;) {
    int level1 = gpio_get_level(INPUT_PIN_1);
    button_click_on_time(button1, level1, esp_timer_get_time() / 1000);
    int level2 = gpio_get_level(INPUT_PIN_2);
    button_click_on_time(button2, level2, esp_timer_get_time() / 1000);
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

