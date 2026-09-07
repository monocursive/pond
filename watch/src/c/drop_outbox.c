#include "drop_outbox.h"

static uint32_t request_id, source_bundle;
static unsigned attempts;
static AppTimer *retry_timer;
static void (*report_error)(void);
static void attempt(void *data);

void drop_outbox_stop(void) {
  if (retry_timer)
    app_timer_cancel(retry_timer);
  retry_timer = NULL;
  request_id = 0;
}

static void retry(void) {
  if (report_error)
    report_error();
  if (attempts >= 8) {
    drop_outbox_stop();
    return;
  }
  if (!retry_timer)
    retry_timer = app_timer_register(1000, attempt, NULL);
  if (!retry_timer)
    drop_outbox_stop();
}

static void attempt(void *data) {
  (void)data;
  retry_timer = NULL;
  if (!request_id)
    return;
  attempts++;
  DictionaryIterator *out;
  if (app_message_outbox_begin(&out) != APP_MSG_OK) {
    retry();
    return;
  }
  dict_write_uint8(out, MESSAGE_KEY_Type, 3);
  dict_write_uint32(out, MESSAGE_KEY_Request, request_id);
  dict_write_uint32(out, MESSAGE_KEY_Source, source_bundle);
  if (app_message_outbox_send() != APP_MSG_OK)
    retry();
}

void drop_outbox_start(uint32_t request, uint32_t source,
                       void (*on_error)(void)) {
  drop_outbox_stop();
  request_id = request;
  source_bundle = source;
  report_error = on_error;
  attempts = 0;
  attempt(NULL);
}

static bool matches(DictionaryIterator *iter) {
  Tuple *type = dict_find(iter, MESSAGE_KEY_Type);
  Tuple *id = dict_find(iter, MESSAGE_KEY_Request);
  return request_id && type && id && type->value->uint8 == 3 &&
         id->value->uint32 == request_id;
}

void drop_outbox_sent(DictionaryIterator *iter, void *context) {
  (void)context;
  if (matches(iter))
    drop_outbox_stop();
}

void drop_outbox_failed(DictionaryIterator *iter, AppMessageResult reason,
                        void *context) {
  (void)reason;
  (void)context;
  if (matches(iter))
    retry();
}
