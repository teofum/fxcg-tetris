#ifndef __INPUT_H__
#define __INPUT_H__

#define KB_REGISTER (void *)0xa44b0000

/*
 * Keyboard modifier state
 */
#define KB_MODE_STD 0x00
#define KB_MODE_SHIFT 0x01
#define KB_MODE_CLIP 0x02
#define KB_MODE_ALPHA 0x04
#define KB_MODE_ALPHA_LOCK 0x80

typedef unsigned char kb_mode_t;

/*
 * Poll keyboard hardware and update internal buffers. Used for non-blocking
 * keyboard input.
 * This function should be called every frame.
 */
void kb_update();

/*
 * Utility function to handle special keys (MENU, Shift + AC).
 * Calls GetKey() if one of the keys that need special handling is pressed.
 * Should always be called after kb_update().
 */
void kb_handle_special_keys();

/*
 * Check if a key is currently pressed.
 * Returns true (1) if the key is being pressed, 0 if it is not.
 */
int kb_ispressed(int code);

/*
 * Check for a keydown event.
 * Returns true (1) if the key was pressed last tick (last time kb_update() was
 * called).
 */
int kb_keydown(int code);

/*
 * Check for a keyup event.
 * Returns true (1) if the key was released last tick (last time kb_update() was
 * called).
 */
int kb_keyup(int code);

/*
 * Get the current modifier state.
 */
kb_mode_t kb_mode();

/*
 * Check the Shift modifier.
 * Returns true (1) if the Shift modifier is active.
 */
int kb_shift();

/*
 * Check the Alpha modifier.
 * Returns true (1) if the Alpha modifier is active.
 */
int kb_alpha();

#endif
