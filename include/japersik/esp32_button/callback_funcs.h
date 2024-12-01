#ifndef _JAPERSIK_ESP32_BUTTON_CALLBACK_FUNCS_H
#define _JAPERSIK_ESP32_BUTTON_CALLBACK_FUNCS_H

#include <inttypes.h>
#include <stdio.h>

#include "japersik/esp32_button/virt_button.h"

void print_state_callback(void *params, ButtonEvent event) {
  char *message = (char *)params;
  static int counter = 0;
  counter++;
  printf("State event %d: Button %s:", counter, message);
  if (event & BUTTON_PRESS) {
    printf(" pressed");
  }
  if (event & BUTTON_PRESSING) {
    printf(" pressing");
  }
  if (event & BUTTON_RELEASE) {
    printf(" released");
  }

  if (event & BUTTON_RELEASE_SHORT_CLICK) {
    printf(" released_short");
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

void hold_callback(void *params, uint32_t hold_time) {
  char *message = (char *)params;
  printf("Button %s holded for %" PRIu32 " ms\n", message, hold_time);
}
void multi_callback(void *params, uint8_t counter) {
  char *message = (char *)params;
  printf("Button %s clicked %d times\n", message, counter);
}
#endif	//_JAPERSIK_ESP32_BUTTON_CALLBACK_FUNCS_H
