#include "policy.h"
#include <string.h>
#define DAY 86400
#define MAGIC 0x504f4e44
void policy_init(Budget *b, int32_t now) {
  memset(b, 0, sizeof(*b));
  b->magic = MAGIC;
  b->hold_until = now + DAY;
  b->last_time = now;
}
bool policy_check_time(Budget *b, int32_t now, bool reliable) {
  if (b->magic != MAGIC)
    policy_init(b, now);
  if (!reliable || now < b->last_time) {
    b->hold_until = now + DAY;
    b->last_time = now;
    return false;
  }
  b->last_time = now;
  return true;
}
bool policy_quiet(const Settings *s, int minute) {
  if (s->all_day)
    return true;
  if (s->quiet_start == s->quiet_end)
    return false;
  if (s->quiet_start < s->quiet_end)
    return minute >= s->quiet_start && minute < s->quiet_end;
  return minute >= s->quiet_start || minute < s->quiet_end;
}
bool policy_direct(const Settings *s, int32_t now, int minute,
                   bool system_quiet) {
  return s->joined && s->gentle && !system_quiet && !policy_quiet(s, minute) &&
         s->pause_until != -1 && s->pause_until <= now;
}
bool policy_incoming(const Settings *s, const Budget *b, int32_t now,
                     int minute, bool system_quiet) {
  if (!policy_direct(s, now, minute, system_quiet) || now < b->hold_until)
    return false;
  int count = 0;
  for (int i = 0; i < 4; i++) {
    if (b->incoming[i] > now)
      return false;
    if (b->incoming[i] && now - b->incoming[i] < DAY)
      count++;
    if (b->incoming[i] && now - b->incoming[i] < 5400)
      return false;
  }
  return count < 4;
}
void policy_reserve(Budget *b, int32_t now) {
  for (int i = 0; i < 3; i++)
    b->incoming[i] = b->incoming[i + 1];
  b->incoming[3] = now;
}
