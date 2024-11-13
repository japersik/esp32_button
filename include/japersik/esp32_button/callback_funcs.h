#ifndef _JAPERSIK_ESP32_BUTTON_CALLBACK_FUNCS_H
#define _JAPERSIK_ESP32_BUTTON_CALLBACK_FUNCS_H

#include <inttypes.h>
#include <stdio.h>

#include "japersik/esp32_button/virt_button.h"

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
#endif	//_JAPERSIK_ESP32_BUTTON_CALLBACK_FUNCS_H
