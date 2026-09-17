#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/items.h"
#include "constants/dialog.h"
#include "constants/hardware.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/sfx.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_got_item(void) {
    /* Test 54: Got Item Tile & Attr Helpers func_002_523A, func_002_523F, func_002_524A */
    {
        GBState gb;
        gb_init(&gb);

        uint8_t a1 = func_002_523A(&gb, 0xC012, 0xAE);
        assert(gb_read(&gb, 0xC012) == 0xAE);
        assert(a1 == 0x14);

        uint8_t a2 = func_002_523F(&gb, 0xC012, 0x8E);
        assert(gb_read(&gb, 0xC012) == 0x8E);
        assert(a2 == 0x14);

        uint8_t a3 = func_002_524A(&gb, 0xC012, 0x8C);
        assert(gb_read(&gb, 0xC012) == 0x8C);
        assert(a3 == 0x10);
    }

    /* Test 55: HandleGotItemB State & Sprite Rendering (02:51C7) */
    {
        GBState gb;

        /* 1. Basic state resets, air physics gravity integration, landing clamp */
        gb_init(&gb);
        gb_write(&gb, wC16A, 0x05);
        gb_write(&gb, wSwordAnimationState, 0x02);
        gb_write(&gb, wIgnoreLinkCollisionsCountdown, 0x04);
        gb_write_hram(&gb, hLinkVelocityZ, 0x01);
        gb_write_hram(&gb, hLinkPositionZ, 0x82); /* negative / ground landing */
        gb_write(&gb, wC149, 0x03);

        HandleGotItemB(&gb, NULL, NULL);
        assert(gb_read(&gb, wC16A) == 0);
        assert(gb_read(&gb, wSwordAnimationState) == 0);
        assert(gb_read(&gb, wIgnoreLinkCollisionsCountdown) == 0);
        assert(gb_read_hram(&gb, hLinkPositionZ) == 0);
        assert(gb_read(&gb, wC149) == 0);
        assert(gb_read_hram(&gb, hLinkVelocityZ) == 0);
        assert(gb_read_hram(&gb, hLinkAnimationState) == LINK_ANIMATION_STATE_UNKNOWN_6B);

        /* 2. Piece of Power item triggers func_1819 callback */
        gb_init(&gb);
        gb_write(&gb, wDialogGotItem, DIALOG_GOT_PIECE_OF_POWER);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x50);
        gb_write_hram(&gb, hLinkPositionZ, 0x04);
        gb_write(&gb, wC13B, 0x08);
        gb_write_hram(&gb, hFrameCounter, 0x04); /* bit 2 set -> (4 << 2) & 0x10 = 0x10 */

        g_mock_20_4ab3_calls = 0;
        HandleGotItemB(&gb, NULL, mock_20_4ab3);
        assert(g_mock_20_4ab3_calls == 1);
        assert(gb_read_hram(&gb, hMultiPurpose1) == (uint8_t)(0x40 - 0x08));
        assert(gb_read_hram(&gb, hMultiPurpose3) == 0x10);
        assert(gb_read_hram(&gb, hMultiPurpose2) == 0x06);

        /* 3. Guardian Acorn sprite setup */
        gb_init(&gb);
        gb_write(&gb, wDialogGotItem, DIALOG_GOT_GUARDIAN_ACORN);
        gb_write_hram(&gb, hLinkPositionX, 0x34);
        gb_write_hram(&gb, hLinkPositionY, 0x48);
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        gb_write(&gb, wC13B, 0x00);

        HandleGotItemB(&gb, NULL, NULL);
        /* multi0 = 0x48 - 0x00 + 0x00 - 0x10 = 0x38 */
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x38);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x10) == (uint8_t)(0x38 + 0x02));
        assert(gb_read(&gb, wLinkOAMBuffer + 0x11) == 0x34);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x12) == 0xAE);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x13) == 0x14);

        /* 4. Magic Powder sprite setup */
        gb_init(&gb);
        gb_write(&gb, wDialogGotItem, DIALOG_GOT_MAGIC_POWDER);
        gb_write_hram(&gb, hLinkPositionX, 0x22);
        gb_write_hram(&gb, hLinkPositionY, 0x30);
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        gb_write(&gb, wC13B, 0x00);

        HandleGotItemB(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x10) == (uint8_t)(0x20 + 0x02));
        assert(gb_read(&gb, wLinkOAMBuffer + 0x11) == 0x22);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x12) == 0x8E);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x13) == 0x16);

        /* 5. Magic Rod sprite setup */
        gb_init(&gb);
        gb_write(&gb, wDialogGotItem, DIALOG_GOT_ROD);
        gb_write_hram(&gb, hLinkPositionX, 0x20);
        gb_write_hram(&gb, hLinkPositionY, 0x30);
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        gb_write(&gb, wC13B, 0x00);

        HandleGotItemB(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x10) == (uint8_t)(0x20 + 0x02));
        assert(gb_read(&gb, wLinkOAMBuffer + 0x11) == 0x20);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x12) == 0x8C);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x13) == 0x10);

        /* 6. Other / default item sprite setup */
        gb_init(&gb);
        gb_write(&gb, wDialogGotItem, 0x00);
        gb_write_hram(&gb, hLinkPositionX, 0x18);
        gb_write_hram(&gb, hLinkPositionY, 0x30);
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        gb_write(&gb, wC13B, 0x00);

        HandleGotItemB(&gb, NULL, NULL);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x10) == (uint8_t)(0x20 + 0x02));
        assert(gb_read(&gb, wLinkOAMBuffer + 0x11) == 0x18);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x12) == 0x8E);
        assert(gb_read(&gb, wLinkOAMBuffer + 0x13) == 0x14);
    }

    /* Test 56: HandleGotItemA Jingle Dispatcher (02:51BC) */
    {
        GBState gb;

        /* Countdown == 0x2E triggers JINGLE_GOT_POWER_UP */
        gb_init(&gb);
        gb_write(&gb, wDialogGotItemCountdown, 0x2E);
        gb_write_hram(&gb, hJingle, 0);

        HandleGotItemA(&gb, NULL, NULL);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_GOT_POWER_UP);

        /* Countdown != 0x2E does not trigger jingle */
        gb_init(&gb);
        gb_write(&gb, wDialogGotItemCountdown, 0x20);
        gb_write_hram(&gb, hJingle, 0);

        HandleGotItemA(&gb, NULL, NULL);
        assert(gb_read_hram(&gb, hJingle) == 0);
    }

}
