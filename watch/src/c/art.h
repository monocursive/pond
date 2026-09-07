#pragma once
#include <pebble.h>

GColor pond_background(void);
GColor pond_ink(void);
void pond_draw(GContext *ctx, GRect frame, uint8_t shape, uint8_t palette,
               uint32_t seed, bool live, bool offline, int animation,
               bool lifted);
