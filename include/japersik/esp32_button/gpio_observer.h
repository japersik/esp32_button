#ifndef _JAPERSIK_ESP32_BUTTON_GPIO_OBSERVER_H
#define _JAPERSIK_ESP32_BUTTON_GPIO_OBSERVER_H

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "hal/gpio_types.h"
#include "soc/gpio_num.h"
#include "virt_button.h"

#define MAX_BUTTONS GPIO_NUM_MAX
typedef struct ButtonObserver ButtonObserver;

ButtonObserver *button_observer_new();
void button_observer_free(ButtonObserver *observer);
void button_observer_add_button(ButtonObserver *observer, VirtButton *button, gpio_num_t pin);

#endif	// _JAPERSIK_ESP32_BUTTON_GPIO_OBSERVER_H
