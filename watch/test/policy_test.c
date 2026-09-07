#include "../src/c/policy.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
  int now = 200000;
  Settings s = {
      .joined = 1, .gentle = 1, .quiet_start = 1320, .quiet_end = 480};
  Budget b;
  policy_init(&b, now);
  assert(!policy_incoming(&s, &b, now, 720, false));
  now += 86400;
  assert(policy_check_time(&b, now, true));
  assert(policy_incoming(&s, &b, now, 720, false));
  assert(!policy_incoming(&s, &b, now, 1320, false));
  assert(!policy_incoming(&s, &b, now, 479, false));
  assert(policy_incoming(&s, &b, now, 480, false));
  assert(!policy_incoming(&s, &b, now, 720, true));
  s.gentle = 0;
  assert(!policy_incoming(&s, &b, now, 720, false));
  s.gentle = 1;
  s.pause_until = -1;
  assert(!policy_incoming(&s, &b, now, 720, false));
  s.pause_until = 0;
  s.all_day = 1;
  assert(!policy_direct(&s, now, 720, false));
  s.all_day = 0;
  for (int i = 0; i < 4; i++) {
    assert(policy_incoming(&s, &b, now, 720, false));
    policy_reserve(&b, now);
    assert(!policy_incoming(&s, &b, now + 5399, 720, false));
    now += 5400;
  }
  assert(!policy_incoming(&s, &b, now, 720, false));
  Budget restarted = b;
  assert(!policy_incoming(&s, &restarted, now, 720, false));
  assert(policy_incoming(&s, &b, b.incoming[0] + 86400, 720, false));
  assert(!policy_check_time(&b, now - 100000, true));
  assert(!policy_incoming(&s, &b, now - 100000, 720, false));
  assert(!policy_check_time(&b, now, false));
  assert(!policy_incoming(&s, &b, now, 720, false));
  s.quiet_start = 600;
  s.quiet_end = 660;
  assert(policy_quiet(&s, 600));
  assert(!policy_quiet(&s, 660));
  s.quiet_start = s.quiet_end;
  assert(!policy_quiet(&s, 600));
  puts("Comfort policy: passed (quiet hours, pause, daily cap, spacing, "
       "restart, clock trust)");
}
