
#include "japersik/esp32_button/virt_button.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

VirtButton *button_new(uint8_t id) {
  VirtButton *self = (VirtButton *)malloc(sizeof(VirtButton));
  self->state = 0;
  self->id = id;
  self->event_callback = NULL;
  self->hold_callback = NULL;
  self->multiclick_callback = NULL;
  self->last_step_time_ms = 0;
  self->hold_duration_ms = 1000;
  self->step_duration_ms = 300;
  self->multiclick_duration_ms = 400;
  self->multiclick_counter = 0;
  return self;
}

void button_free(VirtButton *self) { free(self); }

void button_set_event_callback(VirtButton *self, EventCallbackFunction f) { self->event_callback = f; }
void button_set_hold_callback(VirtButton *self, HoldCallbackFunction f) { self->hold_callback = f; }
void button_set_multiclick_callback(VirtButton *self, MulticlickCallbackFunction f) { self->multiclick_callback = f; }
void button_set_step_duration(VirtButton *self, uint32_t duration_ms) { self->step_duration_ms = duration_ms; }
void button_set_multiclick_timeut(VirtButton *self, uint32_t duration_ms) {
  self->multiclick_duration_ms = duration_ms;
}
void button_set_inverse(VirtButton *self, bool inverse) { self->inverse = inverse; }

static void process_click_on_time(VirtButton *self, bool pressed, uint32_t event_time_ms) {
  pressed ^= self->inverse;
  ButtonEvent state = self->state;
  state &= ~(BUTTON_PRESS | BUTTON_HOLD | BUTTON_RELEASE_CLICK | BUTTON_RELEASE_HOLD | BUTTON_STEP);
  // send multiclick callback
  if (event_time_ms - self->press_start_time_ms >= self->multiclick_duration_ms) {
    if (self->multiclick_counter > 1) {
      self->multiclick_callback(self->id, self->multiclick_counter);
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
      state |= BUTTON_RELEASE_HOLD;
    } else {
      state |= BUTTON_RELEASE_CLICK;
    }
    state &= ~(BUTTON_HOLDING | BUTTON_PRESSING);
  }
  self->state = state;

  // send event callback
  if ((state & (BUTTON_PRESS | BUTTON_HOLD | BUTTON_RELEASE_CLICK | BUTTON_RELEASE_HOLD | BUTTON_STEP)) &&
      self->event_callback != NULL) {
    self->event_callback(self->id, state);
  }
  // send hold callback
  if (state & (BUTTON_RELEASE_HOLD) && self->hold_callback != NULL) {
    self->hold_callback(self->id, event_time_ms - self->press_start_time_ms);
  }
}

void button_click_on_time(VirtButton *self, bool pressed, uint32_t event_time_ms) {
  process_click_on_time(self, pressed, event_time_ms);
}

ButtonEvent button_get_state(const VirtButton *self) { return self->state; }

