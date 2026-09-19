#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/items.h"
#include "constants/dialog.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/memory.h"
#include "constants/sfx.h"

#include <assert.h>
#include <stdint.h>

void test_bank2_func_002_60E0(void) {
    /* Test: func_002_60E0 (02:60E0-02:6206) */

    /* 1. Clamps item counts - magic powder */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x20);  /* max = 32 */
        gb_write(&gb, wMagicPowderCount, 0x30); /* current = 48 */

        func_002_60E0(&gb);

        assert(gb_read(&gb, wMagicPowderCount) == 0x20);
    }

    /* 2. Clamps item counts - bombs */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x3C);  /* max = 60 */
        gb_write(&gb, wBombCount, 0x50);       /* current = 80 */

        func_002_60E0(&gb);

        assert(gb_read(&gb, wBombCount) == 0x3C);
    }

    /* 3. Clamps item counts - arrows */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x46);  /* max = 70 */
        gb_write(&gb, wArrowCount, 0x46);      /* current = 70 */

        func_002_60E0(&gb);

        assert(gb_read(&gb, wArrowCount) == 0x46);
    }

    /* 4. Returns early if Link is not interactive */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wLinkMotionState, LINK_MOTION_TYPE_NON_INTERACTIVE);

        func_002_60E0(&gb);

        /* Should not crash */
    }

    /* 5. Returns early if dialog state is non-zero */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wDialogState, 0x01);

        func_002_60E0(&gb);

        /* Should not crash */
    }

    /* 6. Returns early if room transition state is non-zero */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wRoomTransitionState, 0x01);

        func_002_60E0(&gb);

        /* Should not crash */
    }

    /* 7. Handles subscreen transition - inventory appearing */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wInventoryAppearing, 0x01);
        gb_write(&gb, wDrawCommand, 0x00);
        gb_write(&gb, wInventoryShouldScroll, 0x00);

        func_002_60E0(&gb);

        assert(gb_read(&gb, wInventoryShouldScroll) == 0x01);
    }

    /* 8. Returns early if SELECT not pressed */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hPressedButtonsMask, 0x00);

        func_002_60E0(&gb);

        /* Should not crash - returns early */
    }

    /* 9. Returns early if START not pressed */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hPressedButtonsMask, J_SELECT);
        gb_write_hram(&gb, hJoypadState, 0x00);

        func_002_60E0(&gb);

        /* Should not crash - returns early */
    }

    /* 10. NULL state handling */
    {
        func_002_60E0(NULL);
        /* Should not crash */
    }

    /* 11. func_002_61BA helper */
    {
        GBState gb;
        gb_init(&gb);
        func_002_61BA(&gb);
        /* Should not crash with stubs */
    }

    /* 12. UpdateRupeesCount - basic */
    {
        GBState gb;
        gb_init(&gb);
        UpdateRupeesCount(&gb);
        /* Should not crash */
    }

    /* 13. UpdateHealth - basic */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxHearts, 0x03);
        gb_write(&gb, wHealth, 0x18);
        UpdateHealth(&gb);
        /* Should not crash */
    }

    /* 14. LoadRupeesDigits */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wDrawCommandsSize, 0x00);
        gb_write(&gb, wRupeeCountHigh, 0x01);
        gb_write(&gb, wRupeeCountLow, 0x23);
        LoadRupeesDigits(&gb);
        assert(gb_read(&gb, wDrawCommandsSize) == 0x06);
    }

    /* 15. LoadHeartsCount */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wDrawCommandsSize, 0x00);
        gb_write(&gb, wHealth, 0x10);
        LoadHeartsCount(&gb);
        assert(gb_read(&gb, wDrawCommandsSize) == 0x14);
    }

    /* 16. ClampItemCount with zero max */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wMaxMagicPowder, 0x00);
        gb_write(&gb, wMagicPowderCount, 0x10);

        func_002_60E0(&gb);

        assert(gb_read(&gb, wMagicPowderCount) == 0x00);
    }

    /* 17. Subscreen opening logic (START pressed, SELECT not pressed) */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wSubscreenScrollIncrement, 0x08);  /* Initial value */
        gb_write_hram(&gb, hPressedButtonsMask, 0x00);
        gb_write_hram(&gb, hJoypadState, J_START);
        gb_write(&gb, wWindowY, 0x00);
        gb_write(&gb, wD464, 0x00);
        gb_write(&gb, wC167, 0x00);
        gb_write_hram(&gb, hLinkInteractiveMotionBlocked, 0x00);
        gb_write_hram(&gb, hLinkAnimationState, 0x00);
        gb_write(&gb, wOcarinaMenuOpen, 0x00);
        gb_write(&gb, wOcarinaMenuOpening, 0x00);
        gb_write(&gb, wOcarinaMenuClosing, 0x00);
        gb_write(&gb, wIsIndoor, 0x00);
        gb_write_hram(&gb, hMapId, 0x00);

        func_002_60E0(&gb);

        assert(gb_read(&gb, wInventoryAppearing) == 0x01);
        assert(gb_read(&gb, wGameplayType) == GAMEPLAY_INVENTORY);
        assert(gb_read(&gb, wGameplaySubtype) == GAMEPLAY_INVENTORY_INITIAL);
    }

    /* 18. Map opening logic (SELECT pressed) */
    {
        GBState gb;
        gb_init(&gb);
        gb_write_hram(&gb, hPressedButtonsMask, J_SELECT);
        gb_write_hram(&gb, hJoypadState, 0x00);
        gb_write(&gb, wDialogState, 0x00);

        func_002_60E0(&gb);

        /* Should reach inventory_fully_closed2 and call UpdateRupeesCount/UpdateHealth */
        /* Just verify it doesn't crash */
    }
}