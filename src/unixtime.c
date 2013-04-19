/*

   How to use a custom non-system font.

 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

// The auto-generated header file with resource handle definitions
#include "resource_ids.auto.h"


#define MY_UUID {0x45, 0xC2, 0xED, 0x1A, 0x78, 0x09, 0x4C, 0xFE, 0x91, 0x44, 0x08, 0x17, 0x33, 0x53, 0xAA, 0xAD}
PBL_APP_INFO(MY_UUID, "Midnight Secs", "Overpunch", 1 /* App version */, 0,
 INVALID_RESOURCE, APP_INFO_WATCH_FACE);


Window window;

Layer bg_layer;

TextLayer text_layer;
TextLayer subtext_layer;
GFont large_font, small_font;

void itoa(int value, char *sp, int radix)
{
    char tmp[16];// be careful with the length of the buffer
    char *tp = tmp;
    int i;
    unsigned v;
    int sign;

    sign = (radix == 10 && value < 0);
    if (sign)   v = -value;
    else    v = (unsigned)value;

    while (v || tp == tmp)
    {
        i = v % radix;
        v /= radix; // v/=radix uses less CPU clocks than v=v/radix does
        if (i < 10)
          *tp++ = i+'0';
        else
          *tp++ = i + 'a' - 10;
    }

    if (sign)
    *sp++ = '-';
    while (tp > tmp)
    *sp++ = *--tp;

    *sp++ = '\0';
}

#define GRECT_INSET(r, dx, dy) GRect((r).origin.x+(dx), (r).origin.y+(dy), (r).size.w - (2*(dx)), (r).size.h - (2*(dy)))

void layer_update_handler(Layer* l, GContext* ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpAssignInverted);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRECT_INSET(layer_get_frame(l), 10, 10), 32, GCornerTopRight);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Demo");
  window_stack_push(&window, true /* Animated */);

  // If you neglect to call this, all `resource_get_handle()` requests
  // will return NULL.
  resource_init_current_app(&APP_RESOURCES);

  large_font = \
    fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_M_52));
  small_font = \
    fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_M_24));
    
  layer_init(&bg_layer, window.layer.frame);
  bg_layer.update_proc = &layer_update_handler;
  layer_add_child(&window.layer, &bg_layer);

  text_layer_init(&text_layer, GRect(0, 32, layer_get_frame(&window.layer).size.w, 70));
  text_layer_set_font(&text_layer, large_font);
  text_layer_set_text_color(&text_layer, GColorWhite);
	text_layer_set_background_color(&text_layer, GColorClear);
	text_layer_set_text_alignment(&text_layer, GTextAlignmentCenter);
	
	text_layer_init(&subtext_layer, GRect(0, 32+78, layer_get_frame(&window.layer).size.w, 40));
  text_layer_set_font(&subtext_layer, small_font);
  text_layer_set_text_color(&subtext_layer, GColorWhite);
	text_layer_set_background_color(&subtext_layer, GColorClear);
	text_layer_set_text_alignment(&subtext_layer, GTextAlignmentCenter);
	
  layer_add_child(&window.layer, &text_layer.layer);
  layer_add_child(&window.layer, &subtext_layer.layer);

}

void handle_second_tick(AppContextRef ctx, PebbleTickEvent* e) {
  PblTm* t = e->tick_time;
  
  static char buf[8] = "\0";
  int seconds_since_midnight = t->tm_hour * 3600 + t->tm_min * 60 + t->tm_sec;
  itoa(seconds_since_midnight, buf, 10);
  text_layer_set_text(&text_layer, buf);
  
  static char buf2[32] = "\0";
  string_format_time(buf2, sizeof(buf2), "%I.%M.%S %P", t);
  text_layer_set_text(&subtext_layer, buf2);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
  	.tick_info = 
  	{
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}
