#include "input.h"
#include "menu.h"

#include <fxcg/keyboard.h>
#include <fxcg/system.h>
#include <string.h>

unsigned short keys_current[8];
unsigned short keys_last[8];

// Modifier state
kb_mode_t keyboard_mode = KB_MODE_STD;

void kb_update() {
  memcpy(keys_last, keys_current, sizeof(unsigned short) * 8);
  memcpy(keys_current, KB_REGISTER, sizeof(unsigned short) * 8);

  // Update modifier key state when a modifier key is pressed
  if (kb_keydown(KEY_PRGM_SHIFT)) {
    keyboard_mode =
        keyboard_mode == KB_MODE_SHIFT ? KB_MODE_STD : KB_MODE_SHIFT;
  } else if (kb_keydown(KEY_PRGM_ALPHA)) {
    switch (keyboard_mode) {
    case KB_MODE_STD:
      keyboard_mode = KB_MODE_ALPHA;
      break;
    case KB_MODE_SHIFT:
      keyboard_mode = KB_MODE_ALPHA | KB_MODE_ALPHA_LOCK;
      break;
    default:
      keyboard_mode = KB_MODE_STD;
    }
  }
  SetSetupSetting(0x14, keyboard_mode);
}

void kb_handle_special_keys() {
  if (kb_keydown(KEY_PRGM_MENU)) {
    SaveAndOpenMainMenu();
  } else if (keyboard_mode & KB_MODE_SHIFT && kb_keydown(KEY_PRGM_ACON)) {
    PowerOff(1);
  }
}

static inline int kb_check(unsigned short *buf, int code) {
  int row = code % 10;
  int col = code / 10 - 1;
  int word = row >> 1;
  int bit = col + 8 * (row & 1);
  return (buf[word] & 1 << bit) != 0;
}

int kb_ispressed(int code) { return kb_check(keys_current, code); }

int kb_keydown(int code) {
  return kb_check(keys_current, code) && !kb_check(keys_last, code);
}

int kb_keyup(int code) {
  return !kb_check(keys_current, code) && kb_check(keys_last, code);
}

kb_mode_t kb_mods() { return keyboard_mode; }

int kb_shift() { return (keyboard_mode & KB_MODE_SHIFT) != 0; }

int kb_alpha() { return (keyboard_mode & KB_MODE_ALPHA) != 0; }
