#include <pebble.h>

// Keys
#define KEY_CLOCK_STYLE 0

// Plate geometry
#define PLATE_X  4
#define PLATE_Y  5
#define PLATE_W  90
#define PLATE_H  60
#define PLATE_R  6
#define BLUE_W   20
#define BLUE_H   42

// Jerry can geometry — top-right corner, origin at (ox, oy)
// Body: 42w x 46h, scaled from SVG

// --- Motorcycle wheel battery indicator ---
static void draw_wheel(GContext *ctx, int cx, int cy, int pct) {
  int r_tyre_outer = 22;
  int r_tyre_inner = 18;
  int r_rim_inner  = 16;
  int r_hub        = 4;

  // White background
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(cx, cy), r_tyre_outer);

  // Tyre fill clockwise from top
  if (pct > 0) {
    int32_t sweep = DEG_TO_TRIGANGLE(360 * pct / 100);
    graphics_context_set_fill_color(ctx, GColorBlack);
    for (int32_t a = DEG_TO_TRIGANGLE(-90);
         a < DEG_TO_TRIGANGLE(-90) + sweep;
         a += DEG_TO_TRIGANGLE(1)) {
      int x = cx + (r_tyre_outer * cos_lookup(a) / TRIG_MAX_RATIO);
      int y = cy + (r_tyre_outer * sin_lookup(a) / TRIG_MAX_RATIO);
      graphics_draw_line(ctx, GPoint(cx, cy), GPoint(x, y));
    }
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_circle(ctx, GPoint(cx, cy), r_tyre_inner);
  }

  // Tyre + rim outlines
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, GPoint(cx, cy), r_tyre_outer);
  graphics_draw_circle(ctx, GPoint(cx, cy), r_tyre_inner);
  graphics_draw_circle(ctx, GPoint(cx, cy), r_rim_inner);

  // 32 spokes
  graphics_draw_line(ctx, GPoint(cx+0, cy-3),  GPoint(cx+11, cy-11));
  graphics_draw_line(ctx, GPoint(cx+1, cy-3),  GPoint(cx-11, cy-11));
  graphics_draw_line(ctx, GPoint(cx+1, cy-3),  GPoint(cx+15, cy-6));
  graphics_draw_line(ctx, GPoint(cx+2, cy-2),  GPoint(cx-6,  cy-15));
  graphics_draw_line(ctx, GPoint(cx+2, cy-2),  GPoint(cx+16, cy+0));
  graphics_draw_line(ctx, GPoint(cx+2, cy-2),  GPoint(cx+0,  cy-16));
  graphics_draw_line(ctx, GPoint(cx+3, cy-1),  GPoint(cx+15, cy+6));
  graphics_draw_line(ctx, GPoint(cx+3, cy-1),  GPoint(cx+6,  cy-15));
  graphics_draw_line(ctx, GPoint(cx+3, cy+0),  GPoint(cx+11, cy+11));
  graphics_draw_line(ctx, GPoint(cx+3, cy+1),  GPoint(cx+11, cy-11));
  graphics_draw_line(ctx, GPoint(cx+3, cy+1),  GPoint(cx+6,  cy+15));
  graphics_draw_line(ctx, GPoint(cx+2, cy+2),  GPoint(cx+15, cy-6));
  graphics_draw_line(ctx, GPoint(cx+2, cy+2),  GPoint(cx+0,  cy+16));
  graphics_draw_line(ctx, GPoint(cx+2, cy+2),  GPoint(cx+16, cy+0));
  graphics_draw_line(ctx, GPoint(cx+1, cy+3),  GPoint(cx-6,  cy+15));
  graphics_draw_line(ctx, GPoint(cx+1, cy+3),  GPoint(cx+15, cy+6));
  graphics_draw_line(ctx, GPoint(cx+0, cy+3),  GPoint(cx-11, cy+11));
  graphics_draw_line(ctx, GPoint(cx-1, cy+3),  GPoint(cx+11, cy+11));
  graphics_draw_line(ctx, GPoint(cx-1, cy+3),  GPoint(cx-15, cy+6));
  graphics_draw_line(ctx, GPoint(cx-2, cy+2),  GPoint(cx+6,  cy+15));
  graphics_draw_line(ctx, GPoint(cx-2, cy+2),  GPoint(cx-16, cy+0));
  graphics_draw_line(ctx, GPoint(cx-2, cy+2),  GPoint(cx+0,  cy+16));
  graphics_draw_line(ctx, GPoint(cx-3, cy+1),  GPoint(cx-15, cy-6));
  graphics_draw_line(ctx, GPoint(cx-3, cy+1),  GPoint(cx-6,  cy+15));
  graphics_draw_line(ctx, GPoint(cx-3, cy+0),  GPoint(cx-11, cy-11));
  graphics_draw_line(ctx, GPoint(cx-3, cy-1),  GPoint(cx-11, cy+11));
  graphics_draw_line(ctx, GPoint(cx-3, cy-1),  GPoint(cx-6,  cy-15));
  graphics_draw_line(ctx, GPoint(cx-2, cy-2),  GPoint(cx-15, cy+6));
  graphics_draw_line(ctx, GPoint(cx-2, cy-2),  GPoint(cx+0,  cy-16));
  graphics_draw_line(ctx, GPoint(cx-2, cy-2),  GPoint(cx-16, cy+0));
  graphics_draw_line(ctx, GPoint(cx-1, cy-3),  GPoint(cx+6,  cy-15));
  graphics_draw_line(ctx, GPoint(cx-1, cy-3),  GPoint(cx-15, cy-6));

  // Hub
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(cx, cy), r_hub);

  // Bolt dots
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(cx+0,  cy-3), 1);
  graphics_fill_circle(ctx, GPoint(cx+1,  cy-3), 1);
  graphics_fill_circle(ctx, GPoint(cx+3,  cy-2), 1);
  graphics_fill_circle(ctx, GPoint(cx+3,  cy+0), 1);
  graphics_fill_circle(ctx, GPoint(cx+3,  cy+1), 1);
  graphics_fill_circle(ctx, GPoint(cx+1,  cy+3), 1);
  graphics_fill_circle(ctx, GPoint(cx+0,  cy+3), 1);
  graphics_fill_circle(ctx, GPoint(cx-2,  cy+3), 1);
  graphics_fill_circle(ctx, GPoint(cx-3,  cy+2), 1);
  graphics_fill_circle(ctx, GPoint(cx-3,  cy+0), 1);
  graphics_fill_circle(ctx, GPoint(cx-3,  cy-2), 1);
  graphics_fill_circle(ctx, GPoint(cx-2,  cy-3), 1);

  // Center cap
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(cx, cy), 2);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(cx, cy), 1);
}

static Window    *s_window;
static Layer     *s_canvas_layer;
static GBitmap   *s_tread_bmp;
static GBitmap   *s_map_bmp;
static TextLayer *s_time_layer;
static TextLayer *s_hour_layer;
static TextLayer *s_min_layer;
static TextLayer *s_ampm_layer;
static TextLayer *s_nl_layer;
static GFont      s_font_28;
static GFont      s_font_20;
static int        s_clock_style = 0;





// --- Drawing helpers ---

static void fill_rrect(GContext *ctx, GRect r, int radius, GColor color) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, GRect(r.origin.x + radius, r.origin.y, r.size.w - 2*radius, r.size.h), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(r.origin.x, r.origin.y + radius, r.size.w, r.size.h - 2*radius), 0, GCornerNone);
  graphics_fill_circle(ctx, GPoint(r.origin.x + radius,            r.origin.y + radius),            radius);
  graphics_fill_circle(ctx, GPoint(r.origin.x + r.size.w - radius, r.origin.y + radius),            radius);
  graphics_fill_circle(ctx, GPoint(r.origin.x + radius,            r.origin.y + r.size.h - radius), radius);
  graphics_fill_circle(ctx, GPoint(r.origin.x + r.size.w - radius, r.origin.y + r.size.h - radius), radius);
}

static void draw_stars(GContext *ctx, int cx, int cy, int r) {
  graphics_context_set_fill_color(ctx, GColorChromeYellow);
  for (int i = 0; i < 12; i++) {
    int32_t angle = DEG_TO_TRIGANGLE(i * 30 - 90);
    int x = cx + (r * cos_lookup(angle) / TRIG_MAX_RATIO);
    int y = cy + (r * sin_lookup(angle) / TRIG_MAX_RATIO);
    graphics_fill_circle(ctx, GPoint(x, y), 1);
  }
}

static void draw_stud(GContext *ctx, int cx, int cy) {
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_circle(ctx, GPoint(cx, cy), 3);
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_circle(ctx, GPoint(cx, cy), 1);
}

static void battery_handler(BatteryChargeState state) {
  if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

// --- Layers visibility ---

static void update_layers(void) {
  bool digital = (s_clock_style == 0);
  bool plate   = (s_clock_style == 1);
  layer_set_hidden(text_layer_get_layer(s_hour_layer),  !digital);
  layer_set_hidden(text_layer_get_layer(s_min_layer),   !digital);
  layer_set_hidden(text_layer_get_layer(s_time_layer),  !plate);
  layer_set_hidden(text_layer_get_layer(s_ampm_layer),  !plate);
  layer_set_hidden(text_layer_get_layer(s_nl_layer),    !plate);
}

// --- Canvas ---

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // Map background — drawn first, everything on top
  graphics_context_set_compositing_mode(ctx, GCompOpNormal);
  graphics_draw_bitmap_in_rect(ctx, s_map_bmp, GRect(0, 0, 144, 168));

  if (s_clock_style == 1) {
    // Yellow plate
    fill_rrect(ctx, GRect(PLATE_X, PLATE_Y, PLATE_W, PLATE_H), PLATE_R, GColorChromeYellow);
    // Blue top-left corner
    graphics_context_set_fill_color(ctx, GColorCobaltBlue);
    graphics_fill_circle(ctx, GPoint(PLATE_X + PLATE_R, PLATE_Y + PLATE_R), PLATE_R);
    graphics_fill_rect(ctx, GRect(PLATE_X, PLATE_Y + PLATE_R, BLUE_W, BLUE_H - PLATE_R), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(PLATE_X + PLATE_R, PLATE_Y, BLUE_W - PLATE_R, PLATE_R), 0, GCornerNone);
    draw_stars(ctx, PLATE_X + BLUE_W/2, PLATE_Y + 14, 6);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_round_rect(ctx, GRect(PLATE_X, PLATE_Y, PLATE_W, PLATE_H), PLATE_R);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_line(ctx, GPoint(PLATE_X + BLUE_W, PLATE_Y + 1),
                            GPoint(PLATE_X + BLUE_W, PLATE_Y + BLUE_H));
    draw_stud(ctx, 40, 37);
    draw_stud(ctx, 80, 37);
  }

  // Wheel battery indicator — top-right, center at (124, 38)
  BatteryChargeState bat = battery_state_service_peek();
  draw_wheel(ctx, 119, 23, bat.charge_percent);

  // Tread
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_tread_bmp, GRect(0, 112, 144, 56));
}

// --- Tick ---

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  static char time_buf[6];
  static char ampm_buf[3];
  static char hour_buf[3];
  static char min_buf[3];
  strftime(time_buf, sizeof(time_buf), "%H:%M", tick_time);
  snprintf(ampm_buf, sizeof(ampm_buf), "%s", tick_time->tm_hour < 12 ? "AM" : "PM");
  strftime(hour_buf, sizeof(hour_buf), "%H", tick_time);
  strftime(min_buf,  sizeof(min_buf),  "%M", tick_time);
  text_layer_set_text(s_time_layer, time_buf);
  text_layer_set_text(s_ampm_layer, ampm_buf);
  text_layer_set_text(s_hour_layer, hour_buf);
  text_layer_set_text(s_min_layer,  min_buf);
}

// --- AppMessage ---

static void inbox_received(DictionaryIterator *iter, void *ctx) {
  Tuple *t = dict_find(iter, KEY_CLOCK_STYLE);
  if (t) {
    s_clock_style = (int)t->value->int32;
    persist_write_int(KEY_CLOCK_STYLE, s_clock_style);
    update_layers();
    layer_mark_dirty(s_canvas_layer);
  }
}

// --- Window ---

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);

  s_canvas_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(root, s_canvas_layer);

  s_tread_bmp = gbitmap_create_with_resource(RESOURCE_ID_TREAD);
  s_map_bmp   = gbitmap_create_with_resource(RESOURCE_ID_MAP_BG);
  s_font_28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SPACE_GROTESK_MEDIUM_28));
  s_font_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SPACE_GROTESK_MEDIUM_20));

  // Digital layers
  s_hour_layer = text_layer_create(GRect(6, 2, 90, 38));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorBlack);
  text_layer_set_font(s_hour_layer, s_font_28);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentLeft);
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_hour_layer));

  s_min_layer = text_layer_create(GRect(6, 38, 90, 38));
  text_layer_set_background_color(s_min_layer, GColorClear);
  text_layer_set_text_color(s_min_layer, GColorBlack);
  text_layer_set_font(s_min_layer, s_font_28);
  text_layer_set_text_alignment(s_min_layer, GTextAlignmentLeft);
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_min_layer));

  // Plate layers
  s_ampm_layer = text_layer_create(GRect(26, 6, 66, 24));
  text_layer_set_background_color(s_ampm_layer, GColorClear);
  text_layer_set_text_color(s_ampm_layer, GColorBlack);
  text_layer_set_font(s_ampm_layer, s_font_20);
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentCenter);
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_ampm_layer));

  s_time_layer = text_layer_create(GRect(26, 33, 66, 28));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_font_20);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_time_layer));

  s_nl_layer = text_layer_create(GRect(PLATE_X + 1, PLATE_Y + BLUE_H - 14, BLUE_W - 2, 13));
  text_layer_set_background_color(s_nl_layer, GColorClear);
  text_layer_set_text_color(s_nl_layer, GColorWhite);
  text_layer_set_font(s_nl_layer, fonts_get_system_font(FONT_KEY_GOTHIC_09));
  text_layer_set_text_alignment(s_nl_layer, GTextAlignmentCenter);
  text_layer_set_text(s_nl_layer, "NL");
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_nl_layer));

  // Battery
  battery_state_service_subscribe(battery_handler);

  // AppMessage
  app_message_open(128, 64);
  app_message_register_inbox_received(inbox_received);

  // Restore + init
  s_clock_style = persist_read_int(KEY_CLOCK_STYLE);
  update_layers();
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  handle_tick(t, MINUTE_UNIT);
}

static void window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  fonts_unload_custom_font(s_font_28);
  fonts_unload_custom_font(s_font_20);
  gbitmap_destroy(s_tread_bmp);
  gbitmap_destroy(s_map_bmp);
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_min_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_ampm_layer);
  text_layer_destroy(s_nl_layer);
  layer_destroy(s_canvas_layer);
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

static void deinit(void) {
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
