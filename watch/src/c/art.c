#include "art.h"
#include "art-data.h"

GColor pond_background(void) {
  return PBL_IF_COLOR_ELSE(GColorFromHEX(0x005555), GColorBlack);
}

GColor pond_ink(void) {
  return PBL_IF_COLOR_ELSE(GColorFromHEX(0xFFFFAA), GColorWhite);
}

void pond_draw(GContext *ctx, GRect frame, uint8_t shape, uint8_t palette,
               uint32_t seed, bool live, bool offline, int animation,
               bool lifted) {
  GColor colours[] = {PBL_IF_COLOR_ELSE(GColorFromHEX(0x55AAAA), GColorWhite),
                      pond_ink(),
                      PBL_IF_COLOR_ELSE(palette ? GColorFromHEX(0x55AAAA)
                                                : GColorFromHEX(0xFFAA55),
                                        GColorWhite),
                      PBL_IF_COLOR_ELSE(palette ? GColorFromHEX(0x00AAAA)
                                                : GColorFromHEX(0xAAAA55),
                                        GColorWhite),
                      pond_background()};
  GPoint points[64];
  for (unsigned i = 0; i < sizeof(pond_paths) / sizeof(pond_paths[0]); ++i) {
    const PondPath *source = &pond_paths[i];
    bool water = source->paint == 0 || (source->paint == 1 && source->stroke);
    if ((source->paint == 0 && !live) || (water && offline))
      continue;
    for (unsigned j = 0; j < source->count; ++j) {
      int x = source->points[j].x, y = source->points[j].y;
      if (!water) {
        if (shape == 1)
          y = 92 + (y - 92) * 4 / 5;
        if (shape == 2)
          x = 72 + (x - 72) * 4 / 5 + (70 - y) / 6;
        x += (y - 64) * ((int)(seed % 3) - 1) / 16;
      } else if (source->paint == 0) {
        x = 72 + (x - 72) * (70 + animation * 3) / 103;
      }
      points[j] = GPoint(frame.origin.x + x * frame.size.w / 144,
                         frame.origin.y + (y - 30) * frame.size.h / 96 -
                             (!water && lifted ? 5 : 0));
    }
    // Synchronous draw only: the path and transformed points live on this
    // stack.
    GPath path = {.num_points = source->count, .points = points};
    if (source->closed) {
      graphics_context_set_fill_color(ctx, colours[source->paint]);
      gpath_draw_filled(ctx, &path);
    } else {
      int width = source->stroke * frame.size.w / 144;
      if (width < 1)
        width = 1;
      graphics_context_set_stroke_color(ctx, colours[source->paint]);
      graphics_context_set_stroke_width(ctx, width);
      gpath_draw_outline_open(ctx, &path);
      // Rounded line caps keep the small eyes and loose water marks friendly.
      if (width >= 2) {
        graphics_context_set_fill_color(ctx, colours[source->paint]);
        graphics_fill_circle(ctx, points[0], width / 2);
        graphics_fill_circle(ctx, points[source->count - 1], width / 2);
      }
    }
  }
  graphics_context_set_stroke_width(ctx, 1);
  if (offline) {
    int cx = frame.origin.x + frame.size.w / 2;
    int y = frame.origin.y + frame.size.h * 4 / 5;
    graphics_context_set_stroke_color(ctx, pond_ink());
    graphics_draw_line(ctx, GPoint(cx - 28, y), GPoint(cx - 8, y));
    graphics_draw_line(ctx, GPoint(cx + 8, y), GPoint(cx + 28, y));
  }
}
