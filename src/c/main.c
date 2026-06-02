#include <pebble.h>

static Window *s_window;
static Layer  *s_canvas_layer;
static GBitmap *s_tread_bmp;

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // White background
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, GCornerNone);

  // Draw tread exactly at 144x168
  graphics_context_set_compositing_mode(ctx, GCompOpAnd);
  graphics_draw_bitmap_in_rect(ctx, s_tread_bmp, GRect(0, 0, 144, 168));
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);

  s_canvas_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(root, s_canvas_layer);

  s_tread_bmp = gbitmap_create_with_resource(RESOURCE_ID_TREAD);
}

static void window_unload(Window *window) {
  gbitmap_destroy(s_tread_bmp);
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
