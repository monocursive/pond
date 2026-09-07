#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef struct {
  int16_t x, y, z;
  uint64_t last_sample, last_peak, pair_peak, settle_until;
  bool have_sample;
} Gesture;
void gesture_suppress(Gesture *g, uint64_t until);
bool gesture_sample(Gesture *g, int16_t x, int16_t y, int16_t z, uint64_t at,
                    bool vibrated);
