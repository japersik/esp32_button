#include "japersik/esp32_button/virt_button.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct VirtButton {
  ButtonEventCallbackFunc event_callback;
  ButtonMulticlickCallbackFunc multiclick_callback;
  ButtonHoldReleaseCallbackFunc hold_release_callback;
  void *event_callback_params;
  void *multiclick_callback_params;
  void *hold_release_callback_params;

  uint32_t last_step_time_ms;
  uint32_t press_start_time_ms;

  uint32_t hold_duration_ms;
  uint32_t step_duration_ms;
  uint32_t multiclick_duration_ms;

  uint8_t multiclick_counter;
  ButtonEvent state;
  bool inverse;
};

VirtButton *button_new(const VirtButtonConfig *config) {
  VirtButton *self = (VirtButton *)malloc(sizeof(VirtButton));
  if (self == NULL) {
    return NULL;
  }
  self->state = 0;
  self->last_step_time_ms = 0;
  self->multiclick_counter = 0;

  self->event_callback = NULL;
  self->hold_release_callback = NULL;
  self->multiclick_callback = NULL;
  self->event_callback_params = NULL;
  self->hold_release_callback_params = NULL;
  self->multiclick_callback_params = NULL;
  self->hold_duration_ms = 1000;
  self->step_duration_ms = 300;
  self->multiclick_duration_ms = 400;
  self->inverse = false;

  if (config) {
    self->event_callback = config->event_callback;
    self->hold_release_callback = config->hold_release_callback;
    self->multiclick_callback = config->multiclick_callback;
    self->event_callback_params = config->event_callback_params;
    self->hold_release_callback_params = config->hold_release_callback_params;
    self->multiclick_callback_params = config->multiclick_callback_params;

    if (config->hold_duration_ms)
      self->hold_duration_ms = config->hold_duration_ms;
    if (config->step_duration_ms)
      self->step_duration_ms = config->step_duration_ms;
    if (config->multiclick_duration_ms)
      self->multiclick_duration_ms = config->multiclick_duration_ms;
    if (config->inverse)
      self->inverse = config->inverse;
  }
  return self;
}

void button_free(VirtButton *self) { free(self); }

void button_set_event_callback(VirtButton *self, ButtonEventCallbackFunc f, void *p) {
  self->event_callback = f;
  self->event_callback_params = p;
}
void button_set_multiclick_callback(VirtButton *self, ButtonMulticlickCallbackFunc f, void *p) {
  self->multiclick_callback = f;
  self->multiclick_callback_params = p;
}
void button_set_hold_release_callback(VirtButton *self, ButtonHoldReleaseCallbackFunc f, void *p) {
  self->hold_release_callback = f;
  self->hold_release_callback_params = p;
}

void button_set_step_duration(VirtButton *self, uint32_t duration_ms) { self->step_duration_ms = duration_ms; }
void button_set_multiclick_timeut(VirtButton *self, uint32_t duration_ms) {
  self->multiclick_duration_ms = duration_ms;
}
void button_set_inverse(VirtButton *self, bool inverse) { self->inverse = inverse; }
bool button_is_processing(const VirtButton *self) {
  return self->state & BUTTON_PRESSING || self->multiclick_counter > 0;
}

static void process_click_on_time(VirtButton *self, bool pressed, uint32_t event_time_ms) {
  pressed ^= self->inverse;
  ButtonEvent state = self->state;
  state &=
      ~(BUTTON_PRESS | BUTTON_HOLD | BUTTON_RELEASE_SHORT_CLICK | BUTTON_RELEASE | BUTTON_RELEASE_HOLD | BUTTON_STEP);
  // send multiclick callback
  if (event_time_ms - self->press_start_time_ms >= self->multiclick_duration_ms) {
    if (self->multiclick_counter > 1 && self->multiclick_callback != NULL) {
      self->multiclick_callback(self->multiclick_callback_params, self->multiclick_counter);
    }
    self->multiclick_counter = 0;
  }
  // calculate state
  if (pressed) {
    if (!(state & BUTTON_PRESSING)) {
      self->multiclick_counter++;
      state |= BUTTON_PRESS | BUTTON_PRESSING;
      self->press_start_time_ms = event_time_ms;
    } else {
      if (state & BUTTON_HOLDING) {
	if ((event_time_ms - self->last_step_time_ms) <= self->step_duration_ms) {
	  state |= BUTTON_STEP;
	  self->last_step_time_ms += self->step_duration_ms;
	}
      } else {
	if ((event_time_ms - self->press_start_time_ms) >= self->hold_duration_ms) {
	  self->last_step_time_ms = event_time_ms;
	  state |= BUTTON_HOLD | BUTTON_HOLDING;
	}
      }
    }
  } else if (state & BUTTON_PRESSING) {
    if (state & BUTTON_HOLDING) {
      state |= BUTTON_RELEASE_HOLD | BUTTON_RELEASE;
    } else {
      state |= BUTTON_RELEASE_SHORT_CLICK | BUTTON_RELEASE;
    }
    state &= ~(BUTTON_HOLDING | BUTTON_PRESSING);
  }
  self->state = state;

  // send event callback
  if ((state & (BUTTON_PRESS | BUTTON_HOLD | BUTTON_RELEASE | BUTTON_STEP)) && self->event_callback != NULL) {
    self->event_callback(self->event_callback_params, state);
  }
  // send hold callback
  if (state & (BUTTON_RELEASE_HOLD) && self->hold_release_callback != NULL) {
    self->hold_release_callback(self->hold_release_callback_params, event_time_ms - self->press_start_time_ms);
  }
}

void button_click_on_time(VirtButton *self, bool pressed, uint32_t event_time_ms) {
  process_click_on_time(self, pressed, event_time_ms);
}

ButtonEvent button_get_state(const VirtButton *self) { return self->state; }
bool button_get_inverse(const VirtButton *self) { return self->inverse; }
