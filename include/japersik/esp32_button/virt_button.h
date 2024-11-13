#ifndef _JAPERSIK_ESP32_BUTTON_VIRT_BUTTON_H
#define _JAPERSIK_ESP32_BUTTON_VIRT_BUTTON_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t ButtonEvent;

// Possible button states
#define BUTTON_PRESS (1 << 0)	       // Button was pressed - event
#define BUTTON_PRESSING (1 << 1)       // Button is pressed now - state
#define BUTTON_HOLD (1 << 2)	       // Button holding has started - event
#define BUTTON_HOLDING (1 << 3)	       // Button is held down now - state
#define BUTTON_RELEASE_CLICK (1 << 4)  // Button is released - event
#define BUTTON_RELEASE_HOLD (1 << 5)   // Button is released after holding - event
#define BUTTON_STEP (1 << 6)	       // Hold step event - state

typedef void (*EventCallbackFunction)(uint8_t id, ButtonEvent);
typedef void (*MulticlickCallbackFunction)(uint8_t id, uint8_t count);
typedef void (*HoldCallbackFunction)(uint8_t id, uint32_t hold_duration_ms);

typedef struct {
  EventCallbackFunction event_callback;
  MulticlickCallbackFunction multiclick_callback;
  HoldCallbackFunction hold_callback;

  uint32_t last_step_time_ms;
  uint32_t press_start_time_ms;

  uint32_t hold_duration_ms;
  uint32_t step_duration_ms;
  uint32_t multiclick_duration_ms;

  uint8_t multiclick_counter;
  ButtonEvent state;
  uint8_t id;
  bool inverse;
} VirtButton;

VirtButton *button_new(uint8_t id);
void button_free(VirtButton *);
void button_set_event_callback(VirtButton *, EventCallbackFunction);
void button_set_hold_callback(VirtButton *, HoldCallbackFunction);
void button_set_multiclick_callback(VirtButton *, MulticlickCallbackFunction);
void button_set_step_duration(VirtButton *, uint32_t duration_ms);
void button_set_multiclick_timeut(VirtButton *self, uint32_t duration_ms);
void button_set_inverse(VirtButton *, bool inverse);

void button_click_on_time(VirtButton *, bool pressed, uint32_t time_ms);
ButtonEvent button_get_state(const VirtButton *);
bool button_is_processing(const VirtButton *);
#endif	// !_JAPERSIK_ESP32_BUTTON_VIRT_BUTTON_H
