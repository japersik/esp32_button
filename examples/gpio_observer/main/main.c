#include <inttypes.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "japersik/esp32_button/callback_funcs.h"
#include "japersik/esp32_button/gpio_observer.h"
#include "japersik/esp32_button/virt_button.h"

#define INPUT_PIN_1 22	// connect the button to the 23 gpio pin and ground
#define INPUT_PIN_2 23	// connect the button to the 22 gpio pin and +3.3v

void app_main() {
  VirtButton *button1 = button_new(1);
  button_set_event_callback(button1, print_state_callback);
  button_set_hold_callback(button1, hold_callback);
  button_set_multiclick_callback(button1, multi_callback);
  button_set_inverse(button1, true);

  VirtButton *button2 = button_new(2);
  button_set_event_callback(button2, print_state_callback);
  button_set_hold_callback(button2, hold_callback);
  button_set_multiclick_callback(button2, multi_callback);
  button_set_inverse(button2, false);

  ButtonObserver *observer = button_observer_new();
  gpio_install_isr_service(0);
  button_observer_add_button(observer, button1, INPUT_PIN_1);
  button_observer_add_button(observer, button2, INPUT_PIN_2);
}

