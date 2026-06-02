#include <pebble.h>

static Window    *s_window;
static Layer     *s_canvas_layer;
static GBitmap   *s_tread_bmp;
static TextLayer *s_hour_layer;
static TextLayer *s_min_layer;
static GFont      s_font;

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  static char hour_buf[3];
  static char min_buf[3];
  strftime(hour_buf, sizeof(hour_buf), "%H", tick_time);
  strftime(min_buf,  sizeof(min_buf),  "%M", tick_time);
  text_layer_set_text(s_hour_layer, hour_buf);
  text_layer_set_text(s_min_layer,  min_buf);
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_tread_bmp, GRect(0, 112, 144, 56));
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);

  s_canvas_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(root, s_canvas_layer);

  s_tread_bmp = gbitmap_create_with_resource(RESOURCE_ID_TREAD);
  s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_SPACE_GROTESK_MEDIUM_28));

  // HH — top left
  s_hour_layer = text_layer_create(GRect(6, 4, 80, 34));
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorBlack);
  text_layer_set_font(s_hour_layer, s_font);
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentLeft);
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_hour_layer));

  // MM — below HH
  s_min_layer = text_layer_create(GRect(6, 36, 80, 34));
  text_layer_set_background_color(s_min_layer, GColorClear);
  text_layer_set_text_color(s_min_layer, GColorBlack);
  text_layer_set_font(s_min_layer, s_font);
  text_layer_set_text_alignment(s_min_layer, GTextAlignmentLeft);
  layer_add_child(s_canvas_layer, text_layer_get_layer(s_min_layer));

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  handle_tick(t, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void window_unload(Window *window) {
  tick_timer_service_unsubscribe();
  fonts_unload_custom_font(s_font);
  gbitmap_destroy(s_tread_bmp);
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_min_layer);
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
