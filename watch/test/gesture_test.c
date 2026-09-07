#include "../src/c/gesture.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
  Gesture g = {0};
  assert(!gesture_sample(&g, 0, 0, 1000, 1000, false));
  assert(!gesture_sample(&g, 2200, 0, 1000, 1200, false));
  assert(gesture_sample(&g, 0, 0, 1000, 1500, false));
  // One candidate outside the pairing window starts a new pair.
  assert(!gesture_sample(&g, 2200, 0, 1000, 2500, false));
  assert(!gesture_sample(&g, 0, 0, 1000, 3200, false));
  // Vibration contamination cancels the partial pair and masks settling
  // samples.
  assert(!gesture_sample(&g, 2200, 0, 1000, 3500, true));
  assert(!gesture_sample(&g, 0, 0, 1000, 3800, false));
  assert(!gesture_sample(&g, 2200, 0, 1000, 5800, false));
  assert(!gesture_sample(&g, 0, 0, 1000, 6200, false));
  assert(gesture_sample(&g, 2200, 0, 1000, 6500, false));
  gesture_suppress(&g, 10000);
  assert(!gesture_sample(&g, -32768, 32767, 32767, 6800, false));
  assert(!gesture_sample(&g, 32767, -32768, -32768, 7100, false));
  // Clock reversal does not complete an old pair.
  assert(!gesture_sample(&g, 0, 0, 0, 100, false));

  Gesture first = {0};
  assert(!gesture_sample(&first, 0, 0, 1000, 1000, true));
  assert(!gesture_sample(&first, 2200, 0, 1000, 1200, false));
  assert(!gesture_sample(&first, 0, 0, 1000, 1500, false));
  assert(!gesture_sample(&first, 2200, 0, 1000, 3501, false));
  assert(gesture_sample(&first, 0, 0, 1000, 3801, false));

  Gesture reversed = {0};
  assert(!gesture_sample(&reversed, 0, 0, 1000, 100000, false));
  gesture_suppress(&reversed, 102500);
  assert(!gesture_sample(&reversed, 0, 0, 1000, 1000, false));
  assert(!gesture_sample(&reversed, 2200, 0, 1000, 1200, false));
  assert(!gesture_sample(&reversed, 0, 0, 1000, 1500, false));
  assert(!gesture_sample(&reversed, 2200, 0, 1000, 4200, false));
  assert(gesture_sample(&reversed, 0, 0, 1000, 4500, false));
  puts("Gesture classifier: passed (pairing, expired pairs, motor settling, "
       "clock reversal)");
}
