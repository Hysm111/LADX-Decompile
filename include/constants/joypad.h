#ifndef LADX_CONSTANTS_JOYPAD_H
#define LADX_CONSTANTS_JOYPAD_H

/* Select the corresponding button matrix in rP1 */
#define J_DPAD    0x10
#define J_BUTTONS 0x20

/* Joypad button bit masks */
#define J_RIGHT  (1 << 0)
#define J_LEFT   (1 << 1)
#define J_UP     (1 << 2)
#define J_DOWN   (1 << 3)
#define J_A      (1 << 4)
#define J_B      (1 << 5)
#define J_SELECT (1 << 6)
#define J_START  (1 << 7)

#endif /* LADX_CONSTANTS_JOYPAD_H */
