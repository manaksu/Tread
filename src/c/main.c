#include <pebble.h>

// AppMessage keys — must match CloudPebble Settings tab order (alphabetical)
#define KEY_BAT_STYLE   0
#define KEY_CLOCK_STYLE 1

// Persist keys
#define PERSIST_CLOCK_STYLE 100
#define PERSIST_BAT_STYLE   101

// Plate geometry
#define PLATE_X  4
#define PLATE_Y  5
#define PLATE_W  90
#define PLATE_H  60
#define PLATE_R  6
#define BLUE_W   20
#define BLUE_H   42

// Road position
#define ROAD_Y 68
#define ROAD_H 60

// --- Globals ---
static Window    *s_window;
static Layer     *s_canvas_layer;
static GBitmap   *s_tread_bmp;
static GBitmap   *s_map_bmp;
static GBitmap   *s_road_bmp;
static GBitmap   *s_road_ghost_bmp;
static GFont      s_font_28;
static GFont      s_font_20;
static int        s_clock_style = 0;
static int        s_bat_style   = 0;

// Time buffers updated by tick handler
static char s_hour_buf[3] = "00";
static char s_min_buf[3]  = "00";
static char s_time_buf[6] = "00:00";
static char s_ampm_buf[3] = "AM";

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

// --- Wheel ---
static void draw_wheel(GContext *ctx, int cx, int cy, int pct) {
  int r_tyre_outer = 22;
  int r_tyre_inner = 18;
  int r_rim_inner  = 16;
  int r_hub        = 4;

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(cx, cy), r_tyre_outer);

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

  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, GPoint(cx, cy), r_tyre_outer);
  graphics_draw_circle(ctx, GPoint(cx, cy), r_tyre_inner);
  graphics_draw_circle(ctx, GPoint(cx, cy), r_rim_inner);

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

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(cx, cy), r_hub);
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
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(cx, cy), 2);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(cx, cy), 1);
}

// --- Battery handler ---
static void battery_handler(BatteryChargeState state) {
  if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

// --- Canvas ---
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  BatteryChargeState bat = battery_state_service_peek();

  // 1. Map background
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_map_bmp, GRect(0, 0, 144, 168));

  // 2. Road battery (style 2)
  if (s_bat_style == 2) {
    // Vanishing road — top-right, same area as wheel
    // Road is 44x22, centered around (119,23) → top-left at (97,12)
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_road_ghost_bmp, GRect(97, 12, 44, 22));
    int road_w = (44 * bat.charge_percent) / 100;
    if (road_w > 0) {
      graphics_draw_bitmap_in_rect(ctx, s_road_bmp, GRect(97, 12, road_w, 22));
    }
  }

  // 3. Tread
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_tread_bmp, GRect(0, 112, 144, 56));

  // 4. Clock
  if (s_clock_style == 0) {
    // Digital — draw HH and MM directly
    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(ctx, s_hour_buf, s_font_28,
                       GRect(6, 2, 90, 38),
                       GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    graphics_draw_text(ctx, s_min_buf, s_font_28,
                       GRect(6, 38, 90, 38),
                       GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  } else {
    // Plate
    fill_rrect(ctx, GRect(PLATE_X, PLATE_Y, PLATE_W, PLATE_H), PLATE_R, GColorChromeYellow);
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
    // NL text
    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, "NL",
                       fonts_get_system_font(FONT_KEY_GOTHIC_09),
                       GRect(PLATE_X + 1, PLATE_Y + BLUE_H - 14, BLUE_W - 2, 13),
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    // AM/PM
    graphics_context_set_text_color(ctx, GColorBlack);
    graphics_draw_text(ctx, s_ampm_buf, s_font_20,
                       GRect(26, 6, 66, 24),
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    // HH:MM
    graphics_draw_text(ctx, s_time_buf, s_font_20,
                       GRect(26, 33, 66, 28),
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }

  // 5. Battery indicator
  if (s_bat_style == 0) {
    int bw = (40 * bat.charge_percent) / 100;
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(98, 6, 42, 12), 2, GCornersAll);
    graphics_context_set_fill_color(ctx, GColorBlack);
    if (bw > 0) graphics_fill_rect(ctx, GRect(99, 7, bw, 10), 1, GCornersAll);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, 1);
    graphics_draw_round_rect(ctx, GRect(98, 6, 42, 12), 2);
    graphics_fill_rect(ctx, GRect(140, 9, 3, 6), 1, GCornersAll);
  } else if (s_bat_style == 1) {
    draw_wheel(ctx, 119, 23, bat.charge_percent);
  }
}

// --- Tick ---
static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  strftime(s_time_buf, sizeof(s_time_buf), "%H:%M", tick_time);
  snprintf(s_ampm_buf, sizeof(s_ampm_buf), "%s", tick_time->tm_hour < 12 ? "AM" : "PM");
  strftime(s_hour_buf, sizeof(s_hour_buf), "%H", tick_time);
  strftime(s_min_buf,  sizeof(s_min_buf),  "%M", tick_time);
  if (s_canvas_layer) layer_mark_dirty(s_canvas_layer);
}

// --- AppMessage ---
static void inbox_received(DictionaryIterator *iter, void *ctx) {
  Tuple *t = dict_find(iter, KEY_CLOCK_STYLE);
  if (t) {
    s_clock_style = (int)t->value->int32;
    persist_write_int(PERSIST_CLOCK_STYLE, s_clock_style);
  }
  t = dict_find(iter, KEY_BAT_STYLE);
  if (t) {
    s_bat_style = (int)t->value->int32;
    persist_write_int(PERSIST_BAT_STYLE, s_bat_style);
  }
  layer_mark_dirty(s_canvas_layer);
}

// --- Window ---
static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);

  s_canvas_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(root, s_canvas_layer);

  s_tread_bmp      = gbitmap_create_with_resource(RESOURCE_ID_TREAD);
  s_map_bmp        = gbitmap_create_with_resource(RESOURCE_ID_MAP_BG);
  s_road_bmp       = gbitmap_create_with_resource(RESOURCE_ID_ROAD);
  s_road_ghost_bmp = gbitmap_create_with_resource(RESOURCE_ID_ROAD_GHOST);

  s_font_28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SPACE_GROTESK_MEDIUM_28));
  s_font_20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SPACE_GROTESK_MEDIUM_20));

  battery_state_service_subscribe(battery_handler);
  app_message_open(128, 64);
  app_message_register_inbox_received(inbox_received);

  s_clock_style = persist_read_int(PERSIST_CLOCK_STYLE);
  s_bat_style   = persist_read_int(PERSIST_BAT_STYLE);

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
  gbitmap_destroy(s_road_bmp);
  gbitmap_destroy(s_road_ghost_bmp);
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
