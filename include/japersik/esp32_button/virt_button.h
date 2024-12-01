#ifndef _JAPERSIK_ESP32_BUTTON_VIRT_BUTTON_H
#define _JAPERSIK_ESP32_BUTTON_VIRT_BUTTON_H

#include <stdbool.h>
#include <stdint.h>

typedef uint8_t ButtonEvent;

typedef void (*ButtonEventCallbackFunc)(void *params, ButtonEvent);
typedef void (*ButtonMulticlickCallbackFunc)(void *params, uint8_t count);
typedef void (*ButtonHoldReleaseCallbackFunc)(void *params, uint32_t hold_duration_ms);
// ButtonEvent bit flags
#define BUTTON_PRESS (1 << 0)		     // Button was pressed - event
#define BUTTON_PRESSING (1 << 1)	     // Button is pressed now - state
#define BUTTON_HOLD (1 << 2)		     // Button holding has started - event
#define BUTTON_HOLDING (1 << 3)		     // Button is held down now - state
#define BUTTON_RELEASE (1 << 4)		     // Button is released - event
#define BUTTON_RELEASE_SHORT_CLICK (1 << 5)  // Short click released - event
#define BUTTON_RELEASE_HOLD (1 << 6)	     // Long click released - event
#define BUTTON_STEP (1 << 7)		     // Hold step event - event

typedef struct VirtButton VirtButton;
typedef struct VirtButtonConfig {
  ButtonEventCallbackFunc event_callback;
  ButtonMulticlickCallbackFunc multiclick_callback;
  ButtonHoldReleaseCallbackFunc hold_release_callback;
  void *event_callback_params;
  void *multiclick_callback_params;
  void *hold_release_callback_params;

  uint32_t hold_duration_ms;
  uint32_t step_duration_ms;
  uint32_t multiclick_duration_ms;

  bool inverse;
} VirtButtonConfig;

VirtButton *button_new(const VirtButtonConfig *config);
void button_free(VirtButton *button);

void button_set_event_callback(VirtButton *, ButtonEventCallbackFunc, void *params);
void button_set_hold_release_callback(VirtButton *, ButtonHoldReleaseCallbackFunc, void *params);
void button_set_multiclick_callback(VirtButton *, ButtonMulticlickCallbackFunc, void *params);

void button_set_step_duration(VirtButton *, uint32_t duration_ms);
void button_set_multiclick_timeut(VirtButton *self, uint32_t duration_ms);
void button_set_inverse(VirtButton *, bool inverse);

bool button_get_inverse(const VirtButton *);

void button_click_on_time(VirtButton *, bool pressed, uint32_t time_ms);
ButtonEvent button_get_state(const VirtButton *);
bool button_is_processing(const VirtButton *);
#endif	// !_JAPERSIK_ESP32_BUTTON_VIRT_BUTTON_H
