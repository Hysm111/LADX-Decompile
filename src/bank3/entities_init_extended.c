#include "bank3/entities_init_extended.h"
#include "bank3/entities_physics.h"
#include "bank3/entities_init_basic.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/gameplay.h"
#include "constants/directions.h"
#include "constants/inventory.h"
#include "constants/joypad.h"
#include "constants/sfx.h"
#include "constants/gfx.h"
#include "home/entities.h"
#include "home/room.h"
#include "home/bank.h"
#include "home/audio.h"
#include "home/gameplay.h"
#include "constants/audio.h"

/* ===== EntityInitSouthFaceShrineDoor (03:4B57) ===== */
void EntityInitSouthFaceShrineDoor(GBState *gb) {
    if (!gb) return;

    /* ld a, IEF_STAT | IEF_VBLANK; ldh [rIE], a; ret */
    gb_write(gb, rIE, 0x03);
}

/* ===== EntityInitLeever (03:4B5C) ===== */
void EntityInitLeever(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, $FF; jp SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, 0xFF);
}

/* ===== EntityInitZora (03:4B61) ===== */
void EntityInitZora(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, [wIsIndoor]; and a; jr z, EntityInitNoop */
    if (gb_read(gb, wIsIndoor) == 0) {
        return;
    }

    /* ldh a, [hMapRoom]; cp UNKNOWN_ROOM_DA; jr nz, EntityInitNoop */
    if (gb_read_hram(gb, hMapRoom) != UNKNOWN_ROOM_DA) {
        return;
    }

    /* ld a, [wTradeSequenceItem]; cp TRADING_ITEM_MAGNIFYING_LENS; jp nz, UnloadEntityAndReturn */
    if (gb_read(gb, wTradeSequenceItem) != TRADING_ITEM_MAGNIFYING_LENS) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* ld a, [wPhotos2]; and $01; jr z, EntityInitNoop */
    if ((gb_read(gb, wPhotos2) & 0x01) == 0) {
        return;
    }

    /* ld a, $03; jp SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, 0x03);
}

/* ===== EntityInitWithRightDirection (03:4B81) ===== */
void EntityInitWithRightDirection(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* xor a; jr SetEntityDirection */
    SetEntityDirection(gb, bc, DIRECTION_RIGHT);
}

/* ===== GetColorDungeonRoomStatus (03:4B84) ===== */
uint8_t GetColorDungeonRoomStatus(GBState *gb) {
    if (!gb) return 0;

    /* ld hl, wColorDungeonRoomStatus; ldh a, [hMapRoom]; ld e, a; ld d, $00; add hl, de; ld a, [hl]; ret */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    return gb_read(gb, wColorDungeonRoomStatus + map_room);
}

/* ===== EntityInitRotoswitchRed (03:4B8F) ===== */
void EntityInitRotoswitchRed(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetColorDungeonRoomStatus; and $10; jr nz, jr_003_4BAD */
    uint8_t status = GetColorDungeonRoomStatus(gb);
    if (status & 0x10) {
        /* jr_003_4BAD: ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $80 */
        gb_write(gb, wEntitiesStateTable + bc, 0x80);
    } else {
        /* xor a; jp SetEntitySpriteVariant */
        SetEntitySpriteVariant(gb, bc, 0x00);
    }
}

/* ===== EntityInitRotoswitchYellow (03:4B9A) ===== */
void EntityInitRotoswitchYellow(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetColorDungeonRoomStatus; and $10; jr nz, jr_003_4BAD */
    uint8_t status = GetColorDungeonRoomStatus(gb);
    if (status & 0x10) {
        /* jr_003_4BAD: ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $80 */
        gb_write(gb, wEntitiesStateTable + bc, 0x80);
    } else {
        /* ld a, $04; jp SetEntitySpriteVariant */
        SetEntitySpriteVariant(gb, bc, 0x04);
    }
}

/* ===== EntityInitRotoswitchBlue (03:4BA6) ===== */
void EntityInitRotoswitchBlue(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetColorDungeonRoomStatus; and $10; jr z, jr_003_4BB3 */
    uint8_t status = GetColorDungeonRoomStatus(gb);
    if (!(status & 0x10)) {
        /* jr_003_4BB3: ld a, $08; jp SetEntitySpriteVariant */
        SetEntitySpriteVariant(gb, bc, 0x08);
        return;
    }

    /* jr_003_4BAD: ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $80 */
    gb_write(gb, wEntitiesStateTable + bc, 0x80);
    /* fallthrough to SetEntitySpriteVariant with $08 */
    SetEntitySpriteVariant(gb, bc, 0x08);
}

/* ===== EntityInitHopper (03:4BB8) ===== */
void EntityInitHopper(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $03 */
    gb_write(gb, wEntitiesStateTable + bc, 0x03);

    /* jr EntityInitFlyingHopperBombs.setPosZ */
    /* fallthrough to setPosZ */
    gb_write(gb, wEntitiesPosZTable + bc, 0x10);
    SetEntitySpriteVariant(gb, bc, 0x04);
}

/* ===== EntityInitFlyingHopperBombs (03:4BC0) ===== */
void EntityInitFlyingHopperBombs(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, $04; fallthrough to .setPosZ */
    /* .setPosZ: ld hl, wEntitiesPosZTable; add hl, bc; ld [hl], $10; jp SetEntitySpriteVariant */
    gb_write(gb, wEntitiesPosZTable + bc, 0x10);
    SetEntitySpriteVariant(gb, bc, 0x04);
}

/* ===== EntityInitHardHitBeetle (03:4BCB) ===== */
void EntityInitHardHitBeetle(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesHealthTable; add hl, bc; ld [hl], $10 */
    gb_write(gb, wEntitiesHealthTable + bc, 0x10);

    /* ld hl, wEntitiesPosXTable; add hl, bc; ld a, [hl]; sub $08; ld [hl], a */
    uint8_t pos_x = gb_read(gb, wEntitiesPosXTable + bc);
    gb_write(gb, wEntitiesPosXTable + bc, (uint8_t)(pos_x - 0x08));

    /* jp EntityInitNoop */
    (void)gb;
}

/* ===== EntityInitAvalaunch (03:4BDC) ===== */
void EntityInitAvalaunch(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPosXTable; add hl, bc; ld [hl], $50 */
    gb_write(gb, wEntitiesPosXTable + bc, 0x50);

    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], $00 */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0x00);

    /* jp EntityInitNoop */
    (void)gb;
}

/* ===== EntityInitColorGuardianBlue (03:4BEB) ===== */
void EntityInitColorGuardianBlue(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hIsGBC]; and a; jp z, EntityInitNoop */
    if (gb_read_hram(gb, hIsGBC) == 0) {
        return;
    }

    /* call GetColorDungeonRoomStatus; and $10; jp z, EntityInitNoop */
    uint8_t status = GetColorDungeonRoomStatus(gb);
    if ((status & 0x10) == 0) {
        return;
    }

    /* ld hl, wEntitiesPosXTable; add hl, bc; ld a, $3C; jr jr_003_4C15 */
    gb_write(gb, wEntitiesPosXTable + bc, 0x3C);

    /* jr_003_4C15: ld [hl], a; ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $04; jp EntityInitNoop */
    gb_write(gb, wEntitiesStateTable + bc, 0x04);
}

/* ===== EntityInitColorGuardianRed (03:4C01) ===== */
void EntityInitColorGuardianRed(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hIsGBC]; and a; jp z, EntityInitNoop */
    if (gb_read_hram(gb, hIsGBC) == 0) {
        return;
    }

    /* call GetColorDungeonRoomStatus; and $10; jp z, EntityInitNoop */
    uint8_t status = GetColorDungeonRoomStatus(gb);
    if ((status & 0x10) == 0) {
        return;
    }

    /* ld hl, wEntitiesPosXTable; add hl, bc; ld a, $63 */
    gb_write(gb, wEntitiesPosXTable + bc, 0x63);

    /* jr_003_4C15: ld [hl], a; ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $04; jp EntityInitNoop */
    gb_write(gb, wEntitiesStateTable + bc, 0x04);
}

/* ===== EntityInitColorDungeonBook (03:4C1F) ===== */
void EntityInitColorDungeonBook(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPosYTable; add hl, bc; inc [hl]; inc [hl] */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y + 2));

    /* ld hl, wEntitiesPosZTable; add hl, bc; ld [hl], $04 */
    gb_write(gb, wEntitiesPosZTable + bc, 0x04);

    /* fallthrough to EntityInitGiantBuzzBlob */
    /* EntityInitGiantBuzzBlob (03:4C2D) */
    /* ld hl, wEntitiesHealthTable; add hl, bc; ld [hl], $0C */
    gb_write(gb, wEntitiesHealthTable + bc, 0x0C);

    /* xor a; ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0x00);

    /* ld hl, wEntitiesPosXTable; add hl, bc; ld a, [hl]; add $08; ld [hl], a */
    uint8_t pos_x = gb_read(gb, wEntitiesPosXTable + bc);
    gb_write(gb, wEntitiesPosXTable + bc, (uint8_t)(pos_x + 0x08));

    /* jp EntityInitNoop */
    (void)gb;
}

/* ===== EntityInitGiantBuzzBlob (03:4C2D) ===== */
/* Already handled by EntityInitColorDungeonBook fallthrough */

/* ===== EntityInitWithRandomSpeed (03:4EA8) ===== */
void EntityInitWithRandomSpeed(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetRandomByte; and $03; ld e, a; ld d, b */
    uint8_t random = GetRandomByte(gb) & 0x03;

    /* ld hl, EntityRandomSpeedX; add hl, de; ld a, [hl] */
    static const int8_t EntityRandomSpeedX[4] = { 12, 12, -12, -12 };
    static const int8_t EntityRandomSpeedY[4] = { 12, -12, 12, -12 };

    /* ld hl, wEntitiesSpeedXTable; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesSpeedXTable + bc, (uint8_t)EntityRandomSpeedX[random]);

    /* ld hl, EntityRandomSpeedY; add hl, de; ld a, [hl] */
    /* ld hl, wEntitiesSpeedYTable; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesSpeedYTable + bc, (uint8_t)EntityRandomSpeedY[random]);
}

/* ===== EntityInitSparkClockwise (03:4EC4) ===== */
void EntityInitSparkClockwise(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPrivateState2Table; add hl, bc; ld [hl], $04 */
    gb_write(gb, wEntitiesPrivateState2Table + bc, 0x04);

    /* ld a, $03; jr jr_003_4ED0 */
    /* jr_003_4ED0: ld hl, wEntitiesPosYTable; add hl, bc; add [hl]; ld [hl], a */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y + 0x03));
}

/* ===== EntityInitSparkCounterClockwise (03:4ECE) ===== */
void EntityInitSparkCounterClockwise(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, $FD; jr_003_4ED0: ld hl, wEntitiesPosYTable; add hl, bc; add [hl]; ld [hl], a */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y + 0xFD));  /* -3 */
}

/* ===== EntityInitWizrobe (03:4ED7) ===== */
void EntityInitWizrobe(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetEntityTransitionCountdown; ld [hl], $80 */
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x80);

    /* ld hl, wEntitiesSpriteVariantTable; add hl, bc; dec [hl] */
    uint8_t variant = gb_read(gb, wEntitiesSpriteVariantTable + bc);
    gb_write(gb, wEntitiesSpriteVariantTable + bc, (uint8_t)(variant - 1));
}

/* ===== EntityInitMoblinSword (03:4EE2) ===== */
void EntityInitMoblinSword(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hActiveEntityPosX]; and $10; ld a, $00; jr nz, .jr_4EEC; ld a, $03 */
    uint8_t pos_x = gb_read_hram(gb, hActiveEntityPosX);
    uint8_t direction = (pos_x & 0x10) ? 0x00 : 0x03;

    /* .jr_4EEC: ld hl, wEntitiesDirectionTable; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesDirectionTable + bc, direction);

    /* push hl; call SetEntityVariantForDirection_03; pop hl */
    SetEntityVariantForDirection_03(gb, bc);

    /* ld a, [hl]; xor $01; ld [hl], a */
    uint8_t dir = gb_read(gb, wEntitiesDirectionTable + bc);
    gb_write(gb, wEntitiesDirectionTable + bc, dir ^ 0x01);
}

/* ===== EntityInitSecretSeashell (03:4EFB) ===== */
void EntityInitSecretSeashell(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], $02 */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0x02);

    /* ldh a, [hMapRoom]; cp UNKNOWN_ROOM_A4; jr z, .treeSeashell */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    if (map_room == UNKNOWN_ROOM_A4) {
        /* .treeSeashell: dec [hl] */
        gb_write(gb, wEntitiesPrivateState3Table + bc, 0x01);
        /* call EntityInitWithShiftedPosition */
        EntityShiftPosition(gb, bc);
        return;
    }

    /* cp UNKNOWN_ROOM_D2; jr nz, .treeSeashellEnd */
    if (map_room == UNKNOWN_ROOM_D2) {
        /* .treeSeashell: dec [hl] */
        gb_write(gb, wEntitiesPrivateState3Table + bc, 0x01);
        /* call EntityInitWithShiftedPosition */
        EntityShiftPosition(gb, bc);
        return;
    }

    /* .treeSeashellEnd: ret */
}

/* ===== func_003_4F12 (03:4F12) ===== */
void func_003_4F12(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], $01 */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0x01);

    /* ld a, [wIsIndoor]; and a; jr z, SetHiddenDroppableOptions1 */
    if (gb_read(gb, wIsIndoor) == 0) {
        SetHiddenDroppableOptions1(gb, bc);
        return;
    }

    /* fallthrough to SetHiddenDroppableOptions1 */
    SetHiddenDroppableOptions1(gb, bc);
}

/* ===== SetHiddenDroppableOptions1 (03:4F24) ===== */
void SetHiddenDroppableOptions1(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesOptions1Table; add hl, bc; ld a, [hl] */
    /* or ENTITY_OPT1_NO_GROUND_INTERACTION|ENTITY_OPT1_NO_WALL_COLLISION; ld [hl], a */
    uint8_t options1 = gb_read(gb, wEntitiesOptions1Table + bc);
    options1 |= (ENTITY_OPT1_NO_GROUND_INTERACTION | ENTITY_OPT1_NO_WALL_COLLISION);
    gb_write(gb, wEntitiesOptions1Table + bc, options1);
}

/* ===== EntityInitDiggableBushOrPotDroppable (03:4F1E) ===== */
void EntityInitDiggableBushOrPotDroppable(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], $02 */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0x02);

    /* fallthrough to SetHiddenDroppableOptions1 */
    SetHiddenDroppableOptions1(gb, bc);
}

/* ===== EntityInitKeyDropPoint (03:4F2D) ===== */
void EntityInitKeyDropPoint(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hMapRoom]; cp ROOM_INDOOR_A_QUICKSAND_CAVE; jr nz, .jr_4F44 */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    if (map_room != ROOM_INDOOR_A_QUICKSAND_CAVE) {
        goto jr_4F44;
    }

    /* ldh a, [hRoomStatus]; bit 4, a; jp nz, UnloadEntityAndReturn */
    if (gb_read_hram(gb, hRoomStatus) & 0x10) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* bit 5, a; jp z, UnloadEntityAndReturn */
    if ((gb_read_hram(gb, hRoomStatus) & 0x20) == 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* ld a, $02; jp SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, 0x02);
    return;

jr_4F44:
    /* cp ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_1; jr nz, .jr_4F54 */
    if (map_room != ROOM_INDOOR_B_MOUNTAIN_CAVE_ROOM_1) {
        goto jr_4F54;
    }

    /* ldh a, [hRoomStatus]; and ROOM_STATUS_EVENT_1; jp nz, UnloadEntityAndReturn */
    if (gb_read_hram(gb, hRoomStatus) & ROOM_STATUS_EVENT_1) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* ld a, $04; jp SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, 0x04);
    return;

jr_4F54:
    /* cp ROOM_INDOOR_A_ANGLERS_TUNNEL_KEY_FALL; jr nz, .ret_4F67 */
    if (map_room != ROOM_INDOOR_A_ANGLERS_TUNNEL_KEY_FALL) {
        goto ret_4F67;
    }

    /* ld a, [wIndoorARoomStatus + ROOM_INDOOR_A_ANGLERS_TUNNEL_KEY_DROP]; and $10; jp z, UnloadEntityAndReturn */
    if ((gb_read(gb, wIndoorARoomStatus + ROOM_INDOOR_A_ANGLERS_TUNNEL_KEY_DROP) & 0x10) == 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* ldh a, [hRoomStatus]; and ROOM_STATUS_EVENT_1; jp nz, UnloadEntityAndReturn */
    if (gb_read_hram(gb, hRoomStatus) & ROOM_STATUS_EVENT_1) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

ret_4F67:
    /* ret */
    return;
}

/* ===== EntityInitTradingItem (03:4F68) ===== */
void EntityInitTradingItem(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, [wTradeSequenceItem]; cp TRADING_ITEM_MAGNIFYING_LENS; jr z, EntityInitWithShiftedPosition */
    if (gb_read(gb, wTradeSequenceItem) == TRADING_ITEM_MAGNIFYING_LENS) {
        EntityShiftPosition(gb, bc);
        return;
    }

    /* ret */
}

/* ===== EntityInitWarp (03:4F70) ===== */
void EntityInitWarp(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, [wIsIndoor]; and a; ret z */
    if (gb_read(gb, wIsIndoor) == 0) {
        return;
    }

    /* call IncrementEntityState; jr EntityInitWithShiftedPosition */
    IncrementEntityState(gb, bc);
    EntityShiftPosition(gb, bc);
}

/* ===== EntityInitTreeOrPotDroppable (03:4F7A) ===== */
void EntityInitTreeOrPotDroppable(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call func_003_4F12 */
    func_003_4F12(gb, bc);

    /* ld a, [wIsIndoor]; and a; jr nz, SetDroppableDefaultTimer */
    if (gb_read(gb, wIsIndoor) != 0) {
        SetDroppableDefaultTimer(gb, bc);
        return;
    }

    /* fallthrough to EntityInitWithShiftedPosition (which is EntityShiftPosition) */
    EntityShiftPosition(gb, bc);
}

/* ===== EntityInitWithShiftedXPosition (03:4FA1) ===== */
void EntityInitWithShiftedXPosition(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld de, wEntitiesPosXSignTable; ld hl, wEntitiesPosXTable; jr EntityShiftPosition.shiftBy8 */
    EntityShiftPosition_shiftBy8(gb, bc, wEntitiesPosXSignTable, wEntitiesPosXTable);
}

/* ===== SetDroppableDefaultTimer (03:4FA9) ===== */
void SetDroppableDefaultTimer(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call GetEntitySlowTransitionCountdown; ld [hl], $80 */
    GetEntitySlowTransitionCountdown(gb, bc);
    gb_write(gb, wEntitiesSlowTransitionCountdownTable + bc, 0x80);
}

/* ===== EntityInitWithCountdown (03:4FAF) ===== */
void EntityInitWithCountdown(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetEntityPrivateCountdown1; ld [hl], $A0 */
    GetEntityPrivateCountdown1(gb, bc);
    gb_write(gb, wEntitiesPrivateCountdown1Table + bc, 0xA0);
}

/* ===== EntityInitGhini (03:4FB5) ===== */
void EntityInitGhini(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hActiveEntityType]; cp ENTITY_GHINI; jr nz, .hiding */
    if (gb_read_hram(gb, hActiveEntityType) != ENTITY_GHINI) {
        /* .hiding: jp IncrementEntityState */
        IncrementEntityState(gb, bc);
        return;
    }

    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], $01 */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0x01);

    /* ld hl, wEntitiesPosZTable; add hl, bc; ld [hl], $10 */
    gb_write(gb, wEntitiesPosZTable + bc, 0x10);
}
