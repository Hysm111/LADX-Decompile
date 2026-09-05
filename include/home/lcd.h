#ifndef LADX_HOME_LCD_H
#define LADX_HOME_LCD_H

#include "gb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * LCDOff (00:28CF)
 * Turns off the LCD display during vertical blanking.
 * Backs up rIE to hInterrupts, masks out IEF_VBLANK to prevent interrupts
 * while disabling, waits for scanline 145 (SCRN_Y + 1), clears LCDCF_ON
 * in rLCDC, and restores rIE from hInterrupts.
 */
void LCDOff(GBState *gb);

#ifdef __cplusplus
}
#endif

#endif /* LADX_HOME_LCD_H */
