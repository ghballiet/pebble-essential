#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x6F, 0x62, 0x2E, 0xF1, 0x62, 0xCA, 0x48, 0x55, 0x89, 0xF3, 0x44, 0x74, 0xD8, 0x31, 0x28, 0x35 }
PBL_APP_INFO(MY_UUID,
			 "Essential", "ghballiet",
			 1, 0, /* App version */
			 DEFAULT_MENU_ICON,
			 APP_INFO_WATCH_FACE);

Window window;

TextLayer text_time_layer;

void handle_init(AppContextRef ctx) {
	(void)ctx;

	window_init(&window, "Essential");
	window_stack_push(&window, true /* Animated */);
	window_set_background_color(&window, GColorBlack);

	resource_init_current_app(&APP_RESOURCES);

	text_layer_init(&text_time_layer, window.layer.frame);
	text_layer_set_text_color(&text_time_layer, GColorWhite);
	text_layer_set_background_color(&text_time_layer, GColorClear);
	layer_set_frame(&text_time_layer.layer, GRect(4, 57, 144-4, 168-57));
	text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PRAGMATA_BOLD_48)));
	layer_add_child(&window.layer, &text_time_layer.layer);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;

	// need to be static because they're used by the system later.
	static char time_text[] = "00|00";

	char *time_format;

	if(clock_is_24h_style())
		time_format = "%H|%M";
	else
		time_format = "%I%M";

	string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);

	text_layer_set_text(&text_time_layer, time_text);
}


void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,

		.tick_info = {
			.tick_handler = &handle_minute_tick,
			.tick_units = MINUTE_UNIT
		}
	};
	app_event_loop(params, &handlers);
}
