#include "../src/c/drop_outbox.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static DictionaryIterator outbox;
static AppTimer timer;
static AppMessageResult begin_result, send_result;
static unsigned begins, sends, errors;
static bool timer_available = true;

AppTimer *app_timer_register(uint32_t ms, void (*callback)(void *),
                             void *data) {
  assert(ms == 1000);
  assert(!timer.active);
  if (!timer_available)
    return NULL;
  timer = (AppTimer){callback, data, true};
  return &timer;
}
bool app_timer_cancel(AppTimer *t) {
  t->active = false;
  return true;
}
AppMessageResult app_message_outbox_begin(DictionaryIterator **out) {
  begins++;
  if (begin_result != APP_MSG_OK)
    return begin_result;
  memset(&outbox, 0, sizeof(outbox));
  *out = &outbox;
  return APP_MSG_OK;
}
AppMessageResult app_message_outbox_send(void) {
  sends++;
  return send_result;
}
void dict_write_uint32(DictionaryIterator *out, uint32_t key, uint32_t value) {
  out->values[key].uint32 = value;
  out->tuples[key].value = &out->values[key];
}
void dict_write_uint8(DictionaryIterator *out, uint32_t key, uint8_t value) {
  dict_write_uint32(out, key, value);
}
Tuple *dict_find(const DictionaryIterator *iter, uint32_t key) {
  return key < 4 && iter->tuples[key].value ? (Tuple *)&iter->tuples[key]
                                            : NULL;
}
static void error(void) { errors++; }
static void run_timer(void) {
  assert(timer.active);
  timer.active = false;
  timer.callback(timer.data);
}
static void reset(void) {
  drop_outbox_stop();
  begins = sends = errors = 0;
  begin_result = send_result = APP_MSG_OK;
  timer_available = true;
}
static void assert_drop(uint32_t id, uint32_t source) {
  assert(outbox.values[MESSAGE_KEY_Type].uint32 == 3);
  assert(outbox.values[MESSAGE_KEY_Request].uint32 == id);
  assert(outbox.values[MESSAGE_KEY_Source].uint32 == source);
}
int main(void) {
  reset();
  begin_result = APP_MSG_BUSY;
  drop_outbox_start(77, 31, error);
  assert(errors == 1 && sends == 0 && timer.active);
  begin_result = APP_MSG_OK;
  run_timer();
  assert_drop(77, 31);
  assert(begins == 2 && sends == 1);
  drop_outbox_sent(&outbox, NULL);
  drop_outbox_failed(&outbox, APP_MSG_SEND_TIMEOUT, NULL);
  assert(!timer.active); // Late callbacks cannot resurrect delivered work.

  reset();
  send_result = APP_MSG_BUSY;
  drop_outbox_start(55, 12, error);
  assert(errors == 1 && timer.active);
  send_result = APP_MSG_OK;
  run_timer();
  assert_drop(55, 12);
  assert(sends == 2);
  drop_outbox_failed(&outbox, APP_MSG_SEND_TIMEOUT, NULL);
  assert(timer.active && errors == 2);
  run_timer();
  assert_drop(55, 12);
  drop_outbox_sent(&outbox, NULL);
  assert(!timer.active);

  reset();
  drop_outbox_start(88, 44, error);
  dict_write_uint32(&outbox, MESSAGE_KEY_Type, 4); // Unrelated settings ACK.
  drop_outbox_failed(&outbox, APP_MSG_SEND_TIMEOUT, NULL);
  assert(errors == 0 && !timer.active);

  reset();
  begin_result = APP_MSG_BUSY;
  drop_outbox_start(99, 0, error);
  while (timer.active)
    run_timer();
  assert(begins == 8 && errors == 8 && sends == 0);

  reset();
  send_result = APP_MSG_BUSY;
  drop_outbox_start(100, 4, error);
  assert(timer.active);
  drop_outbox_stop(); // Leave, disconnect, receipt or original watch deadline.
  drop_outbox_failed(&outbox, APP_MSG_SEND_TIMEOUT, NULL);
  assert(!timer.active && sends == 1);

  reset();
  timer_available = false;
  begin_result = APP_MSG_BUSY;
  drop_outbox_start(101, 0, error);
  assert(errors == 1 && !timer.active);
  puts("Drop outbox: passed (busy, send failure, async retry, same ID/source, "
       "stop, attempt cap)");
}
