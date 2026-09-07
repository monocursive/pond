#pragma once
#include <pebble.h>

// One live drop, at most eight transport attempts. The watch owns its deadline.
void drop_outbox_start(uint32_t request, uint32_t source,
                       void (*on_error)(void));
void drop_outbox_stop(void);
void drop_outbox_sent(DictionaryIterator *iter, void *context);
void drop_outbox_failed(DictionaryIterator *iter, AppMessageResult reason,
                        void *context);
