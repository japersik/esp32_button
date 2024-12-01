#include <inttypes.h>
#include <stdio.h>

#include "driver/gpio.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "japersik/esp32_button/callback_funcs.h"
#include "japersik/esp32_button/gpio_observer.h"
#include "japersik/esp32_button/virt_button.h"

#define INPUT_PIN_1 21	// connect the button to the 21 gpio pin and ground
#define INPUT_PIN_2 19	// connect the button to the 19 gpio pin and +3.3v

void app_main() {
  char *message1 = "1";
  VirtButtonConfig config = {
      .inverse = true,
      .event_callback = print_state_callback,
      .event_callback_params = message1,
      .multiclick_callback = multi_callback,
      .multiclick_callback_params = message1,
      .hold_release_callback = hold_callback,
      .hold_release_callback_params = message1,

  };
  VirtButton *button1 = button_new(&config);
  button_set_inverse(button1, true);

  char *message2 = "2";
  VirtButton *button2 = button_new(NULL);
  button_set_event_callback(button2, print_state_callback, message2);
  button_set_hold_release_callback(button2, hold_callback, message2);
  button_set_multiclick_callback(button2, multi_callback, message2);
  button_set_inverse(button2, false);

  ButtonObserver *observer = button_observer_new();
  gpio_install_isr_service(0);
  button_observer_add_button(observer, button1, INPUT_PIN_1);
  button_observer_add_button(observer, button2, INPUT_PIN_2);
}

