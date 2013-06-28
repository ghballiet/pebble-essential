#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x6F, 0x62, 0x2E, 0xF1, 0x62, 0xCA, 0x48, 0x55, 0x89, 0xF3, 0x44, 0x74, 0xD8, 0x31, 0x28, 0x35 }
PBL_APP_INFO(MY_UUID,
             "Essential", "ghballiet",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_STANDARD_APP);

Window window;


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Window Name");
  window_stack_push(&window, true /* Animated */);
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init
  };
  app_event_loop(params, &handlers);
}
