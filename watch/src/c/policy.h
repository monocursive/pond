#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef struct {
  uint32_t revision, seed;
  int32_t pause_until;
  uint16_t quiet_start, quiet_end;
  uint8_t joined, gentle, all_day, reduced, tick, shape, palette;
} Settings;
typedef struct {
  int32_t incoming[4], hold_until, last_time;
  uint32_t last_bundle, request_counter, magic;
} Budget;
bool policy_quiet(const Settings *s, int minute);
void policy_init(Budget *b, int32_t now);
bool policy_check_time(Budget *b, int32_t now, bool reliable);
bool policy_direct(const Settings *s, int32_t now, int minute,
                   bool system_quiet);
bool policy_incoming(const Settings *s, const Budget *b, int32_t now,
                     int minute, bool system_quiet);
bool policy_haptic(const Settings *s, const Budget *b, int32_t now, int minute,
                   bool system_quiet, bool trusted_time, bool incoming);
void policy_reserve(Budget *b, int32_t now);
