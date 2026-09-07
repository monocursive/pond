#pragma once
// Minimal transport surface for host tests of the real drop outbox.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef enum {
  APP_MSG_OK = 0,
  APP_MSG_BUSY = 64,
  APP_MSG_SEND_TIMEOUT = 2
} AppMessageResult;
enum { MESSAGE_KEY_Type = 1, MESSAGE_KEY_Request, MESSAGE_KEY_Source };
typedef union {
  uint8_t uint8;
  uint32_t uint32;
} TupleValue;
typedef struct {
  TupleValue *value;
} Tuple;
typedef struct {
  TupleValue values[4];
  Tuple tuples[4];
} DictionaryIterator;
typedef struct {
  void (*callback)(void *);
  void *data;
  bool active;
} AppTimer;
AppTimer *app_timer_register(uint32_t ms, void (*callback)(void *), void *data);
bool app_timer_cancel(AppTimer *timer);
AppMessageResult app_message_outbox_begin(DictionaryIterator **out);
AppMessageResult app_message_outbox_send(void);
void dict_write_uint8(DictionaryIterator *out, uint32_t key, uint8_t value);
void dict_write_uint32(DictionaryIterator *out, uint32_t key, uint32_t value);
Tuple *dict_find(const DictionaryIterator *iter, uint32_t key);
