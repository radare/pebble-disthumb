#include <pebble.h>
#include "thumb.h"

static Window *window;
static TextLayer *text_layer;
static TextLayer *hex_layer;
static TextLayer *cur_layer;

static int nibble = 0;
static uint32_t insn = 0;
static uint8_t *insn8 = (uint8_t*)&insn;
char hexstr[32];
char curstr[32];
static int insnlen = 0;
static struct thumb_disasm_t dis;

static void show_hex(TextLayer *text_layer) {
    int byte = nibble / 2;
    int nibb = nibble % 2;
if (byte>=insnlen) {
  byte = nibb = nibble = 0;
}
    snprintf (hexstr, sizeof (hexstr), "%02x %02x %02x %02x",
      insn8[0], insn8[1], insn8[2], insn8[3]);
    strcpy (curstr, "-- -- -- --");
    curstr[(byte*3)+nibb] = '^';
if (insnlen>0) {
hexstr[insnlen*3] = 0;
curstr[insnlen*3] = 0;
}
    text_layer_set_text(cur_layer, curstr);
    text_layer_set_text(text_layer, hexstr);
}

static void show_disasm (TextLayer *text_layer) {
  dis.pc = 0x100;
  dis.buf = (const uint8_t*)&insn;
  insnlen = thumb_disasm (&dis);
  if (insnlen) {
    text_layer_set_text(text_layer, dis.str);
  } else {
    text_layer_set_text(text_layer, "invalid");
  }
  show_hex (hex_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  nibble ++;
  if (nibble>7) nibble = 0;
  show_disasm (text_layer);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  int byte = nibble/2;
  uint8_t n1 = (insn8[byte]) & 0xf;
  uint8_t n2 = (insn8[byte]>>4) & 0xf;
  if (nibble & 1) {
    n1 ++;
  } else {
    n2 ++;
  }
  insn8[byte] = (n1&0xf) | (n2&0xf)<<4;
  show_disasm (text_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  int byte = nibble/2;
  uint8_t n1 = (insn8[byte]) & 0xf;
  uint8_t n2 = (insn8[byte]>>4) & 0xf;
  if (nibble & 1) {
    n1 --;
  } else {
    n2 --;
  }
  insn8[byte] = (n1&0xf) | (n2&0xf)<<4;
  show_disasm (text_layer);
}

static void click_config_provider(void *context) {
  window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 100, select_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  hex_layer = text_layer_create((GRect) { .origin = { 0, 16}, .size = { bounds.size.w, 42 } });
  text_layer_set_text_alignment(hex_layer, GTextAlignmentCenter);
  text_layer_set_font(hex_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(hex_layer));

  cur_layer = text_layer_create((GRect) { .origin = { 0, 40}, .size = { bounds.size.w, 50 } });
  text_layer_set_text_alignment(cur_layer, GTextAlignmentCenter);
  text_layer_set_font(cur_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(cur_layer));

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 50 } });
  show_disasm (text_layer);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
