#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x6F, 0x62, 0x2E, 0xF1, 0x62, 0xCA, 0x48, 0x55, 0x89, 0xF3, 0x44, 0x74, 0xD8, 0x31, 0x28, 0x35 }
PBL_APP_INFO(MY_UUID,
			 "Essential", "ghballiet",
			 1, 0, /* App version */
			 DEFAULT_MENU_ICON,
			 APP_INFO_WATCH_FACE);

#define TIME_ZONE_OFFSET 7

Window window;

TextLayer text_time_layer;
TextLayer text_date_layer;
TextLayer text_unix_layer;

// make it uppercase
// found here: http://goo.gl/bbgVd
char *upcase(char *str) {
	for (int i = 0; str[i] != 0; i++) {
		if (str[i] >= 'a' && str[i] <= 'z') {
			str[i] -= 0x20;
		}
	}

	return str;
}

void handle_init(AppContextRef ctx) {
	(void)ctx;

	window_init(&window, "Essential");
	window_stack_push(&window, true /* Animated */);
	window_set_background_color(&window, GColorBlack);

	resource_init_current_app(&APP_RESOURCES);

	// time
	text_layer_init(&text_time_layer, window.layer.frame);
	text_layer_set_text_color(&text_time_layer, GColorWhite);
	text_layer_set_background_color(&text_time_layer, GColorClear);
	layer_set_frame(&text_time_layer.layer, GRect(4, 10, 144-4, 168-10));
	text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_MONO_BOLD_53)));
	layer_add_child(&window.layer, &text_time_layer.layer);

	// date
	text_layer_init(&text_date_layer, window.layer.frame);
	text_layer_set_text_color(&text_date_layer, GColorWhite);
	text_layer_set_background_color(&text_date_layer, GColorClear);
	layer_set_frame(&text_date_layer.layer, GRect(17, 75, 144-17, 168-75));
	text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_MONO_24)));
	layer_add_child(&window.layer, &text_date_layer.layer);

	// unix
	text_layer_init(&text_unix_layer, window.layer.frame);
	text_layer_set_text_color(&text_unix_layer, GColorWhite);
	text_layer_set_background_color(&text_unix_layer, GColorClear);
	layer_set_frame(&text_unix_layer.layer, GRect(25, 120, 144-25, 168-120));
	text_layer_set_font(&text_unix_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UBUNTU_MONO_14)));
	layer_add_child(&window.layer, &text_unix_layer.layer);
}

int get_unix_time(PblTm *now) {
	// taken from http://goo.gl/pnPwQ
	unsigned int unix_time;
	unix_time = ((0-TIME_ZONE_OFFSET)*3600) // time zone offset, in hours
		+ now->tm_sec // start with seconds
		+ now->tm_min * 60 // add minutes
		+ now->tm_hour * 3600 // add hours
		+ now->tm_yday * 86400 // add days
		+ (now->tm_year - 70) * 31536000 // add years since 1970
		+ ((now->tm_year - 69) / 4) * 86400 // add a day after leap years, starting in 1973
		+ ((now->tm_year - 1) / 100) * 86400 // remove a leap year every 100 years, starting in 2001
		+ ((now->tm_year + 299) / 400) * 86400; // add a leap year back every 400 years, starting in 2001
	return unix_time;
}

char *itoa(int num) {
	// found here: http://goo.gl/DaC1N
	static char buff[20] = {};
	int i = 0, temp_num = num, length = 0;
	char *string = buff;

	if(num >= 0) {
		// count how many characters in the number
		while(temp_num) {
			temp_num /= 10;
			length++;
		}

		// assign the number to the buffer starting at the end of the
		// number and going to the begining since we are doing the
		// integer to character conversion on the last number in the
		// sequence
		for(i = 0; i < length; i++) {
			buff[(length-1)-i] = '0' + (num % 10);
			num /= 10;
		}
		buff[i] = '\0'; // can't forget the null byte to properly end our string
	}
	else
		return "Unsupported Number";

	return string;
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
	(void)ctx;

	// need to be static because they're used by the system later.
	static char time_text[] = "00|00";
	static char date_text[] = "XXX 00/00";
	static char unix_text[] = "00000000000";

	char *time_format;

	// date
	string_format_time(date_text, sizeof(date_text), "%a %m/%d", t->tick_time);
	text_layer_set_text(&text_date_layer, upcase(date_text));

	if(clock_is_24h_style())
		time_format = "%H|%M";
	else
		time_format = "%I:%M";

	// time
	string_format_time(time_text, sizeof(time_text), time_format, t->tick_time);
	text_layer_set_text(&text_time_layer, time_text);

	// unix
	// string_format_time(unix_text, sizeof(unix_text), "%d", get_unix_time(t->tick_time));
	// unix_text = itoa(get_unix_time(t->tick_time));
	text_layer_set_text(&text_unix_layer, itoa(get_unix_time(t->tick_time)));
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
