#include "home/lcd.h"
#include "constants/hardware.h"
#include "constants/memory.h"

void LCDOff(GBState *gb) {
    if (!gb) return;

    /* Save interrupts configuration */
    uint8_t ie = gb_read(gb, rIE);
    gb_write(gb, hInterrupts, ie);

    /* Disable VBlank interrupt */
    gb_write(gb, rIE, ie & ~IEF_VBLANK);

    /*
     * Wait for row 145 (SCRN_Y + 1).
     * In hardware:
     *   .waitForEndOfLine
     *   ldh a, [rLY]
     *   cp SCRN_Y + 1
     *   jr nz, .waitForEndOfLine
     */
    gb_write(gb, rLY, SCRN_Y + 1);

    /* Switch off LCD screen */
    uint8_t lcdc = gb_read(gb, rLCDC);
    gb_write(gb, rLCDC, lcdc & ~LCDCF_ON);

    /* Restore interrupts configuration */
    gb_write(gb, rIE, gb_read(gb, hInterrupts));
}
