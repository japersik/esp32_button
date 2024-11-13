#include "japersik/esp32_button/gpio_observer.h"

#include <stdio.h>
#include <stdlib.h>

#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "japersik/esp32_button/virt_button.h"
#include "portmacro.h"

void monitor_gpio_task(void *params);

ButtonObserver *button_observer_new() {
  ButtonObserver *self = (ButtonObserver *)malloc(sizeof(ButtonObserver));
  if (self == NULL) {
    return NULL;
  }
  self->interruptQueue = xQueueCreate(20, sizeof(buttonInfo *));
  self->activeButtonsNum = 0;

  xTaskCreate(monitor_gpio_task, "monitor_gpio_task", 2048, self, 1, NULL);
  return self;
}

void button_observer_free(ButtonObserver *observer) {
  if (observer == NULL) {
    return;
  }
  if (observer->interruptQueue != NULL) {
    vQueueDelete(observer->interruptQueue);
  }
  // TODO remove all buttonInfo, monitoring task and all ISRs
  free(observer);
}

static void IRAM_ATTR gpio_isr_handler(void *arg) {
  buttonInfo *params = (buttonInfo *)arg;
  gpio_isr_handler_remove(params->pin);
  xQueueSendFromISR(params->observer->interruptQueue, &(params), NULL);
}

void button_observer_add_button(ButtonObserver *observer, VirtButton *button, gpio_num_t pin) {
  gpio_config_t io_conf = {};
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = 1 << pin;

  if (button->inverse) {
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  } else {
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
  }
  gpio_config(&io_conf);

  buttonInfo *data = (buttonInfo *)malloc(sizeof(buttonInfo));
  data->pin = pin;
  data->button = button;
  data->observer = observer;

  gpio_isr_handler_add(pin, gpio_isr_handler, (void *)data);
}

void monitor_gpio_task(void *params) {
  ButtonObserver *observer = (ButtonObserver *)params;
  buttonInfo *data_from_isr;

  TickType_t waitTime = portMAX_DELAY;

  while (true) {
    // receive and start monitoring
    if (xQueueReceive(observer->interruptQueue, &data_from_isr, waitTime)) {
      button_click_on_time(data_from_isr->button, !data_from_isr->button->inverse, esp_timer_get_time() / 1000);
      observer->activeButtons[observer->activeButtonsNum] = data_from_isr;
      observer->activeButtonsNum++;
    }
    // wait period
    vTaskDelay(20 / portTICK_PERIOD_MS);
    // process all active buttons
    for (int i = 0; i < observer->activeButtonsNum; i++) {
      int level = gpio_get_level(observer->activeButtons[i]->pin);
      button_click_on_time(observer->activeButtons[i]->button, level, esp_timer_get_time() / 1000);
      if (!button_is_processing(observer->activeButtons[i]->button)) {
	gpio_isr_handler_add(observer->activeButtons[i]->pin, gpio_isr_handler, (void *)observer->activeButtons[i]);
	observer->activeButtons[i] = observer->activeButtons[observer->activeButtonsNum - 1];
	observer->activeButtonsNum--;
      }
    }
    // set a wait period
    if (observer->activeButtonsNum == 0) {
      waitTime = portMAX_DELAY;
    } else {
      waitTime = 0;
    }
  }
}

