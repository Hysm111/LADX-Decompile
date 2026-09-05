#include "home/room.h"
#include "constants/memory.h"
#include "constants/sfx.h"

void MarkTriggerAsResolved(GBState *gb) {
    if (!gb) return;

    if (gb_read(gb, wRoomEventEffectExecuted) != 0) {
        return;
    }

    gb_write(gb, wC1CF, 0);
    gb_write(gb, wRoomEventEffectExecuted, 1);
    gb_write(gb, wC5A6, 1);

    if (gb_read(gb, wC19D) == 0) {
        gb_write(gb, hJingle, JINGLE_PUZZLE_SOLVED);
    }
}
