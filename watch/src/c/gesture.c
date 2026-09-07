#include "gesture.h"
void gesture_suppress(Gesture *g, uint64_t until) {
  g->settle_until = until;
  g->pair_peak = 0;
}
bool gesture_sample(Gesture *g, int16_t x, int16_t y, int16_t z, uint64_t at,
                    bool vibrated) {
  int32_t dx = x - g->x, dy = y - g->y, dz = z - g->z;
  bool reversed = g->have_sample && at < g->last_sample;
  bool reset = !g->have_sample || reversed;
  // A deadline on the old timeline must not lock out future gestures.
  // Restart the short settling interval conservatively after a clock reset.
  if (reversed || vibrated)
    gesture_suppress(g, at + 2500);
  g->x = x;
  g->y = y;
  g->z = z;
  g->last_sample = at;
  g->have_sample = true;
  if (reset) {
    g->pair_peak = g->last_peak = 0;
    return false;
  }
  if (at < g->settle_until)
    return false;
  // Physical Time 2 feedback: 1.8 g required an uncomfortable knock. Keep
  // the two-peak timing/motor guards, but accept a lighter 0.9 g impulse.
  if ((int64_t)dx * dx + (int64_t)dy * dy + (int64_t)dz * dz < 900 * 900 ||
      at - g->last_peak < 160)
    return false;
  g->last_peak = at;
  if (g->pair_peak && at - g->pair_peak <= 600) {
    g->pair_peak = 0;
    return true;
  }
  g->pair_peak = at;
  return false;
}
