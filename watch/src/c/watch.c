#include "art.h"
#include "drop_outbox.h"
#include "gesture.h"
#include "glyphs.h"
#include "policy.h"
#include <pebble.h>
enum { K_SETTINGS = 1, K_STATE, K_DROP, K_ACK, K_RECEIPT, K_PREVIEW, K_READY };
enum { P_SETTINGS = 1, P_BUDGET };
static Window *window;
static Layer *canvas;
static Settings settings;
static Budget budget;
static AppTimer *prepare_timer, *visual_timer, *animation_timer, *expiry_timer;
static uint32_t bundle, expires, captured_source, pending_request;
static int32_t last_sync, last_preview;
static bool has_other, connected, snapshot = true, trusted_time, recent_echo;
static Gesture gesture;
static bool sampling;
static void accel(AccelData *samples, uint32_t count);
static void update_sampling(void) {
  if (settings.joined && !sampling) {
    gesture.have_sample = false;
    gesture.pair_peak = gesture.last_peak = 0;
    accel_data_service_subscribe(5, accel);
    accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);
    sampling = true;
  } else if (!settings.joined && sampling) {
    accel_data_service_unsubscribe();
    sampling = false;
  }
}
static int animation = 11, transient;
static bool persist_budget(void) {
  return persist_write_data(P_BUDGET, &budget, sizeof(budget)) ==
         sizeof(budget);
}
static int minute_now(time_t now) {
  struct tm *t = localtime(&now);
  return t->tm_hour * 60 + t->tm_min;
}
static uint64_t millis(void) {
  time_t seconds;
  uint16_t ms;
  time_ms(&seconds, &ms);
  return (uint64_t)seconds * 1000 + ms;
}
static void redraw(void) {
  if (canvas)
    layer_mark_dirty(canvas);
}
static void expire_bundle(void *data) {
  (void)data;
  expiry_timer = NULL;
  bundle = expires = 0;
  has_other = false;
  redraw();
}
static void clear_visual(void *data) {
  (void)data;
  visual_timer = NULL;
  bool unresolved = pending_request != 0;
  drop_outbox_stop();
  transient = unresolved ? 4 : 0;
  pending_request = 0;
  if (unresolved)
    visual_timer = app_timer_register(8000, clear_visual, NULL);
  redraw();
}
static void show(int state, int duration) {
  transient = state;
  if (visual_timer)
    app_timer_cancel(visual_timer);
  visual_timer = app_timer_register(duration, clear_visual, NULL);
  redraw();
}
static void send(int type, int key, uint32_t value) {
  DictionaryIterator *out;
  if (app_message_outbox_begin(&out) != APP_MSG_OK)
    return;
  dict_write_uint8(out, MESSAGE_KEY_Type, type);
  dict_write_uint32(out, key, value);
  if (type == K_ACK) {
    time_t now = time(NULL);
    dict_write_int32(out, MESSAGE_KEY_PauseUntil, settings.pause_until);
    dict_write_uint8(out, MESSAGE_KEY_Preview,
                     trusted_time && now >= budget.hold_until &&
                         policy_direct(&settings, now, minute_now(now),
                                       quiet_time_is_active()));
  }
  app_message_outbox_send();
}
static void animate(void *data) {
  (void)data;
  animation_timer = NULL;
  if (++animation < 12 && !settings.reduced)
    animation_timer = app_timer_register(120, animate, NULL);
  redraw();
}
static void vibrate(int kind) {
  time_t now = time(NULL);
  if (!trusted_time || now < budget.hold_until ||
      !policy_check_time(&budget, now, clock_is_timezone_set()) ||
      !policy_direct(&settings, now, minute_now(now), quiet_time_is_active()))
    return;
  if (kind != 0) {
    if (!policy_incoming(&settings, &budget, now, minute_now(now),
                         quiet_time_is_active()))
      return;
    policy_reserve(&budget, now);
    if (!persist_budget()) {
      trusted_time = false;
      return;
    }
  }
  static const uint32_t tick[] = {60}, ripple[] = {80, 120, 80},
                        echo[] = {60, 100, 60, 100, 60};
  VibePattern pattern = {.durations = kind == 0   ? tick
                                      : kind == 2 ? echo
                                                  : ripple,
                         .num_segments = kind == 0   ? 1
                                         : kind == 2 ? 5
                                                     : 3};
  gesture_suppress(&gesture, millis() + 2500);
  vibes_enqueue_custom_pattern(pattern);
}
static void drop_error(void) {
  // Keep the original deadline; retries must not extend this submission.
  transient = 4;
  redraw();
}
static void submit(void *data) {
  (void)data;
  prepare_timer = NULL;
  if (!settings.joined || !connected) {
    show(4, 8000);
    return;
  }
  budget.request_counter++;
  if (!budget.request_counter || !persist_budget()) {
    show(4, 8000);
    return;
  }
  pending_request = budget.request_counter;
  show(2, 120000);
  drop_outbox_start(pending_request, captured_source, drop_error);
}
static void double_tap(void) {
  if (!settings.joined)
    return;
  if (prepare_timer) {
    app_timer_cancel(prepare_timer);
    prepare_timer = NULL;
    transient = 0;
    redraw();
    return;
  }
  if (transient == 2 || pending_request)
    return;
  time_t now = time(NULL);
  captured_source = has_other && expires > (uint32_t)now ? bundle : 0;
  show(1, 2500);
  prepare_timer = app_timer_register(2000, submit, NULL);
}
static void accel(AccelData *samples, uint32_t count) {
  for (uint32_t i = 0; i < count; i++) {
    AccelData a = samples[i];
    if (settings.joined &&
        gesture_sample(&gesture, a.x, a.y, a.z, a.timestamp, a.did_vibrate))
      double_tap();
  }
}
static void pixel_text(GContext *ctx, const char *text, int y, int scale,
                       int width) {
  int length = strlen(text), text_width = 0;
  for (int i = 0; i < length; i++)
    text_width += (text[i] == ':' ? 3 : 6) * scale;
  text_width -= scale;
  int x = (width - text_width) / 2;
  for (int i = 0; i < length; i++) {
    const uint8_t *rows = pond_glyph(text[i]);
    int cols = text[i] == ':' ? 2 : 5;
    for (int r = 0; r < 7; r++)
      for (int c = 0; c < cols; c++)
        if (rows[r] & (1 << (cols - 1 - c)))
          graphics_fill_rect(ctx,
                             GRect(x + c * scale, y + r * scale, scale, scale),
                             0, GCornerNone);
    x += (cols + 1) * scale;
  }
}
static void draw(Layer *layer, GContext *ctx) {
  graphics_context_set_antialiased(ctx, false);
  GRect bounds = layer_get_bounds(layer);
  int w = bounds.size.w, h = bounds.size.h;
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char clock[6], date[10];
  strftime(clock, sizeof(clock), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);
  static const char *days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
  snprintf(date, sizeof(date), "%s %02d", days[t->tm_wday], t->tm_mday);
  graphics_context_set_fill_color(ctx, pond_background());
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  graphics_context_set_fill_color(ctx, pond_ink());
  int scale = w >= 260   ? 8
              : w >= 200 ? 7
                         : 5,
      top = PBL_IF_ROUND_ELSE(w >= 260 ? 36 : 22, 12);
  pixel_text(ctx, date, top, 2, w);
  pixel_text(ctx, clock, top + 27, scale, w);
  if (!clock_is_24h_style())
    pixel_text(ctx, t->tm_hour < 12 ? "AM" : "PM", top + 29 + scale * 7, 1, w);
  bool live = expires > (uint32_t)now && has_other;
  bool offline =
      !connected || (settings.joined && (!last_sync || now - last_sync > 120));
  int art_width = w >= 260 ? 180 : w >= 200 ? 150 : 116;
  int art_y = PBL_IF_ROUND_ELSE(w >= 260 ? 135 : 96, h >= 228 ? 108 : 80);
  int art_height = PBL_IF_ROUND_ELSE(w >= 260 ? 88 : 48, h >= 228 ? 86 : 60);
  if (!clock_is_24h_style() && !PBL_IF_ROUND_ELSE(true, false) && h < 228) {
    art_y += 12;
    art_height -= 12;
  }
  pond_draw(ctx, GRect((w - art_width) / 2, art_y, art_width, art_height),
            settings.shape, settings.palette, settings.seed, live, offline,
            settings.reduced ? 11 : animation, transient == 1);
  const char *status = "STILL";
  if (!settings.joined)
    status = "SETUP";
  else if (transient == 1)
    status = "DROP?";
  else if (transient == 2)
    status = "SENDING";
  else if (transient == 3)
    status = "DROPPED";
  else if (transient == 4)
    status = "UNKNOWN";
  else if (transient == 5)
    status = "RATE LIMIT";
  else if (transient == 6)
    status = "NOT SENT";
  else if (!connected || !last_sync || now - last_sync > 120)
    status = "OFFLINE";
  else if (settings.pause_until == -1 || settings.pause_until > now)
    status = "PAUSED";
  else if (policy_quiet(&settings, minute_now(now)))
    status = "QUIET";
  else if (live)
    status = recent_echo ? "ECHO" : "RIPPLE";
  graphics_context_set_fill_color(ctx, pond_ink());
  pixel_text(ctx, status, PBL_IF_ROUND_ELSE(h - 30, h - 24), 2, w);
}
static int32_t value(DictionaryIterator *iter, int key, int32_t fallback) {
  Tuple *tuple = dict_find(iter, key);
  return tuple && (tuple->type == TUPLE_INT || tuple->type == TUPLE_UINT)
             ? tuple->value->int32
             : fallback;
}
static void inbox(DictionaryIterator *iter, void *context) {
  (void)context;
  int type = value(iter, MESSAGE_KEY_Type, 0);
  time_t now = time(NULL);
  if (type == K_SETTINGS) {
    Settings next = settings;
    next.revision = value(iter, MESSAGE_KEY_Revision, 0);
    next.joined = value(iter, MESSAGE_KEY_Joined, 0) == 1;
    next.gentle = value(iter, MESSAGE_KEY_Gentle, 0) == 1;
    next.quiet_start = value(iter, MESSAGE_KEY_QuietStart, 1320);
    next.quiet_end = value(iter, MESSAGE_KEY_QuietEnd, 480);
    next.all_day = value(iter, MESSAGE_KEY_AllDay, 0) == 1;
    next.pause_until = value(iter, MESSAGE_KEY_PauseUntil, 0);
    next.reduced = value(iter, MESSAGE_KEY_Reduced, 0) == 1;
    next.tick = value(iter, MESSAGE_KEY_Tick, 0) == 1;
    next.seed = value(iter, MESSAGE_KEY_Seed, 1);
    next.shape = value(iter, MESSAGE_KEY_Shape, 0);
    next.palette = value(iter, MESSAGE_KEY_Palette, 0);
    if (!next.revision || next.quiet_start > 1439 || next.quiet_end > 1439 ||
        next.shape > 2 || next.palette > 1 || next.pause_until < -3)
      return;
    if (next.pause_until < -1) {
      if (next.revision == settings.revision)
        next.pause_until = settings.pause_until;
      else if (next.pause_until == -3)
        next.pause_until = now + 3600;
      else {
        struct tm *local = localtime(&now);
        next.pause_until =
            clock_to_timestamp((WeekDay)((local->tm_wday + 1) % 7 + 1), 8, 0);
      }
    }
    if (persist_write_data(P_SETTINGS, &next, sizeof(next)) != sizeof(next))
      return;
    settings = next;
    update_sampling();
    if (!settings.joined) {
      drop_outbox_stop();
      bundle = expires = 0;
      pending_request = 0;
      transient = 0;
      if (prepare_timer) {
        app_timer_cancel(prepare_timer);
        prepare_timer = NULL;
      }
    }
    send(K_ACK, MESSAGE_KEY_Applied, settings.revision);
  } else if (type == K_STATE) {
    int32_t server = value(iter, MESSAGE_KEY_ServerTime, 0);
    bool reliable = clock_is_timezone_set() && server > 0 &&
                    abs((int)(now - server)) <= 120;
    if (budget.last_time && now - budget.last_time > 120)
      budget.hold_until = now + 86400;
    trusted_time = policy_check_time(&budget, now, reliable);
    last_sync = now;
    uint32_t incoming = value(iter, MESSAGE_KEY_Bundle, 0),
             expiry = value(iter, MESSAGE_KEY_Expires, 0);
    bool fresh = incoming > budget.last_bundle && expiry > (uint32_t)now;
    bool quiet = snapshot || value(iter, MESSAGE_KEY_Snapshot, 1) == 1;
    if (incoming && expiry > (uint32_t)now && (fresh || quiet)) {
      if (expiry_timer)
        app_timer_cancel(expiry_timer);
      expiry_timer =
          app_timer_register((expiry - now) * 1000, expire_bundle, NULL);
      bundle = incoming;
      expires = expiry;
      has_other = value(iter, MESSAGE_KEY_Other, 0) == 1;
      recent_echo = value(iter, MESSAGE_KEY_Echo, 0) == 1;
      if (fresh)
        budget.last_bundle = incoming;
      bool saved = !fresh || persist_budget();
      if (fresh && !quiet && has_other) {
        animation = 0;
        if (animation_timer)
          app_timer_cancel(animation_timer);
        animation_timer = NULL;
        if (!settings.reduced)
          animation_timer = app_timer_register(120, animate, NULL);
        if (saved)
          vibrate(recent_echo ? 2 : 1);
      }
    }
    snapshot = false;
    send(K_ACK, MESSAGE_KEY_Applied, settings.revision);
  } else if (type == K_RECEIPT) {
    uint32_t request = value(iter, MESSAGE_KEY_Request, 0);
    if (request && request == pending_request) {
      // Even an unknown receipt proves the phone has taken over this request.
      drop_outbox_stop();
      int result = value(iter, MESSAGE_KEY_Result, 0);
      if (result == 1) {
        show(3, 4000);
        if (settings.tick && !snapshot)
          vibrate(0);
        pending_request = 0;
      } else if (result == 2) {
        show(3, 4000);
        pending_request = 0;
      } else if (result == 0) {
        // The phone is still retrying. Preserve the original deadline.
        transient = 2;
      } else {
        show(result == 4 ? 5 : result == 5 ? 6 : 4, 8000);
        pending_request = 0;
      }
    }
  } else if (type == K_PREVIEW && now - last_preview >= 10) {
    last_preview = now;
    vibrate(0);
  }
  redraw();
}
static void connection(bool up) {
  drop_outbox_stop();
  if (pending_request)
    drop_error();
  connected = up;
  snapshot = true;
  trusted_time = false;
  bundle = expires = 0;
  last_sync = 0;
  redraw();
}
static void tick(struct tm *tm, TimeUnits units) {
  (void)tm;
  (void)units;
  time_t now = time(NULL);
  if (budget.last_time &&
      (now < budget.last_time || now - budget.last_time > 120))
    budget.hold_until = now + 86400;
  policy_check_time(&budget, now, clock_is_timezone_set());
  persist_budget();
  redraw();
}
int main(void) {
  settings = (Settings){.quiet_start = 1320, .quiet_end = 480, .seed = 1};
  if (persist_read_data(P_SETTINGS, &settings, sizeof(settings)) !=
      sizeof(settings))
    settings = (Settings){
        .quiet_start = 1320, .quiet_end = 480, .seed = (uint32_t)time(NULL)};
  if (persist_read_data(P_BUDGET, &budget, sizeof(budget)) != sizeof(budget))
    policy_init(&budget, time(NULL));
  policy_check_time(&budget, time(NULL), clock_is_timezone_set());
  persist_budget();
  window = window_create();
  canvas = layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_set_update_proc(canvas, draw);
  layer_add_child(window_get_root_layer(window), canvas);
  window_stack_push(window, true);
  app_message_register_inbox_received(inbox);
  app_message_register_outbox_sent(drop_outbox_sent);
  app_message_register_outbox_failed(drop_outbox_failed);
  app_message_open(256, 128);
  connected = connection_service_peek_pebble_app_connection();
  connection_service_subscribe(
      (ConnectionHandlers){.pebble_app_connection_handler = connection});
  tick_timer_service_subscribe(MINUTE_UNIT, tick);
  update_sampling();
  app_event_loop();
  drop_outbox_stop();
  persist_budget();
  accel_data_service_unsubscribe();
  tick_timer_service_unsubscribe();
  connection_service_unsubscribe();
  layer_destroy(canvas);
  window_destroy(window);
}
