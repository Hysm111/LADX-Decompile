#include "bank2/room_transition.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/sfx.h"
#include "home/link.h"

/* Bank 2 Debug Warp Tables (02:5471+) */
const uint8_t DebugWarpRooms[11] = {
    0x30, /* MAP_BOTTLE_GROTTO */
    0x33, /* MAP_BOTTLE_GROTTO */
    0x81, /* MAP_CATFISHS_MAW */
    0x01, /* MAP_TAIL_CAVE */
    0x28, /* MAP_BOTTLE_GROTTO */
    0x56, /* MAP_KEY_CAVERN */
    0x68, /* MAP_ANGLERS_TUNNEL */
    0x87, /* MAP_CATFISHS_MAW */
    0xB3, /* MAP_FACE_SHRINE */
    0xE6, /* MAP_EAGLES_TOWER */
    0x0A  /* MAP_TURTLE_ROCK */
};

const uint8_t DebugWarpMaps[11] = {
    MAP_BOTTLE_GROTTO,
    MAP_BOTTLE_GROTTO,
    MAP_CATFISHS_MAW,
    MAP_TAIL_CAVE,
    MAP_BOTTLE_GROTTO,
    MAP_KEY_CAVERN,
    MAP_ANGLERS_TUNNEL,
    MAP_CATFISHS_MAW,
    MAP_FACE_SHRINE,
    MAP_EAGLES_TOWER,
    MAP_TURTLE_ROCK
};

void ExecuteDebugWarp(GBState *gb) {
    if (!gb) return;

    gb_write(gb, wWarp0MapCategory, 0x01);
    uint8_t index = gb_read(gb, wDebugWarpIndex);
    uint8_t next_index = (uint8_t)(index + 1);
    if (next_index >= 0x0B) {
        next_index = 0;
    }
    gb_write(gb, wDebugWarpIndex, next_index);

    if (index >= 11) {
        index = 0;
    }
    gb_write(gb, wWarp0Room, DebugWarpRooms[index]);
    gb_write(gb, wWarp0Map, DebugWarpMaps[index]);
    gb_write(gb, wWarp0DestinationX, 0x50);
    gb_write(gb, wWarp0DestinationY, 0x70);
    gb_write_hram(gb, hJingle, JINGLE_PUZZLE_SOLVED);
    ApplyMapFadeOutTransitionWithNoise(gb);
}

void staircaseIsActive(GBState *gb) {
    if (!gb) return;

    /* Don't trigger staircase when jumping over it */
    if (gb_read_hram(gb, hLinkPositionZ) != 0) {
        return;
    }

    /* Don't trigger staircase if not over it */
    uint8_t diff_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - gb_read_hram(gb, hStaircasePosX) + 0x05);
    if (diff_x >= 0x0A) {
        return;
    }

    uint8_t diff_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - gb_read_hram(gb, hStaircasePosY) + 0x05);
    if (diff_y >= 0x0A) {
        return;
    }

    /* Don't trigger staircase when carrying an object */
    if (gb_read(gb, wIsCarryingLiftedObject) != 0) {
        return;
    }

    /* Don't trigger staircase when color dungeon entrance is not open */
    if (gb_read_hram(gb, hMapRoom) == ROOM_OW_COLOR_DUNGEON_ENTRANCE && gb_read(gb, wIsIndoor) == 0) {
        if (gb_read(gb, wColorDungonCorrectTombStones) != 0x80) {
            return;
        }
    }

    /* Trigger staircase warp */
    ApplyMapFadeOutTransitionWithSound(gb);
    gb_write_hram(gb, hStaircase, STAIRCASE_NONE);
}

void renderTranscientVFXs(GBState *gb, void (*render_transient_vfx)(GBState *, uint8_t)) {
    if (!gb) return;

    for (int i = 0x0F; i >= 0; i--) {
        gb_write(gb, wActiveEntityIndex, (uint8_t)i);
        uint8_t vfx_type = gb_read(gb, (uint16_t)(wTranscientVfxTypeTable + i));
        if (vfx_type != 0 && render_transient_vfx) {
            render_transient_vfx(gb, (uint8_t)i);
        }
    }

    if (gb_read(gb, wRoomTransitionState) != 0) {
        return;
    }

    uint8_t staircase = gb_read_hram(gb, hStaircase);
    if (staircase == STAIRCASE_NONE) {
        return;
    }

    if (staircase != STAIRCASE_INACTIVE) {
        staircaseIsActive(gb);
        return;
    }

    /* If the player left an inactive staircase, mark it as active */
    uint8_t diff_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) - gb_read_hram(gb, hStaircasePosX) + 0x06);
    if (diff_x >= 0x0C) {
        gb_write_hram(gb, hStaircase, STAIRCASE_ACTIVE);
        return;
    }

    uint8_t diff_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - gb_read_hram(gb, hStaircasePosY) + 0x06);
    if (diff_y >= 0x0C) {
        gb_write_hram(gb, hStaircase, STAIRCASE_ACTIVE);
        return;
    }
}

void label_002_5487(GBState *gb, void (*render_transient_vfx)(GBState *, uint8_t)) {
    if (!gb) return;

    gb_write(gb, wIndoorARoomStatus, 0x00);
    gb_write(gb, wIndoorBRoomStatus, 0x00);

    uint8_t dialog_cd = gb_read(gb, wDialogCooldown);
    if (dialog_cd != 0) {
        gb_write(gb, wDialogCooldown, (uint8_t)(dialog_cd - 1));
    }

    uint8_t photo_cd = gb_read(gb, wPhotoAlbumCooldown);
    if (photo_cd != 0) {
        gb_write(gb, wPhotoAlbumCooldown, (uint8_t)(photo_cd - 1));
    }

    renderTranscientVFXs(gb, render_transient_vfx);
}
