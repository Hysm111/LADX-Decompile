#include "bank3/entities.h"
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

/* Forward declarations for functions called before definition */
void func_003_52D4(GBState *gb, uint16_t bc);

void ConfigureNewEntity(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call ResetEntity_trampoline */
    ResetEntity_trampoline(gb, NULL);

    /* Store the entity room id */
    /* ldh a, [hMapRoom]; ld hl, wEntitiesRoomTable; add hl, bc; ld [hl], a */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    gb_write(gb, wEntitiesRoomTable + bc, map_room);

    /* Set the entity load order to a default value */
    /* ld hl, wEntitiesLoadOrderTable; add hl, bc; ld [hl], $FF */
    gb_write(gb, wEntitiesLoadOrderTable + bc, 0xFF);

    /* .attributes: de = entity type */
    /* ld hl, wEntitiesTypeTable; add hl, bc; ld e, [hl]; ld d, b */
    uint8_t entity_type = gb_read(gb, wEntitiesTypeTable + bc);
    uint8_t d = (bc >> 8) & 0xFF;

    /* wEntitiesPhysicsFlagsTable = PhysicsFlagsForEntity[EntityType] */
    /* ld hl, PhysicsFlagsForEntity; add hl, de; ld a, [hl] */
    /* ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; ld [hl], a */
    uint8_t physics = gb->rom[PhysicsFlagsForEntity + entity_type];
    gb_write(gb, wEntitiesPhysicsFlagsTable + bc, physics);

    /* wEntitiesHitboxFlagsTable = HitboxFlagsForEntity[EntityType] */
    /* ld hl, HitboxFlagsForEntity; add hl, de; ld a, [hl] */
    /* ld hl, wEntitiesHitboxFlagsTable; add hl, bc; ld [hl], a */
    uint8_t hitbox = gb->rom[HitboxFlagsForEntity + entity_type];
    gb_write(gb, wEntitiesHitboxFlagsTable + bc, hitbox);

    /* call ConfigureEntityHealth */
    ConfigureEntityHealth(gb, bc, entity_type, d);

    /* ld hl, Options1ForEntity; add hl, de; ld a, [hl] */
    /* ld hl, wEntitiesOptions1Table; add hl, bc; ld [hl], a */
    uint8_t options1 = gb->rom[Options1ForEntity + entity_type];
    gb_write(gb, wEntitiesOptions1Table + bc, options1);

    /* jp ConfigureEntityHitbox */
    ConfigureEntityHitbox(gb, bc);
}

void ConfigureEntityHealth(GBState *gb, uint16_t bc, uint8_t entity_type, uint8_t d) {
    if (!gb) return;

    /* push de */
    /* e = HealthGroupForEntity[entity index] */
    /* ld hl, HealthGroupForEntity; add hl, de; ld e, [hl] */
    uint8_t health_group = gb->rom[HealthGroupForEntity + entity_type];

    /* wEntitiesHealthGroup[entity index] = e */
    /* ld hl, wEntitiesHealthGroup; add hl, bc; ld [hl], e */
    gb_write(gb, wEntitiesHealthGroup + bc, health_group);

    /* a = InitialHealthForGroup[health group] */
    /* ld d, b; ld hl, InitialHealthForGroup; add hl, de; ld a, [hl] */
    /* wEntitiesHealthTable[entity index] = a */
    uint8_t initial_health = gb->rom[InitialHealthForGroup + health_group];
    gb_write(gb, wEntitiesHealthTable + bc, initial_health);

    /* pop de; ret */
}

void EntityInitHandler(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* If the entity is a boss, and the room's boss is defeated, don't load the entity. */
    /* ld hl, wEntitiesOptions1Table; add hl, bc; ld a, [hl] */
    /* and ENTITY_OPT1_IS_BOSS; jr z, .callEntityInitHandler */
    uint8_t options1 = gb_read(gb, wEntitiesOptions1Table + bc);
    if ((options1 & ENTITY_OPT1_IS_BOSS) != 0) {
        /* ldh a, [hRoomStatus]; and ROOM_STATUS_EVENT_2|ROOM_STATUS_EVENT_1 */
        uint8_t room_status = gb_read_hram(gb, hRoomStatus);
        if ((room_status & (ROOM_STATUS_EVENT_2 | ROOM_STATUS_EVENT_1)) != 0) {
            UnloadEntityAndReturn(gb, bc);
            return;
        }
    }

    /* Special case for Master Stalfos */
    /* ldh a, [hActiveEntityType]; cp ENTITY_MASTER_STALFOS; jr nz, .masterStalfosEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_MASTER_STALFOS) {
        /* ldh a, [hMapRoom]; cp ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_1; jr z, .masterStalfosEnd */
        /* cp ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_2; jr z, .masterStalfosEnd */
        /* cp ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_3; jr z, .jr_003_48E2 */
        uint8_t map_room = gb_read_hram(gb, hMapRoom);
        if (map_room != ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_1 &&
            map_room != ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_2 &&
            map_room != ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_3) {

            /* Check room status for the three Master Stalfos rooms */
            /* ld a, [wIndoorARoomStatus + ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_3]; and $30; jr z, MasterStalfosDefeated */
            uint8_t status3 = gb_read(gb, wIndoorARoomStatus + ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_3);
            if ((status3 & 0x30) == 0) {
                MasterStalfosDefeated(gb);
                return;
            }

            /* ld a, [wIndoorARoomStatus + ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_2]; and $30; jr z, MasterStalfosDefeated */
            uint8_t status2 = gb_read(gb, wIndoorARoomStatus + ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_2);
            if ((status2 & 0x30) == 0) {
                MasterStalfosDefeated(gb);
                return;
            }

            /* ld a, [wIndoorARoomStatus + ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_1]; and $30; jr z, MasterStalfosDefeated */
            uint8_t status1 = gb_read(gb, wIndoorARoomStatus + ROOM_INDOOR_A_CATFISHS_MAW_MSTALFOS_1);
            if ((status1 & 0x30) == 0) {
                MasterStalfosDefeated(gb);
                return;
            }
        }
    }

    /* .masterStalfosEnd */

    /* ld a, [wIsIndoor]; and a; jr z, .indoorEnd */
    if (gb_read(gb, wIsIndoor) != 0) {
        /* ld a, [wD478]; and a; jr nz, .jr_003_490B */
        if (gb_read(gb, wD478) == 0) {
            /* ld hl, wEntitiesOptions1Table; add hl, bc; ld a, [hl] */
            /* and ENTITY_OPT1_IS_MINI_BOSS; jr z, .indoorEnd */
            if ((options1 & ENTITY_OPT1_IS_MINI_BOSS) != 0) {
                /* ld [wC1CF], a */
                gb_write(gb, wC1CF, options1);
            }
        }
    }

    /* .indoorEnd */
    /* call label_27F2 */
    label_27F2(gb);

    /* .jr_003_490B */
    /* xor a; ld [wDidBossIntro], a; inc a; ld [wInBossBattle], a */
    gb_write(gb, wDidBossIntro, 0);
    gb_write(gb, wInBossBattle, 1);

    /* ld a, $20; ld [wBossIntroDelay], a */
    gb_write(gb, wBossIntroDelay, 0x20);

    /* .callEntityInitHandler */

    /* Mark the entity as active */
    /* ld hl, wEntitiesStatusTable; add hl, bc; ld [hl], ENTITY_STATUS_ACTIVE */
    gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_ACTIVE);

    /* ld a, BANK(@); call GetEntityInitHandler_trampoline; jp hl */
    GetEntityInitHandler_trampoline(gb, 0x03, NULL);
}

void MasterStalfosDefeated(GBState *gb) {
    if (!gb) return;

    /* ld a, $01; ld [wRoomEventEffectExecuted], a */
    gb_write(gb, wRoomEventEffectExecuted, 0x01);

    /* jp UnloadEntityAndReturn */
    uint16_t bc = gb_read(gb, wActiveEntityIndex);
    UnloadEntityAndReturn(gb, bc);
}

void EntityInitHorsePiece(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesLoadOrderTable; add hl, bc; ld e, [hl]; ld d, b */
    uint8_t load_order = gb_read(gb, wEntitiesLoadOrderTable + bc);
    (void)bc; /* d = (bc >> 8) & 0xFF; unused in C */

    /* ld hl, Data_003_4924; add hl, de; ld a, [hl] */
    /* jp SetEntitySpriteVariant */
    static const uint8_t Data_003_4924[2] = { 0x01, 0x04 };
    uint8_t variant = Data_003_4924[load_order & 1];
    SetEntitySpriteVariant(gb, bc, variant);
}

void EntityInitMarinAtTalTalHeights(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPosYTable; add hl, bc; ld a, [hl]; sub $03; ld [hl], a; ret */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y - 0x03));
}

void EntityInitSnake(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetEntityPrivateCountdown1 */
    /* ld hl, wEntitiesPrivateCountdown1Table; add hl, bc; ld a, [hl]; and a; ret z */
    GetEntityPrivateCountdown1(gb, bc);

    /* ld [hl], $30 */
    gb_write(gb, wEntitiesPrivateCountdown1Table + bc, 0x30);
}

void EntityInitSideViewPlatformVertical(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hMapRoom]; cp UNKNOWN_ROOM_65; ret nz */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    if (map_room != UNKNOWN_ROOM_65) {
        return;
    }

    /* ldh a, [hActiveEntityVisualPosY]; cp $50; ret c */
    uint8_t visual_pos_y = gb_read_hram(gb, hActiveEntityVisualPosY);
    if (visual_pos_y < 0x50) {
        return;
    }

    /* ld hl, wEntitiesPrivateState1Table; add hl, bc; inc [hl]; ret */
    gb_write(gb, wEntitiesPrivateState1Table + bc, gb_read(gb, wEntitiesPrivateState1Table + bc) + 1);
}

void EntityInitZol(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesHealthTable; add hl, bc; ld [hl], $02; ret */
    gb_write(gb, wEntitiesHealthTable + bc, 0x02);
}

void EntityInitMarinAtTheShore(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wIsMarinInAnimalVillage; ld a, [wIsMarinFollowingLink]; or [hl]; jp nz, UnloadEntityAndReturn */
    uint8_t marin_in_village = gb_read(gb, wIsMarinInAnimalVillage);
    uint8_t marin_following = gb_read(gb, wIsMarinFollowingLink);
    if ((marin_in_village | marin_following) != 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* ret */
}

void EntityInitBomber(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPosZTable; add hl, bc; ld [hl], $10 */
    gb_write(gb, wEntitiesPosZTable + bc, 0x10);

    /* call GetRandomByte */
    uint8_t random = GetRandomByte(gb);

    /* ld hl, wEntitiesInertiaTable; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesInertiaTable + bc, random);
}

void EntityInitBushCrawler(GBState *gb) {
    if (!gb) return;

    /* ret */
    (void)gb;
}

void EntityInitTarinBeekeeper(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call EntityShiftPosition */
    EntityShiftPosition(gb, bc);

    /* ld a, $02; jp SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, 0x02);
}

void EntityInitTelephone(GBState *gb) {
    if (!gb) return;

    /* ld a, MUSIC_ULRIRA; jr SetMusicTrackIfHasSword */
    SetMusicTrackIfHasSword(gb, MUSIC_ULRIRA);
}

void EntityInitRichard(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, [wGoldenLeavesCount]; cp SLIME_KEY; jr c, .jr_003_4993 */
    uint8_t golden_leaves = gb_read(gb, wGoldenLeavesCount);
    if (golden_leaves < SLIME_KEY) {
        /* ld a, MUSIC_RICHARD_HOUSE */
        /* fallthrough to SetMusicTrackIfHasSword */
        SetMusicTrackIfHasSword(gb, MUSIC_RICHARD_HOUSE);
        return;
    }

    /* ld hl, wEntitiesPosXTable; add hl, bc; ld [hl], $58 */
    gb_write(gb, wEntitiesPosXTable + bc, 0x58);

    /* ld hl, wEntitiesDirectionTable; add hl, bc; ld [hl], DIRECTION_DOWN */
    gb_write(gb, wEntitiesDirectionTable + bc, DIRECTION_DOWN);

    /* .jr_003_4993: ld a, MUSIC_RICHARD_HOUSE */
    /* fallthrough to SetMusicTrackIfHasSword */
    SetMusicTrackIfHasSword(gb, MUSIC_RICHARD_HOUSE);
}

void SetMusicTrackIfHasSword(GBState *gb, uint8_t music_track) {
    if (!gb) return;

    /* ld e, a; ld a, [wSwordLevel]; and a; ret z */
    uint8_t sword_level = gb_read(gb, wSwordLevel);
    if (sword_level == 0) {
        return;
    }

    /* ld a, e; fallthrough to SetMusicTrack */
    SetMusicTrack(gb, music_track);
}

void SetMusicTrack(GBState *gb, uint8_t music_track) {
    if (!gb) return;

    /* ld [wMusicTrackToPlay], a */
    gb_write(gb, wMusicTrackToPlay, music_track);

    /* ldh [hDefaultMusicTrack], a */
    gb_write_hram(gb, hDefaultMusicTrack, music_track);

    /* ldh [hDefaultMusicTrackAlt], a */
    gb_write_hram(gb, hDefaultMusicTrackAlt, music_track);

    /* ldh [hNextDefaultMusicTrack], a */
    gb_write_hram(gb, hNextDefaultMusicTrack, music_track);
}

void EntityInitFinalNightmare(GBState *gb) {
    if (!gb) return;

    /* xor a; ld [wFinalNightmareForm], a; jp label_27F2 */
    gb_write(gb, wFinalNightmareForm, 0x00);
    label_27F2(gb);
}

void EntityInitDreamShrineBed(GBState *gb) {
    if (!gb) return;

    /* ld a, MUSIC_DREAM_SHRINE_BED; jr SetMusicTrack */
    SetMusicTrack(gb, MUSIC_DREAM_SHRINE_BED);
}

void EntityInitFishermanUnderBridge(GBState *gb) {
    if (!gb) return;

    /* ld a, MUSIC_FISHERMAN_UNDER_BRIDGE; jr SetMusicTrack */
    SetMusicTrack(gb, MUSIC_FISHERMAN_UNDER_BRIDGE);
}

void EntityInitKikiTheMonkey(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* xor a; ld [wC168], a */
    gb_write(gb, wC168, 0x00);

    /* ld hl, wEntitiesPosYTable; add hl, bc; ld a, [hl]; sub $04; ld [hl], a */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y - 0x04));
}

void EntityInitFireballShooter(GBState *gb) {
    if (!gb) return;

    /* call GetRandomByte; jp SetEntitySpriteVariant */
    uint8_t random = GetRandomByte(gb);
    uint16_t bc = gb_read(gb, wActiveEntityIndex);
    SetEntitySpriteVariant(gb, bc, random);
}

void EntityInitAntiKirby(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetEntitySlowTransitionCountdown */
    GetEntitySlowTransitionCountdown(gb, bc);

    /* call GetRandomByte */
    uint8_t random = GetRandomByte(gb);

    /* and $3F; add $10; ld [hl], a */
    random = (random & 0x3F) + 0x10;
    gb_write(gb, wEntitiesSlowTransitionCountdownTable + bc, random);
}

void EntityInitMovingBlockMover(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPosYTable; add hl, bc; ld a, [hl]; add $0A; ld [hl], a */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y + 0x0A));

    /* ld hl, wEntitiesPrivateState2Table; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesPrivateState2Table + bc, gb_read(gb, wEntitiesPosYTable + bc));
}

void EntityInitDesertLanmola(GBState *gb) {
    if (!gb) return;

    /* xor a; ldh [hDefaultMusicTrack], a; ret */
    gb_write_hram(gb, hDefaultMusicTrack, 0x00);
}

void EntityInitFloatingItem2(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call SetZPosForFloatingItem */
    SetZPosForFloatingItem(gb, bc);

    /* ldh a, [hActiveEntityPosX]; swap a; and $01; add $04; jp SetEntitySpriteVariant */
    uint8_t pos_x = gb_read_hram(gb, hActiveEntityPosX);
    uint8_t variant = ((pos_x >> 4) & 0x01) + 0x04;
    SetEntitySpriteVariant(gb, bc, variant);
}

void EntityInitFloatingItem(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hActiveEntityPosX]; swap a; and $01; ld e, a */
    uint8_t pos_x = gb_read_hram(gb, hActiveEntityPosX);
    uint8_t e = (pos_x >> 4) & 0x01;

    /* ldh a, [hActiveEntityPosY]; swap a; inc a; rla; and $02; or e */
    uint8_t pos_y = gb_read_hram(gb, hActiveEntityPosY);
    uint8_t a = ((pos_y >> 4) + 1) << 1;
    a = (a & 0x02) | e;

    /* call SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, a);

    /* cp $01; jr nz, SetZPosForFloatingItem */
    if (a != 0x01) {
        SetZPosForFloatingItem(gb, bc);
        return;
    }

    /* ld a, [wHasToadstool]; and a; jp nz, UnloadEntityAndReturn */
    if (gb_read(gb, wHasToadstool) != 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }
}

void SetZPosForFloatingItem(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesPosZTable; add hl, bc; ld [hl], $13; ret */
    gb_write(gb, wEntitiesPosZTable + bc, 0x13);
}

void EntityInitKid71(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesDirectionTable; add hl, bc; ld [hl], DIRECTION_UP */
    gb_write(gb, wEntitiesDirectionTable + bc, DIRECTION_UP);

    /* call IncrementEntityState */
    IncrementEntityState(gb, bc);

    /* call GetEntityTransitionCountdown; ld [hl], $20 */
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x20);
}

void EntityInitKid72(GBState *gb) {
    if (!gb) return;

    /* ret */
    (void)gb;
}

void EntityInitMrWrite(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hMapRoom]; cp ROOM_INDOOR_B_CHRISTINE_HOUSE; ld a, $32; jr nz, .jr_4A32; ld a, $37 */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    uint8_t a = 0x32;
    if (map_room == ROOM_INDOOR_B_CHRISTINE_HOUSE) {
        a = 0x37;
    }

    /* jr jr_003_4A4F */
    gb_write(gb, wMusicTrackToPlay, a);
    gb_write_hram(gb, hDefaultMusicTrack, a);
    gb_write_hram(gb, hDefaultMusicTrackAlt, a);
    gb_write_hram(gb, hNextDefaultMusicTrack, a);

    /* The function continues to EntityShiftPosition.shiftBy8 for X position */
    EntityShiftPosition_shiftBy8(gb, bc, wEntitiesPosXSignTable, wEntitiesPosXTable);
}

void EntityInitBigFairy(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);
    uint8_t a = 0x0C;

    /* ld hl, wEntitiesPosZTable; add hl, bc; ld [hl], $10 */
    gb_write(gb, wEntitiesPosZTable + bc, 0x10);

    /* ld a, [wIsIndoor]; and a; jr z, .indoorEnd */
    if (gb_read(gb, wIsIndoor) != 0) {
        /* ldh a, [hMapId]; cp MAP_COLOR_DUNGEON; jr z, jr_003_4A4D */
        if (gb_read_hram(gb, hMapId) == MAP_COLOR_DUNGEON) {
            goto jr_003_4A4D;
        }
        /* .indoorEnd: ld a, [wFullHearts]; and a; jp nz, UnloadEntityAndReturn */
        if (gb_read(gb, wFullHearts) != 0) {
            UnloadEntityAndReturn(gb, bc);
            return;
        }
    }

jr_003_4A4D:
    /* ld a, $0C */
    a = 0x0C;

    /* call SetMusicTrackIfHasSword */
    SetMusicTrackIfHasSword(gb, a);

    /* ld de, wEntitiesPosXSignTable; ld hl, wEntitiesPosXTable; jp EntityShiftPosition.shiftBy8 */
    EntityShiftPosition_shiftBy8(gb, bc, wEntitiesPosXSignTable, wEntitiesPosXTable);
}

void EntityInitBowWow(GBState *gb) {
    if (!gb) return;

    /* ldh a, [hMapRoom]; cp UNKNOWN_ROOM_E2; jr nz, .jr_4A6B */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    if (map_room != UNKNOWN_ROOM_E2) {
        goto jr_4A6B;
    }

    /* ld a, [wIsBowWowFollowingLink]; cp BOW_WOW_KIDNAPPED; jr z, .return; jp UnloadEntityAndReturn */
    uint8_t bowwow_following = gb_read(gb, wIsBowWowFollowingLink);
    if (bowwow_following == BOW_WOW_KIDNAPPED) {
        return;
    }
    uint16_t bc = gb_read(gb, wActiveEntityIndex);
    UnloadEntityAndReturn(gb, bc);
    return;

jr_4A6B:
    /* ld a, [wIsBowWowFollowingLink]; and a; jp nz, UnloadEntityAndReturn */
    if (gb_read(gb, wIsBowWowFollowingLink) != 0) {
        uint16_t bc = gb_read(gb, wActiveEntityIndex);
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* .return: ret */
}

void EntityInitOwlEvent(GBState *gb) {
    if (!gb) return;

    /* ldh a, [hRoomStatus]; rra; jr UnloadEntityIfRoomStatusSet */
    uint8_t room_status = gb_read_hram(gb, hRoomStatus);
    if (room_status & 0x10) { /* bit 4 after rra means bit 5 before */
        UnloadEntityIfRoomStatusSet(gb);
    }
}

void EntityInitSword(GBState *gb) {
    if (!gb) return;

    /* ldh a, [hRoomStatus]; fallthrough to UnloadEntityIfRoomStatusSet */
    /* and $10; jp nz, UnloadEntityAndReturn */
    if (gb_read_hram(gb, hRoomStatus) & 0x10) {
        UnloadEntityIfRoomStatusSet(gb);
    }
}

void UnloadEntityIfRoomStatusSet(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* and $10; jp nz, UnloadEntityAndReturn */
    if (gb_read_hram(gb, hRoomStatus) & 0x10) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* ret */
}

void EntityInitMarin(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hMapRoom]; cp UNKNOWN_ROOM_C0; jr c, .checkMarinDebug */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    if (map_room < UNKNOWN_ROOM_C0) {
        goto checkMarinDebug;
    }

    /* ld a, [wIsMarinInAnimalVillage]; and a; jp z, UnloadEntityAndReturn */
    if (gb_read(gb, wIsMarinInAnimalVillage) == 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* ld a, [wIsMarinFollowingLink]; and a; jp nz, UnloadEntityAndReturn */
    if (gb_read(gb, wIsMarinFollowingLink) != 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* inc a; ld [wIsMarinSinging], a */
    gb_write(gb, wIsMarinSinging, 0x01);

    /* ld a, MUSIC_MARIN_SING; ldh [hNextMusicTrackToFadeInto], a; ldh [hDefaultMusicTrack], a; ldh [hDefaultMusicTrackAlt], a; call ResetMusicFadeTimer */
    gb_write_hram(gb, hNextMusicTrackToFadeInto, MUSIC_MARIN_SING);
    gb_write_hram(gb, hDefaultMusicTrack, MUSIC_MARIN_SING);
    gb_write_hram(gb, hDefaultMusicTrackAlt, MUSIC_MARIN_SING);
    ResetMusicFadeTimer(gb);

checkMarinDebug:
    /* ld a, [ROM_DebugTool1]; and a; jp z, EntityInitNpcFacingDown */
    if (gb->rom[ROM_DebugTool1] == 0) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wName]; and a; jr nz, EntityInitNpcFacingDown */
    if (gb_read(gb, wName) != 0) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wName + 1]; and a; jr nz, .enableTextDebugger */
    if (gb_read(gb, wName + 1) != 0) {
        goto enableTextDebugger;
    }

    /* ld [wGameplaySubtype], a; ld a, GAMEPLAY_CREDITS; ld [wGameplayType], a; ret */
    gb_write(gb, wGameplaySubtype, 0x00);
    gb_write(gb, wGameplayType, GAMEPLAY_CREDITS);
    return;

enableTextDebugger:
    /* ld hl, wEntitiesTypeTable; add hl, bc; ld [hl], ENTITY_TEXT_DEBUGGER; ret */
    gb_write(gb, wEntitiesTypeTable + bc, ENTITY_TEXT_DEBUGGER);
}

void EntityInitTarin(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ldh a, [hIsGBC]; and a; jr z, EntityInitNpcFacingDown */
    if (gb_read_hram(gb, hIsGBC) == 0) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wIsIndoor]; and a; jr z, EntityInitNpcFacingDown */
    if (gb_read(gb, wIsIndoor) == 0) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wIsMarinFollowingLink]; and a; jr nz, EntityInitNpcFacingDown */
    if (gb_read(gb, wIsMarinFollowingLink) != 0) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wHasInstrument3]; and $02; jr nz, EntityInitNpcFacingDown */
    if (gb_read(gb, wHasInstrument3) & 0x02) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wTradeSequenceItem]; cp TRADING_ITEM_BANANAS; jr nc, EntityInitNpcFacingDown */
    if (gb_read(gb, wTradeSequenceItem) >= TRADING_ITEM_BANANAS) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wTarinFlag]; and a; jr z, EntityInitNpcFacingDown */
    if (gb_read(gb, wTarinFlag) == 0) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* cp $01; jr z, EntityInitNpcFacingDown */
    if (gb_read(gb, wTarinFlag) == 0x01) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, $02; ldh [rSVBK], a */
    /* ld hl, wObjPal8; ld de, Data_003_4AC6; loop: ld a, [de]; ld [hl+], a; inc de; ld a, l; and $07; jr nz, loop; xor a; ldh [rSVBK], a; jr EntityInitNpcFacingDown */
    static const uint8_t Data_003_4AC6[8] = { 0xFF, 0x7F, 0xBE, 0x0F, 0x13, 0x02, 0x00, 0x00 };
    for (int i = 0; i < 8; i++) {
        gb_write(gb, wObjPal8 + i, Data_003_4AC6[i]);
    }
    EntityInitNpcFacingDown(gb, bc);
}

void EntityInitMadamMeowMeow(GBState *gb) {
    if (!gb) return;

    /* ld a, [wIsBowWowFollowingLink]; cp BOW_WOW_KIDNAPPED; jr nz, .return */
    if (gb_read(gb, wIsBowWowFollowingLink) != BOW_WOW_KIDNAPPED) {
        return;
    }

    /* ld a, MUSIC_BOWWOW_KIDNAPPED; ld [wMusicTrackToPlay], a */
    gb_write(gb, wMusicTrackToPlay, MUSIC_BOWWOW_KIDNAPPED);

    /* .return: ret */
}

void EntityInitRaftRaftOwner(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, [wIsIndoor]; and a; jr nz, EntityInitNpcFacingDown */
    if (gb_read(gb, wIsIndoor) != 0) {
        EntityInitNpcFacingDown(gb, bc);
        return;
    }

    /* ld a, [wD477]; and a; ret nz */
    if (gb_read(gb, wD477) != 0) {
        return;
    }

    /* ld hl, wEntitiesPosYTable; add hl, bc; ld a, [hl]; sub $10; ld [hl], a; ret */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y - 0x10));
}

void EntityInitNpcFacingDown(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesDirectionTable; add hl, bc; ld [hl], DIRECTION_DOWN; fallthrough to EntityInitStoreOwner */
    gb_write(gb, wEntitiesDirectionTable + bc, DIRECTION_DOWN);
}

void EntityInitStoreOwner(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld a, [wShieldLevel]; and a; jr nz, .noShieldEnd; ld a, $1C; call SetMusicTrack */
    if (gb_read(gb, wShieldLevel) == 0) {
        SetMusicTrack(gb, 0x1C);
    }

    /* .noShieldEnd: jr EntityInitShopOwner.setDirectionLeft */
    /* falls through to EntityInitShopOwner.setDirectionLeft */
    EntityInitShopOwner_setDirectionLeft(gb, bc);
}

void EntityInitWitch(GBState *gb) {
    if (!gb) return;

    /* ret */
    (void)gb;
}

void EntityInitShopOwner(GBState *gb) {
    if (!gb) return;

    /* ld a, MUSIC_SHOP; call SetMusicTrackIfHasSword */
    SetMusicTrackIfHasSword(gb, MUSIC_SHOP);
}

void EntityInitShopOwner_setDirectionLeft(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* .setDirectionLeft: ld a, DIRECTION_LEFT; jr SetEntityDirection */
    SetEntityDirection(gb, bc, DIRECTION_LEFT);
}

void EntityInitWithRandomDirection(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetRandomByte; and $03; fallthrough to SetEntityDirection */
    uint8_t random = GetRandomByte(gb);
    SetEntityDirection(gb, bc, random & 0x03);
}

void SetEntityDirection(GBState *gb, uint16_t bc, uint8_t direction) {
    if (!gb) return;

    /* ld hl, wEntitiesDirectionTable; add hl, bc; ld [hl], a; fallthrough to EntityInitNoop */
    gb_write(gb, wEntitiesDirectionTable + bc, direction);
}

void EntityInitNoop(GBState *gb) {
    if (!gb) return;

    /* ret */
    (void)gb;
}

/* Entity Init Functions (03:4B57+) */

/* EntityInitSouthFaceShrineDoor (03:4B57) */
void EntityInitSouthFaceShrineDoor(GBState *gb) {
    if (!gb) return;

    /* ld a, IEF_STAT | IEF_VBLANK; ldh [rIE], a; ret */
    gb_write(gb, rIE, 0x03);
}

/* EntityInitLeever (03:4B5C) */
void EntityInitLeever(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, $FF; jp SetEntitySpriteVariant */
    SetEntitySpriteVariant(gb, bc, 0xFF);
}

/* EntityInitZora (03:4B61) */
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

/* EntityInitWithRightDirection (03:4B81) */
void EntityInitWithRightDirection(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* xor a; jr SetEntityDirection */
    SetEntityDirection(gb, bc, DIRECTION_RIGHT);
}

/* GetColorDungeonRoomStatus (03:4B84) */
uint8_t GetColorDungeonRoomStatus(GBState *gb) {
    if (!gb) return 0;

    /* ld hl, wColorDungeonRoomStatus; ldh a, [hMapRoom]; ld e, a; ld d, $00; add hl, de; ld a, [hl]; ret */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    return gb_read(gb, wColorDungeonRoomStatus + map_room);
}

/* EntityInitRotoswitchRed (03:4B8F) */
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

/* EntityInitRotoswitchYellow (03:4B9A) */
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

/* EntityInitRotoswitchBlue (03:4BA6) */
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

/* EntityInitHopper (03:4BB8) */
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

/* EntityInitFlyingHopperBombs (03:4BC0) */
void EntityInitFlyingHopperBombs(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, $04; fallthrough to .setPosZ */
    /* .setPosZ: ld hl, wEntitiesPosZTable; add hl, bc; ld [hl], $10; jp SetEntitySpriteVariant */
    gb_write(gb, wEntitiesPosZTable + bc, 0x10);
    SetEntitySpriteVariant(gb, bc, 0x04);
}

/* EntityInitHardHitBeetle (03:4BCB) */
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

/* EntityInitAvalaunch (03:4BDC) */
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

/* EntityInitColorGuardianBlue (03:4BEB) */
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

/* EntityInitColorGuardianRed (03:4C01) */
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

/* EntityInitColorDungeonBook (03:4C1F) */
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

/* EntityBurningHandler (03:4C4C) */
void EntityBurningHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call GetEntityTransitionCountdown; jr z, .burningEnd */
    uint8_t countdown = GetEntityTransitionCountdown(gb, bc);
    if (countdown == 0) {
        goto burningEnd;
    }

    /* Animate the entity burning with fire */
    /* ldh a, [hFrameCounter]; rra; rra; rra; and $01; ldh [hActiveEntitySpriteVariant], a */
    uint8_t frame = gb_read_hram(gb, hFrameCounter);
    uint8_t variant = (frame >> 3) & 0x01;
    gb_write_hram(gb, hActiveEntitySpriteVariant, variant);

    /* ld de, FireSpriteVariants; call RenderActiveEntitySpritesPair */
    static const uint8_t FireSpriteVariants[8] = {
        0x34, 0x02,  /* variant0: tile $34, palette 2 */
        0x34, 0x42,  /* variant0 flipped: tile $34, palette 2 | xflip */
        0x34, 0x04,  /* variant1: tile $34, palette 4 */
        0x34, 0x44   /* variant1 flipped: tile $34, palette 4 | xflip */
    };
    RenderActiveEntitySpritesPair(gb, FireSpriteVariants, NULL);

    /* ld hl, wEntitiesSpriteVariantTable; add hl, bc; ld a, [hl]; ldh [hActiveEntitySpriteVariant], a */
    uint8_t sprite_variant = gb_read(gb, wEntitiesSpriteVariantTable + bc);
    gb_write_hram(gb, hActiveEntitySpriteVariant, sprite_variant);

    /* call ExecuteActiveEntityHandler_trampoline */
    ExecuteActiveEntityHandler_trampoline(gb, NULL);

    /* call ReturnIfNonInteractive_03.allowInactiveEntity */
    if (ReturnIfNonInteractive_03(gb, true)) {
        return;
    }

    /* call ApplyRecoilIfNeeded_03 */
    ApplyRecoilIfNeeded_03(gb, bc);

    /* call BouncingEntityPhysics */
    BouncingEntityPhysics(gb, bc);

    /* call ClearEntitySpeed */
    ClearEntitySpeed(gb, bc);
    return;

burningEnd:
    /* If burning a Gibdo... */
    /* ldh a, [hActiveEntityType]; cp ENTITY_GIBDO; jr nz, gibdoEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GIBDO) {
        /* ... replace it by a Stalfos. */
        /* ld hl, wEntitiesTypeTable; add hl, bc; ld [hl], ENTITY_STALFOS_EVASIVE */
        gb_write(gb, wEntitiesTypeTable + bc, ENTITY_STALFOS_EVASIVE);
        /* ld hl, wEntitiesStatusTable; add hl, bc; ld [hl], ENTITY_STATUS_ACTIVE */
        gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_ACTIVE);
        /* jp ConfigureNewEntity.attributes */
        /* Note: ConfigureNewEntity.attributes is called via ConfigureEntityHitbox */
        ConfigureEntityHitbox(gb, bc);
        return;
    }

    /* ld hl, wEntitiesPrivateCountdown3Table; add hl, bc; ld [hl], $1F */
    gb_write(gb, wEntitiesPrivateCountdown3Table + bc, 0x1F);

    /* ld hl, wEntitiesStatusTable; add hl, bc; ld [hl], ENTITY_STATUS_DYING */
    gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_DYING);

    /* ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; ld [hl], 4 */
    gb_write(gb, wEntitiesPhysicsFlagsTable + bc, 4);

    /* ld hl, hNoiseSfx; ld [hl], NOISE_SFX_ENEMY_DESTROYED */
    gb_write_hram(gb, hNoiseSfx, NOISE_SFX_ENEMY_DESTROYED);
}

/* EntityFallHandler (03:4CB6) */
void EntityFallHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ldh a, [hMapId]; cp MAP_COLOR_DUNGEON; jr nz, colorShellEnd */
    if (gb_read_hram(gb, hMapId) != MAP_COLOR_DUNGEON) {
        goto colorShellEnd;
    }

    /* ld hl, wEntitiesTypeTable; add hl, bc; ld a, [hl] */
    uint8_t entity_type = gb_read(gb, wEntitiesTypeTable + bc);

    /* cp ENTITY_COLOR_SHELL_RED; jr z, animateColorShell */
    /* cp ENTITY_COLOR_SHELL_GREEN; jr z, animateColorShell */
    /* cp ENTITY_COLOR_SHELL_BLUE; jr z, animateColorShell */
    if (entity_type == ENTITY_COLOR_SHELL_RED ||
        entity_type == ENTITY_COLOR_SHELL_GREEN ||
        entity_type == ENTITY_COLOR_SHELL_BLUE) {
        /* animateColorShell */
        /* ld hl, wEntitiesStatusTable; add hl, bc; ld a, ENTITY_STATUS_ACTIVE; ld [hl], a */
        gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_ACTIVE);
        /* ld hl, wEntitiesStateTable; add hl, bc; ld a, $06; ld [hl], a; ret */
        gb_write(gb, wEntitiesStateTable + bc, 0x06);
        return;
    }

colorShellEnd: ;
    /* call GetEntityTransitionCountdown; jr nz, jr_003_4D07 */
    uint8_t countdown = GetEntityTransitionCountdown(gb, bc);
    if (countdown != 0) {
        goto jr_003_4D07;
    }

    /* ld hl, wEntitiesOptions1Table; add hl, bc; ld a, [hl]; and ENTITY_OPT1_EXCLUDED_FROM_KILL_ALL; jr nz, jr_4CEF */
    uint8_t options1 = gb_read(gb, wEntitiesOptions1Table + bc);
    if ((options1 & ENTITY_OPT1_EXCLUDED_FROM_KILL_ALL) == 0) {
        /* ld hl, wD460; ld [hl], $01 */
        gb_write(gb, wD460, 0x01);
    }

    /* ldh a, [hActiveEntityType]; cp ENTITY_WRECKING_BALL; jr nz, jr_4D04 */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_WRECKING_BALL) {
        /* ld a, $16; ld [wWreckingBallRoom], a */
        gb_write(gb, wWreckingBallRoom, 0x16);
        /* ld a, $50; ld [wWreckingBallPosX], a */
        gb_write(gb, wWreckingBallPosX, 0x50);
        /* ld a, $27; ld [wWreckingBallPosY], a */
        gb_write(gb, wWreckingBallPosY, 0x27);
    }

    /* jp UnloadEntityAndReturn */
    UnloadEntityAndReturn(gb, bc);
    return;

jr_003_4D07:
    /* cp $40; jr c, jr_003_4D29 */
    if (countdown < 0x40) {
        goto jr_003_4D29;
    }

    /* ldh a, [hActiveEntityType]; cp ENTITY_OCTOROK; jr z, jr_4D19 */
    /* cp ENTITY_MOBLIN; jr z, jr_4D19 */
    /* cp ENTITY_MOBLIN_SWORD; jr nz, jr_003_4D22 */
    entity_type = gb_read_hram(gb, hActiveEntityType);
    if (entity_type == ENTITY_OCTOROK ||
        entity_type == ENTITY_MOBLIN ||
        entity_type == ENTITY_MOBLIN_SWORD) {
        /* jr_4D19: call SetEntityVariantForDirection_03 x3 */
        SetEntityVariantForDirection_03(gb, bc);
        SetEntityVariantForDirection_03(gb, bc);
        SetEntityVariantForDirection_03(gb, bc);
    }

    /* call ExecuteActiveEntityHandler_trampoline */
    ExecuteActiveEntityHandler_trampoline(gb, NULL);

    /* call ReturnIfNonInteractive_03.allowInactiveEntity */
    if (ReturnIfNonInteractive_03(gb, true)) {
        return;
    }
    return;

jr_003_4D29: ;
    /* rra x4; and $03; ld hl, wEntitiesSpriteVariantTable; add hl, bc; ld [hl], a; ldh [hActiveEntitySpriteVariant], a */
    uint8_t variant = (countdown >> 4) & 0x03;
    gb_write(gb, wEntitiesSpriteVariantTable + bc, variant);
    gb_write_hram(gb, hActiveEntitySpriteVariant, variant);

    /* ld e, a; ld d, b; ld hl, Data_003_4CA4; add hl, de; ldh a, [hActiveEntityVisualPosY]; add [hl]; ldh [hActiveEntityVisualPosY], a */
    static const uint8_t Data_003_4CA4[4] = { 0x00, 0x00, 0x04, 0x00 };
    uint8_t visual_pos_y = gb_read_hram(gb, hActiveEntityVisualPosY);
    uint8_t offset = Data_003_4CA4[variant];
    gb_write_hram(gb, hActiveEntityVisualPosY, (uint8_t)(visual_pos_y + offset));

    /* ld a, e; cp $03; jr nz, jr_4D51 */
    if (variant != 0x03) {
        /* jr_4D51: ld de, Data_003_4CAC; call RenderActiveEntitySprite */
        static const uint8_t Data_003_4CAC[6] = { 0x24, 0x01, 0x24, 0x01, 0x3E, 0x01 };
        RenderActiveEntitySprite(gb, Data_003_4CAC, NULL);
    } else {
        /* xor a; ldh [hActiveEntitySpriteVariant], a; ld de, Unknown020SpriteVariants; call RenderActiveEntitySpritesPair */
        gb_write_hram(gb, hActiveEntitySpriteVariant, 0x00);
        static const uint8_t Unknown020SpriteVariants[4] = { 0x1E, 0x01, 0x1E, 0x61 };
        RenderActiveEntitySpritesPair(gb, Unknown020SpriteVariants, NULL);
    }

    /* jr jr_003_4D57 */
    /* jr_003_4D57: call ReturnIfNonInteractive_03.allowInactiveEntity */
    if (ReturnIfNonInteractive_03(gb, true)) {
        return;
    }

    /* call GetEntityTransitionCountdown; cp $3F; jr nz, jr_4D66 */
    countdown = GetEntityTransitionCountdown(gb, bc);
    if (countdown == 0x3F) {
        /* ld hl, hJingle; ld [hl], JINGLE_ITEM_FALLING */
        gb_write_hram(gb, hJingle, JINGLE_ITEM_FALLING);
    }

    /* rra x4; and $03; ld e, a; ld d, b; ld hl, Data_003_4CA8; add hl, de */
    variant = (countdown >> 4) & 0x03;
    static const uint8_t Data_003_4CA8[4] = { 0x00, 0x01, 0x03, 0x06 };
    /* Note: The assembly continues but we don't have the full implementation here.
       The function would continue with more sprite rendering logic. */
    (void)Data_003_4CA8; /* Suppress unused warning */
}

/* EntityThrownHandler (03:4D94) */
void EntityThrownHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call ExecuteActiveEntityHandler_trampoline */
    ExecuteActiveEntityHandler_trampoline(gb, NULL);

    /* call ReturnIfNonInteractive_03.allowInactiveEntity */
    if (ReturnIfNonInteractive_03(gb, true)) {
        return;
    }

    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld [hl], $02 */
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x02);

    /* call BouncingEntityPhysics */
    BouncingEntityPhysics(gb, bc);

    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld [hl], b */
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, 0);

    /* call BombEntityHandler.BounceOffWalls */
    /* Note: This calls a bank 4 function. For now, we'll stub it. */
    (void)gb;

    /* call EntityCheckThrowAtTriggers */
    /* Note: This calls a bank 3 function at 03:5438. For now, we'll stub it. */
    (void)gb;

    /* ldh a, [hActiveEntityType]; cp ENTITY_GENIE; jr nz, .genieEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GENIE) {
        /* ld hl, wEntitiesCollisionsTable; add hl, bc; ld a, [hl]; and a; jr z, .genieEnd */
        uint8_t collisions = gb_read(gb, wEntitiesCollisionsTable + bc);
        if (collisions != 0) {
            /* ld hl, wEntitiesFlashCountdownTable; add hl, bc; ld [hl], $20 */
            gb_write(gb, wEntitiesFlashCountdownTable + bc, 0x20);
            /* ld hl, hWaveSfx; ld [hl], WAVE_SFX_BOSS_HURT */
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_BOSS_HURT);
            /* ld hl, wEntitiesPrivateState4Table; add hl, bc; ld a, [hl]; inc a; ld [hl], a; cp $03; jr z, .genie2 */
            uint8_t private_state4 = gb_read(gb, wEntitiesPrivateState4Table + bc);
            private_state4++;
            gb_write(gb, wEntitiesPrivateState4Table + bc, private_state4);
            if (private_state4 == 0x03) {
                /* .genie2: fall through to set stunned */
                goto genie2;
            }
        }
        /* .genieEnd: */
    }

    /* ld a, DAMAGE_TYPE_THROW_AT; ld [wAttackDamageType], a */
    gb_write(gb, wAttackDamageType, DAMAGE_TYPE_THROW_AT);

    /* call func_003_75A2 */
    func_003_75A2(gb, bc);

    /* ld hl, wEntitiesSpeedXTable; add hl, bc; ld a, [hl] */
    /* ld hl, wEntitiesSpeedYTable; add hl, bc; or [hl]; jr nz, .return */
    uint8_t speed_x = gb_read(gb, wEntitiesSpeedXTable + bc);
    uint8_t speed_y = gb_read(gb, wEntitiesSpeedYTable + bc);
    if ((speed_x | speed_y) != 0) {
        return;
    }

    /* call EntityBecomeStunned */
    EntityBecomeStunned(gb, bc);

    /* ldh a, [hActiveEntityType]; cp ENTITY_GENIE; jr nz, .return */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GENIE) {
    genie2:
        /* ld hl, wEntitiesStatusTable; add hl, bc; ld [hl], $05 */
        gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_ACTIVE);
        /* call IncrementEntityState */
        IncrementEntityState(gb, bc);
        /* ld [hl], $01 */
        gb_write(gb, wEntitiesStateTable + bc, 0x01);
        /* call GetEntityTransitionCountdown; ld [hl], $80 */
        gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x80);
        /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], b */
        gb_write(gb, wEntitiesPrivateState3Table + bc, 0);
    }

    /* .return: ret */
}

/* EntityStunnedHandler (03:4E07) */
void EntityStunnedHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call ExecuteActiveEntityHandler_trampoline */
    ExecuteActiveEntityHandler_trampoline(gb, NULL);

    /* call ReturnIfNonInteractive_03.allowInactiveEntity */
    if (ReturnIfNonInteractive_03(gb, true)) {
        return;
    }

    /* call ApplyRecoilIfNeeded_03 */
    ApplyRecoilIfNeeded_03(gb, bc);

    /* call BouncingEntityPhysics */
    BouncingEntityPhysics(gb, bc);

    /* call ClearEntitySpeed */
    ClearEntitySpeed(gb, bc);

    /* call func_003_6E2B */
    func_003_6E2B(gb, bc);

    /* ld a, [wInventoryItems.BButtonSlot]; cp INVENTORY_POWER_BRACELET; jr nz, .noBraceletB */
    uint8_t b_button = gb_read(gb, wInventoryItems_BButtonSlot);
    if (b_button == INVENTORY_POWER_BRACELET) {
        /* ldh a, [hJoypadState]; and J_B; jr nz, EntityGetLiftedUp */
        uint8_t joypad = gb_read_hram(gb, hJoypadState);
        if (joypad & J_B) {
            EntityGetLiftedUp(gb, bc);
            return;
        }
        goto jr_003_4E72;
    }

    /* .noBraceletB */
    /* ld a, [wInventoryItems.AButtonSlot]; cp INVENTORY_POWER_BRACELET; jr nz, jr_003_4E72 */
    uint8_t a_button = gb_read(gb, wInventoryItems_AButtonSlot);
    if (a_button == INVENTORY_POWER_BRACELET) {
        /* ldh a, [hJoypadState]; and J_A; jr z, jr_003_4E72 */
        uint8_t joypad = gb_read_hram(gb, hJoypadState);
        if (joypad & J_A) {
            /* fallthrough to EntityGetLiftedUp */
            EntityGetLiftedUp(gb, bc);
            return;
        }
    }

jr_003_4E72: ;
    /* ld hl, wEntitiesPrivateCountdown2Table; add hl, bc; ld a, [hl]; and a; jr nz, .jr_4E85 */
    uint8_t countdown2 = gb_read(gb, wEntitiesPrivateCountdown2Table + bc);
    if (countdown2 == 0) {
        /* ld hl, wEntitiesStatusTable; add hl, bc; ld [hl], $05 */
        gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_ACTIVE);
        /* ld hl, wEntitiesSpeedZTable; add hl, bc; ld [hl], b */
        gb_write(gb, wEntitiesSpeedZTable + bc, 0);
    }

    /* .jr_4E85: cp $38; ret nc */
    if (countdown2 >= 0x38) {
        return;
    }

    /* srl a; srl a; and $01; ld e, a; ld d, b; ld hl, Data_003_4E05; add hl, de; ld a, [hl] */
    uint8_t variant = (countdown2 >> 2) & 0x01;
    static const uint8_t Data_003_4E05[2] = { 0x10, 0xF0 };
    int8_t speed = (int8_t)Data_003_4E05[variant];

    /* ld hl, wEntitiesSpeedXTable; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesSpeedXTable + bc, (uint8_t)speed);

    /* call AddEntitySpeedToPos_03 */
    AddEntitySpeedToPos_03(gb, bc);

    /* jp ClearEntitySpeed */
    ClearEntitySpeed(gb, bc);
}

/* EntityGetLiftedUp (03:4E35) */
void EntityGetLiftedUp(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld a, [wC3CF]; and a; jr nz, jr_003_4E72 */
    if (gb_read(gb, wC3CF) != 0) {
        return;
    }

    /* ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; ld a, [hl]; push hl; push af */
    uint8_t physics = gb_read(gb, wEntitiesPhysicsFlagsTable + bc);

    /* or ENTITY_PHYSICS_HARMLESS; ld [hl], a */
    gb_write(gb, wEntitiesPhysicsFlagsTable + bc, physics | ENTITY_PHYSICS_HARMLESS);

    /* call CheckLinkCollisionWithEnemy */
    bool collision = CheckLinkCollisionWithEnemy(gb, bc);

    /* rl e; pop af; pop hl; ld [hl], a; rr e; jr nc, jr_003_4E72 */
    gb_write(gb, wEntitiesPhysicsFlagsTable + bc, physics);
    if (!collision) {
        return;
    }

    /* ld a, $01; ld [wC3CF], a */
    gb_write(gb, wC3CF, 0x01);

    /* ld hl, wEntitiesStatusTable; add hl, bc; ld [hl], $07 */
    gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_LIFTED);

    /* ld a, WAVE_SFX_LIFT_UP; ldh [hWaveSfx], a */
    gb_write_hram(gb, hWaveSfx, WAVE_SFX_LIFT_UP);

    /* ld hl, wEntitiesLiftedTable; add hl, bc; ld [hl], b */
    gb_write(gb, wEntitiesLiftedTable + bc, 0);

    /* call GetEntityTransitionCountdown; ld [hl], $02 */
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x02);

    /* ldh a, [hLinkDirection]; ld [wC15D], a */
    uint8_t link_dir = gb_read_hram(gb, hLinkDirection);
    gb_write(gb, wC15D, link_dir);

    /* jp EntityLiftedHandler */
    EntityLiftedHandler(gb, bc);
}

/* EntityLiftedHandler (03:5732) */
void EntityLiftedHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* Implementation based on bank3.asm:03:5732 */
    /* This is a stub - the full implementation is in bank3.asm at 03:5732 */
    (void)bc;
}

/* EntityBecomeStunned (03:7267) */
void EntityBecomeStunned(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesStatusTable; add hl, bc; ld [hl], ENTITY_STATUS_STUNNED */
    gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_STUNNED);

    /* ld hl, wEntitiesPrivateCountdown2Table; add hl, bc; ld [hl], $FF */
    gb_write(gb, wEntitiesPrivateCountdown2Table + bc, 0xFF);

    /* ld hl, wEntitiesSpeedZTable; add hl, bc; ld [hl], b */
    gb_write(gb, wEntitiesSpeedZTable + bc, 0);

    /* ret */
}

/* Entity Init Functions (03:4EA8+) */

/* EntityInitWithRandomSpeed (03:4EA8) */
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

/* EntityInitSparkClockwise (03:4EC4) */
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

/* EntityInitSparkCounterClockwise (03:4ECE) */
void EntityInitSparkCounterClockwise(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld a, $FD; jr_003_4ED0: ld hl, wEntitiesPosYTable; add hl, bc; add [hl]; ld [hl], a */
    uint8_t pos_y = gb_read(gb, wEntitiesPosYTable + bc);
    gb_write(gb, wEntitiesPosYTable + bc, (uint8_t)(pos_y + 0xFD));  /* -3 */
}

/* EntityInitWizrobe (03:4ED7) */
void EntityInitWizrobe(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetEntityTransitionCountdown; ld [hl], $80 */
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x80);

    /* ld hl, wEntitiesSpriteVariantTable; add hl, bc; dec [hl] */
    uint8_t variant = gb_read(gb, wEntitiesSpriteVariantTable + bc);
    gb_write(gb, wEntitiesSpriteVariantTable + bc, (uint8_t)(variant - 1));
}

/* EntityInitMoblinSword (03:4EE2) */
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

/* EntityInitSecretSeashell (03:4EFB) */
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

/* Helper functions (03:4F12+) */

/* func_003_4F12 (03:4F12) */
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

/* SetHiddenDroppableOptions1 (03:4F24) */
void SetHiddenDroppableOptions1(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesOptions1Table; add hl, bc; ld a, [hl] */
    /* or ENTITY_OPT1_NO_GROUND_INTERACTION|ENTITY_OPT1_NO_WALL_COLLISION; ld [hl], a */
    uint8_t options1 = gb_read(gb, wEntitiesOptions1Table + bc);
    options1 |= (ENTITY_OPT1_NO_GROUND_INTERACTION | ENTITY_OPT1_NO_WALL_COLLISION);
    gb_write(gb, wEntitiesOptions1Table + bc, options1);
}

/* EntityInitDiggableBushOrPotDroppable (03:4F1E) */
void EntityInitDiggableBushOrPotDroppable(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], $02 */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0x02);

    /* fallthrough to SetHiddenDroppableOptions1 */
    SetHiddenDroppableOptions1(gb, bc);
}

/* EntityInitKeyDropPoint (03:4F2D) */
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

/* EntityInitTradingItem (03:4F68) */
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

/* EntityInitWarp (03:4F70) */
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

/* EntityInitTreeOrPotDroppable (03:4F7A) */
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

/* EntityInitWithShiftedPosition is EntityShiftPosition (03:4F83) - already implemented */
/* EntityInitWithShiftedXPosition (03:4FA1) */
void EntityInitWithShiftedXPosition(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld de, wEntitiesPosXSignTable; ld hl, wEntitiesPosXTable; jr EntityShiftPosition.shiftBy8 */
    EntityShiftPosition_shiftBy8(gb, bc, wEntitiesPosXSignTable, wEntitiesPosXTable);
}

/* SetDroppableDefaultTimer (03:4FA9) */
void SetDroppableDefaultTimer(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call GetEntitySlowTransitionCountdown; ld [hl], $80 */
    GetEntitySlowTransitionCountdown(gb, bc);
    gb_write(gb, wEntitiesSlowTransitionCountdownTable + bc, 0x80);
}

/* EntityInitWithCountdown (03:4FAF) */
void EntityInitWithCountdown(GBState *gb) {
    if (!gb) return;

    uint16_t bc = gb_read(gb, wActiveEntityIndex);

    /* call GetEntityPrivateCountdown1; ld [hl], $A0 */
    GetEntityPrivateCountdown1(gb, bc);
    gb_write(gb, wEntitiesPrivateCountdown1Table + bc, 0xA0);
}

/* EntityInitGhini (03:4FB5) */
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

/* EntityShiftPosition (03:4F83) */
void EntityShiftPosition(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld de, wEntitiesPosXSignTable; ld hl, wEntitiesPosXTable; call .shiftBy8 */
    EntityShiftPosition_shiftBy8(gb, bc, wEntitiesPosXSignTable, wEntitiesPosXTable);

    /* ld de, wEntitiesPosYSignTable; ld hl, wEntitiesPosYTable; fallthrough to .shiftBy8 */
    EntityShiftPosition_shiftBy8(gb, bc, wEntitiesPosYSignTable, wEntitiesPosYTable);
}

/* EntityShiftPosition.shiftBy8 (03:4F92) */
void EntityShiftPosition_shiftBy8(GBState *gb, uint16_t bc, uint16_t sign_table, uint16_t pos_table) {
    if (!gb) return;

    /* add hl, bc; ld a, [hl]; add $08; ld [hl], a */
    uint8_t pos = gb_read(gb, pos_table + bc);
    uint8_t new_pos = pos + 0x08;
    gb_write(gb, pos_table + bc, new_pos);

    /* rla; ld l, e; ld h, d; add hl, bc; rra; ld a, [hl]; adc $00; ld [hl], a */
    uint8_t carry = (pos >= 0xF8) ? 1 : 0;  /* carry from add $08 */
    uint8_t sign = gb_read(gb, sign_table + bc);
    uint8_t new_sign = sign + carry;
    gb_write(gb, sign_table + bc, new_sign);
}

/* PushedBlockEntityHandler (03:5249) */
void PushedBlockEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld a, [wIsIndoor]; ldh [hActiveEntitySpriteVariant], a */
    uint8_t is_indoor = gb_read(gb, wIsIndoor);
    gb_write_hram(gb, hActiveEntitySpriteVariant, is_indoor);

    /* ld de, Unknown011SpriteVariants */
    /* and a; jr nz, .render */
    static const uint8_t Unknown011SpriteVariants[8] = {
        0x6E, 0xC7,  /* variant0: tile $6E, palette 7 | OAMF_PAL0 */
        0x6E, 0xC7 | OAMF_XFLIP,  /* variant0 flipped */
        0xF8, 0xE7,  /* variant1: tile $F8, palette 7 | OAMF_PAL1 */
        0xFA, 0xE7   /* variant1 flipped */
    };
    static const uint8_t Unknown010SpriteVariants[4] = {
        0x7E, 0xC7,  /* variant0: tile $7E, palette 7 | OAMF_PAL0 */
        0x7E, 0xC7 | OAMF_XFLIP
    };

    const uint8_t *de = Unknown011SpriteVariants;
    if (is_indoor != 0) {
        goto render;
    }

    /* ldh a, [hMapRoom]; cp ROOM_OW_COLOR_DUNGEON_ENTRANCE; jr nz, .render */
    if (gb_read_hram(gb, hMapRoom) == ROOM_OW_COLOR_DUNGEON_ENTRANCE) {
        de = Unknown010SpriteVariants;
    }

render:
    /* call RenderActiveEntitySpritesPair */
    RenderActiveEntitySpritesPair(gb, de, NULL);

    /* call ReturnIfNonInteractive_03 */
    if (ReturnIfNonInteractive_03(gb, false)) {
        return;
    }

    /* call UpdateEntityPosWithSpeed_03 */
    UpdateEntityPosWithSpeed_03(gb, bc);

    /* call func_003_52D4 */
    func_003_52D4(gb, bc);

    /* call CheckLinkCollisionWithEnemy.collisionEvenInTheAir; jr nc, .jr_5276 */
    if (CheckLinkCollisionWithEnemy(gb, bc)) {
        /* call CopyLinkFinalPositionToPosition */
        CopyLinkFinalPositionToActivePosition(gb);

        /* ld a, $03; ld [wIsLinkPushing], a */
        gb_write(gb, wIsLinkPushing, 0x03);
    }

    /* .jr_5276: */
    /* ldh a, [hMapRoom]; cp UNKNOWN_ROOM_C7; jr z, .jr_5282 */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    if (map_room == UNKNOWN_ROOM_C7) {
        goto jr_5282;
    }

    /* ld a, [wIsIndoor]; and a; jr nz, jr_003_5286 */
    if (gb_read(gb, wIsIndoor) != 0) {
        goto jr_003_5286;
    }

jr_5282:
    /* ld a, $02; ldh [hLinkInteractiveMotionBlocked], a */
    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x02);

jr_003_5286: ;
    /* ld hl, wEntitiesInertiaTable; add hl, bc; ld a, [hl]; inc a; ld [hl], a; cp $21; ret nz */
    uint8_t inertia = gb_read(gb, wEntitiesInertiaTable + bc);
    inertia++;
    gb_write(gb, wEntitiesInertiaTable + bc, inertia);
    if (inertia != 0x21) {
        return;
    }

    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld [hl], a */
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, inertia);

    /* call ApplyEntityInteractionWithBackground */
    ApplyEntityInteractionWithBackground(gb, bc);

    /* ld hl, wEntitiesStatusTable; add hl, bc; ld a, [hl]; and a; ret z */
    uint8_t status = gb_read(gb, wEntitiesStatusTable + bc);
    if (status == 0) {
        return;
    }

    /* cp $02; ret z */
    if (status == 0x02) {
        return;
    }

    /* call UnloadEntity */
    UnloadEntity(gb, bc);

    /* ld de, Data_003_5162; ld b, $C4 */
    /* ld a, [wIsIndoor]; and a; jr z, .jr_52B5 */
    /* ld de, Data_003_515E; ld b, $A6 */
    static const uint8_t Data_003_5162[4] = { 0xF8, 0xF9, 0xFA, 0xFB };
    static const uint8_t Data_003_515E[4] = { 0x0E, 0x1E, 0x0F, 0x1F };
    const uint8_t *data_ptr = Data_003_5162;
    uint8_t b_val = 0xC4;
    if (gb_read(gb, wIsIndoor) == 0) {
        data_ptr = Data_003_515E;
        b_val = 0xA6;
    }

    /* call func_003_51C9 */
    func_003_51C9(gb, bc, data_ptr, b_val);

    /* ld a, [wRoomEvent]; and EVENT_TRIGGER_MASK; cp TRIGGER_PUSH_SINGLE_BLOCK; jr z, .jr_52D1 */
    uint8_t room_event = gb_read(gb, wRoomEvent) & EVENT_TRIGGER_MASK;
    if (room_event == TRIGGER_PUSH_SINGLE_BLOCK) {
        goto jr_52D1;
    }

    /* cp TRIGGER_PUSH_BLOCKS; ret nz */
    if (room_event != TRIGGER_PUSH_BLOCKS) {
        return;
    }

    /* call ApplyEntityInteractionWithBackground */
    ApplyEntityInteractionWithBackground(gb, bc);

    /* ld a, [wEntityHorizontallyCollidedObject]; cp $A7; jr z, .jr_52D1 */
    uint8_t collided_obj = gb_read(gb, wEntityHorizontallyCollidedObject);
    if (collided_obj == 0xA7) {
        goto jr_52D1;
    }

    /* cp $A6; ret nz */
    if (collided_obj != 0xA6) {
        return;
    }

jr_52D1:
    /* jp MarkTriggerAsResolved */
    MarkTriggerAsResolved(gb);
}

/* func_003_52D4 (03:52D4) */
void func_003_52D4(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld e, $0F; ld d, b; label_003_52D7: */
    uint8_t e = 0x0F;
    uint8_t d = (bc >> 8) & 0xFF;

    while (true) {
        /* ld hl, wEntitiesStatusTable; add hl, de; ld a, [hl]; cp $05; jr c, .jr_531E */
        uint8_t status = gb_read(gb, wEntitiesStatusTable + e);
        if (status < 0x05) {
            goto jr_531E;
        }

        /* ld hl, wEntitiesPhysicsFlagsTable; add hl, de; ld a, [hl]; and ENTITY_PHYSICS_PROJECTILE_NOCLIP; jr nz, .jr_531E */
        uint8_t physics = gb_read(gb, wEntitiesPhysicsFlagsTable + e);
        if (physics & ENTITY_PHYSICS_PROJECTILE_NOCLIP) {
            goto jr_531E;
        }

        /* ld hl, wEntitiesPosXTable; add hl, de; ldh a, [hActiveEntityPosX]; sub [hl]; add $0C; cp $18; jr nc, .jr_531E */
        uint8_t entity_pos_x = gb_read(gb, wEntitiesPosXTable + e);
        uint8_t active_pos_x = gb_read_hram(gb, hActiveEntityPosX);
        int16_t diff_x = (int16_t)active_pos_x - entity_pos_x + 0x0C;
        if ((uint8_t)diff_x >= 0x18) {
            goto jr_531E;
        }

        /* ld hl, wEntitiesPosYTable; add hl, de; ld a, [hl]; ld hl, wEntitiesPosZTable; add hl, de; sub [hl]; ld hl, hActiveEntityVisualPosY; sub [hl]; add $0C; cp $18; jr nc, .jr_531E */
        uint8_t entity_pos_y = gb_read(gb, wEntitiesPosYTable + e);
        uint8_t entity_pos_z = gb_read(gb, wEntitiesPosZTable + e);
        uint8_t visual_pos_y = gb_read_hram(gb, hActiveEntityVisualPosY);
        int16_t diff_y = (int16_t)entity_pos_y - entity_pos_z - visual_pos_y + 0x0C;
        if ((uint8_t)diff_y >= 0x18) {
            goto jr_531E;
        }

        /* ld hl, wEntitiesPhysicsFlagsTable; add hl, de; ld a, [hl]; and ENTITY_PHYSICS_GRABBABLE; jr nz, .jr_531E */
        if (physics & ENTITY_PHYSICS_GRABBABLE) {
            goto jr_531E;
        }

        /* push bc; ld c, e; ld b, d; push de; ld a, $08; call ConfigureEntityRecoil; pop de; pop bc */
        uint16_t other_bc = ((uint16_t)d << 8) | e;
        ConfigureEntityRecoil(gb, other_bc, 0x08);

jr_531E:
        /* dec e; ld a, e; cp $FF; jp nz, label_003_52D7 */
        if (e == 0) {
            break;
        }
        e--;
    }

    return;
}

/* Entity4BHandler (03:5326) */
void Entity4BHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld d, $03; fallthrough to LiftableRockEntityHandler */
    /* This is a special entry point that sets d=3 before calling LiftableRockEntityHandler */
    (void)gb;  /* d register is handled in LiftableRockEntityHandler */
    LiftableRockEntityHandler(gb, bc);
}

/* LiftableRockEntityHandler (03:5328) */
void LiftableRockEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld a, c; ld [wPickedUpRockIndex], a */
    uint8_t c = bc & 0xFF;
    gb_write(gb, wPickedUpRockIndex, c);

    /* call GetEntityPrivateCountdown1; ldh [hMultiPurpose0], a; jp z, LiftableRockIntactHandler */
    uint8_t countdown1 = GetEntityPrivateCountdown1(gb, bc);
    gb_write_hram(gb, hMultiPurpose0, countdown1);
    if (countdown1 == 0) {
        LiftableRockIntactHandler(gb, bc);
        return;
    }

    /* cp $01; jr nz, jr_003_5395 */
    if (countdown1 != 0x01) {
        goto jr_003_5395;
    }

    /* Last frame */
    /* ld hl, wEntitiesPrivateState5Table; add hl, bc; ld a, [hl]; and a; jr z, .spawnFairyEnd */
    uint8_t private_state5 = gb_read(gb, wEntitiesPrivateState5Table + bc);
    if (private_state5 != 0) {
        /* call GetRandomByte; and $03; jr nz, .spawnFairyEnd */
        uint8_t random = GetRandomByte(gb) & 0x03;
        if (random == 0) {
            /* ld a, ENTITY_DROPPABLE_FAIRY; call SpawnNewEntity; jr c, .spawnFairyEnd */
            uint16_t de = SpawnNewEntity_trampoline(gb, ENTITY_DROPPABLE_FAIRY, NULL);
            if (de != 0xFFFF) {
                /* ldh a, [hMultiPurpose0]; ld hl, wEntitiesPosXTable; add hl, de; ld [hl], a */
                gb_write(gb, wEntitiesPosXTable + de, gb_read_hram(gb, hMultiPurpose0));
                /* ldh a, [hMultiPurpose1]; ld hl, wEntitiesPosYTable; add hl, de; ld [hl], a */
                gb_write(gb, wEntitiesPosYTable + de, gb_read_hram(gb, hMultiPurpose1));
                /* ldh a, [hMultiPurpose3]; ld hl, wEntitiesPosZTable; add hl, de; ld [hl], a */
                gb_write(gb, wEntitiesPosZTable + de, gb_read_hram(gb, hMultiPurpose3));
                /* ld hl, wEntitiesSlowTransitionCountdownTable; add hl, de; ld [hl], $80 */
                gb_write(gb, wEntitiesSlowTransitionCountdownTable + de, 0x80);
            }
        }
    }

    /* ldh a, [hActiveEntitySpriteVariant]; and a; jr nz, .marinReactionEnd */
    if (gb_read_hram(gb, hActiveEntitySpriteVariant) != 0) {
        goto marinReactionEnd;
    }

    /* If inside a house... ldh a, [hMapId]; cp MAP_GHOST_HOUSE; jr z, .insideHouse */
    /* cp MAP_HOUSE; jr nz, .marinReactionEnd */
    uint8_t map_id = gb_read_hram(gb, hMapId);
    if (map_id == MAP_GHOST_HOUSE || map_id == MAP_HOUSE) {
        /* ... and Marin is following Link... ld a, [wIsMarinFollowingLink]; and a; jr z, .marinReactionEnd */
        if (gb_read(gb, wIsMarinFollowingLink) != 0) {
            /* draw a random number; and $3F; jr nz, .marinReaction2 */
            uint8_t random = GetRandomByte(gb) & 0x3F;
            if (random == 0) {
                /* Open Marin reaction 1 (Dialog028) */
                OpenDialogInTable0_trampoline(gb, 0x28);
                /* jp UnloadEntityAndReturn */
                UnloadEntityAndReturn(gb, bc);
                return;
            }
            /* .marinReaction2: Open Marin reaction 2 (Dialog199) */
            OpenDialogInTable0_trampoline(gb, 0xC7);  /* Dialog199 */
        }
    }

    /* fall through to marinReactionEnd */
    /* jp UnloadEntityAndReturn */
    UnloadEntityAndReturn(gb, bc);
    return;

marinReactionEnd:
    /* jp UnloadEntityAndReturn */
    UnloadEntityAndReturn(gb, bc);
    return;

jr_003_5395:
    /* jp label_3935 */
    label_3935(gb, NULL);
}

/* LiftableRockIntactHandler (03:53A8) */
void LiftableRockIntactHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* Select sprite variants based on indoor/outdoor */
    static const uint8_t LiftableRockOutdoorSpriteVariants[8] = {
        0xF0, 0xE7, 0xF2, 0xE7,  /* variant0: Rock */
        0xF4, 0xE6, 0xF6, 0xE6   /* variant1: Bush */
    };
    static const uint8_t LiftableRockIndoorSpriteVariants[8] = {
        0xF0, 0xE6, 0xF2, 0xE6,  /* variant0: Pot/Skull */
        0xF4, 0xE6, 0xF6, 0xE6   /* variant1: Unused? */
    };

    const uint8_t *de = LiftableRockOutdoorSpriteVariants;
    if (gb_read(gb, wIsIndoor) != 0) {
        de = LiftableRockIndoorSpriteVariants;
    }

    /* .render: call RenderActiveEntitySpritesPair */
    RenderActiveEntitySpritesPair(gb, de, NULL);

    /* call ReturnIfNonInteractive_03 */
    if (ReturnIfNonInteractive_03(gb, false)) {
        return;
    }

    /* ld a, DAMAGE_TYPE_THROW_AT; ld [wAttackDamageType], a; call func_003_75A2 */
    gb_write(gb, wAttackDamageType, DAMAGE_TYPE_THROW_AT);
    func_003_75A2(gb, bc);

    /* call BouncingEntityPhysics */
    BouncingEntityPhysics(gb, bc);

    /* ld hl, wEntitiesStatusTable; add hl, bc; ld a, [hl]; cp ENTITY_STATUS_FALLING; jp z, ret_003_5406 */
    uint8_t status = gb_read(gb, wEntitiesStatusTable + bc);
    if (status == ENTITY_STATUS_FALLING) {
        return;
    }

    /* ld hl, wEntitiesPosZTable; add hl, bc; ld a, [hl]; and a; jr z, LiftableRockStartSmashingAnimation */
    uint8_t pos_z = gb_read(gb, wEntitiesPosZTable + bc);
    if (pos_z == 0) {
        LiftableRockStartSmashingAnimation(gb, bc);
        return;
    }

    /* ld hl, wEntitiesCollisionsTable; add hl, bc; ld a, [hl]; and a; jr z, ret_003_5406 */
    uint8_t collisions = gb_read(gb, wEntitiesCollisionsTable + bc);
    if (collisions == 0) {
        return;
    }

    /* call EntityCheckThrowAtTriggers; fallthrough to LiftableRockStartSmashingAnimation */
    EntityCheckThrowAtTriggers(gb, bc);
    /* fallthrough */
    LiftableRockStartSmashingAnimation(gb, bc);
}

/* LiftableRockStartSmashingAnimation (03:53E4) */
void LiftableRockStartSmashingAnimation(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, hNoiseSfx; ld [hl], NOISE_SFX_CUT_GRASS */
    gb_write_hram(gb, hNoiseSfx, NOISE_SFX_CUT_GRASS);

    /* ld e, $1F; ldh a, [hActiveEntitySpriteVariant]; cp $FF; jr z, .grassSound */
    /* cp $01; jr z, .grassSound */
    /* ld [hl], NOISE_SFX_POT_SMASHED; ld e, $0F */
    uint8_t e = 0x1F;
    uint8_t variant = gb_read_hram(gb, hActiveEntitySpriteVariant);
    if (variant != 0xFF && variant != 0x01) {
        gb_write_hram(gb, hNoiseSfx, NOISE_SFX_POT_SMASHED);
        e = 0x0F;
    }

    /* ld hl, wEntitiesPrivateCountdown1Table; add hl, bc; ld [hl], e */
    gb_write(gb, wEntitiesPrivateCountdown1Table + bc, e);

    /* ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; inc [hl]; inc [hl] */
    uint8_t physics = gb_read(gb, wEntitiesPhysicsFlagsTable + bc);
    physics += 2;
    gb_write(gb, wEntitiesPhysicsFlagsTable + bc, physics);

    return;
}

/* Arrow Entity Handlers (03:6A34-03:6B71) */

/* Arrow sprite variants - from 03:6AC6 (EntityArrowSpriteVariants) */
/* 4 variants * 4 bytes each (2 sprites * 2 bytes) = 16 bytes */
static const uint8_t EntityArrowSpriteVariants[16] = {
    /* variant 0 (right): tile $2E/$2C, attrs with XFLIP */
    0x2E, OAM_GBC_PAL_1 | OAMF_PAL0 | OAMF_XFLIP,
    0x2C, OAM_GBC_PAL_1 | OAMF_PAL0 | OAMF_XFLIP,
    /* variant 1 (left): tile $2C/$2E, attrs without XFLIP */
    0x2C, OAM_GBC_PAL_1 | OAMF_PAL0,
    0x2E, OAM_GBC_PAL_1 | OAMF_PAL0,
    /* variant 2 (up): tile $2A, attrs with YFLIP */
    0x2A, OAM_GBC_PAL_1 | OAMF_PAL0 | OAMF_YFLIP,
    0x2A, OAM_GBC_PAL_1 | OAMF_PAL0 | OAMF_YFLIP | OAMF_XFLIP,
    /* variant 3 (down): tile $2A, attrs without YFLIP */
    0x2A, OAM_GBC_PAL_1 | OAMF_PAL0,
    0x2A, OAM_GBC_PAL_1 | OAMF_PAL0 | OAMF_XFLIP
};

/* Bomb Arrow bomb sprite - from 03:6A66 */
static const uint8_t BombArrowBombSprite[2] = {
    0x80, 0xE1  /* tile $80, palette 5 | OAMF_PAL1 */
};

/* Bomb Arrow offsets - from 03:6A68/03:6A6C */
static const int8_t BombArrowBombXOffsetPerDirection[4] = { +4, -4, 0, 0 };
static const int8_t BombArrowBombYOffsetPerDirection[4] = { -2, -2, -6, +4 };

/* Arrow spinning sprite variant frames - from 03:6B48 */
static const uint8_t ArrowSpinningSpriteVariantFrames[4] = {
    DIRECTION_RIGHT, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_UP
};

/* Octorok Rock sprite variants - from 03:6A1E (OctorokRockSpriteVariants) */
/* 2 variants * 4 bytes each (2 sprites * 2 bytes) = 8 bytes */
static const uint8_t OctorokRockSpriteVariants[8] = {
    /* variant 0: tile $6C, attrs */
    0x6C, 0x01,
    0x6C, OAMF_XFLIP,
    /* variant 1: tile $5C, attrs */
    0x5C, 0x01,
    0x5C, OAMF_XFLIP
};

/* ArrowEntityHandler (03:6A34) */
void ArrowEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* Increment the active projectiles count */
    /* ld hl, wActiveProjectileCount; inc [hl] */
    gb_write(gb, wActiveProjectileCount, gb_read(gb, wActiveProjectileCount) + 1);

    /* If hActiveEntityState == 1... */
    /* ldh a, [hActiveEntityState]; and a; jr nz, BombArrowHandler */
    if (gb_read_hram(gb, hActiveEntityState) != 0) {
        BombArrowHandler(gb, bc);
        return;
    }

    /* If GetEntityTransitionCountdown != 0... */
    /* call GetEntityTransitionCountdown; jp nz, ArrowRenderAndMove */
    if (GetEntityTransitionCountdown(gb, bc) != 0) {
        ArrowRenderAndMove(gb, bc);
        return;
    }

    /* hActiveEntityState == 0 and GetEntityTransitionCountdown == 0 */
    /* ld a, DAMAGE_TYPE_ARROW; ld [wAttackDamageType], a; call func_003_75A2 */
    gb_write(gb, wAttackDamageType, DAMAGE_TYPE_ARROW);
    func_003_75A2(gb, bc);

    /* call ArrowRenderAndMove */
    ArrowRenderAndMove(gb, bc);

    /* Allow shooting the Dungeon 8 statue in the eye */
    /* ldh a, [hActiveEntitySpriteVariant]; cp DIRECTION_UP; ret nz */
    if (gb_read_hram(gb, hActiveEntitySpriteVariant) != DIRECTION_UP) {
        return;
    }
    /* and the event trigger is TRIGGER_SHOOT_STATUE_EYE... */
    /* ld a, [wRoomEvent]; and EVENT_TRIGGER_MASK; cp TRIGGER_SHOOT_STATUE_EYE; ret nz */
    if ((gb_read(gb, wRoomEvent) & EVENT_TRIGGER_MASK) != TRIGGER_SHOOT_STATUE_EYE) {
        return;
    }
    /* and hObjectUnderEntity == OBJECT_ONE_EYED_STATUE... */
    /* ldh a, [hObjectUnderEntity]; cp OBJECT_ONE_EYED_STATUE; ret nz */
    if (gb_read_hram(gb, hObjectUnderEntity) != OBJECT_ONE_EYED_STATUE) {
        return;
    }
    /* call MarkTriggerAsResolved, and clear entity */
    /* call MarkTriggerAsResolved; jp UnloadEntityAndReturn */
    MarkTriggerAsResolved(gb);
    UnloadEntityAndReturn(gb, bc);
}

/* BombArrowHandler (03:6A70) */
void BombArrowHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call GetEntityTransitionCountdown; jr z, .beforeExploding */
    uint8_t countdown = GetEntityTransitionCountdown(gb, bc);
    if (countdown == 0) {
        goto beforeExploding;
    }

    /* ld a, ENTITY_BOMB; call SpawnNewEntity; jr c, .unloadAndReturn */
    uint16_t de = SpawnNewEntity_trampoline(gb, ENTITY_BOMB, NULL);
    if (de == 0xFFFF) {
        goto unloadAndReturn;
    }

    /* ldh a, [hMultiPurpose0]; ld hl, wEntitiesPosXTable; add hl, de; ld [hl], a */
    gb_write(gb, wEntitiesPosXTable + de, gb_read_hram(gb, hMultiPurpose0));
    /* ldh a, [hMultiPurpose1]; ld hl, wEntitiesPosYTable; add hl, de; ld [hl], a */
    gb_write(gb, wEntitiesPosYTable + de, gb_read_hram(gb, hMultiPurpose1));
    /* ld hl, wEntitiesTransitionCountdownTable; add hl, de; ld [hl], $17 */
    gb_write(gb, wEntitiesTransitionCountdownTable + de, 0x17);
    /* call PlayBombExplosionSfx */
    PlayBombExplosionSfx(gb);

unloadAndReturn:
    /* jp UnloadEntityAndReturn */
    UnloadEntityAndReturn(gb, bc);
    return;

beforeExploding: ;
    /* Render the bomb arrow's bomb */
    /* ldh a, [hActiveEntitySpriteVariant]; push af; ld e, a; ld d, b; xor a; ldh [hActiveEntitySpriteVariant], a */
    uint8_t sprite_variant = gb_read_hram(gb, hActiveEntitySpriteVariant);
    gb_write_hram(gb, hActiveEntitySpriteVariant, 0x00);

    /* ld hl, BombArrowBombXOffsetPerDirection; add hl, de; ldh a, [hActiveEntityPosX]; add [hl]; ldh [hActiveEntityPosX], a */
    int8_t x_offset = BombArrowBombXOffsetPerDirection[sprite_variant & 0x03];
    uint8_t pos_x = gb_read_hram(gb, hActiveEntityPosX);
    gb_write_hram(gb, hActiveEntityPosX, (uint8_t)(pos_x + x_offset));

    /* ld hl, BombArrowBombYOffsetPerDirection; add hl, de; ldh a, [hActiveEntityVisualPosY]; add [hl]; ldh [hActiveEntityVisualPosY], a */
    int8_t y_offset = BombArrowBombYOffsetPerDirection[sprite_variant & 0x03];
    uint8_t visual_pos_y = gb_read_hram(gb, hActiveEntityVisualPosY);
    gb_write_hram(gb, hActiveEntityVisualPosY, (uint8_t)(visual_pos_y + y_offset));

    /* ld de, BombArrowBombSprite; call RenderActiveEntitySprite */
    RenderActiveEntitySprite(gb, BombArrowBombSprite, NULL);

    /* call CopyEntityPositionToActivePosition */
    CopyEntityPositionToActivePosition(gb, bc);

    /* pop af; ldh [hActiveEntitySpriteVariant], a */
    gb_write_hram(gb, hActiveEntitySpriteVariant, sprite_variant);

    /* Render the arrow itself */
    /* ld de, EntityArrowSpriteVariants; call RenderActiveEntitySpritesPair */
    RenderActiveEntitySpritesPair(gb, EntityArrowSpriteVariants, NULL);

    /* Deal (no) damage to other entities before exploding */
    /* ld a, DAMAGE_TYPE_BOMB_ARROW; ld [wAttackDamageType], a; call func_003_75A2 */
    gb_write(gb, wAttackDamageType, DAMAGE_TYPE_BOMB_ARROW);
    func_003_75A2(gb, bc);
    /* jr ArrowRenderAndMove.skipRendering */
    /* fallthrough to ArrowRenderAndMove skipRendering */
    ArrowRenderAndMove_skipRendering(gb, bc);
}

/* MoblinArrowEntityHandler (03:6ACC) */
void MoblinArrowEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call GetEntityTransitionCountdown; jr nz, ArrowRenderAndMove */
    if (GetEntityTransitionCountdown(gb, bc) != 0) {
        ArrowRenderAndMove(gb, bc);
        return;
    }

    /* call CheckLinkCollisionWithProjectile; fallthrough to ArrowRenderAndMove */
    CheckLinkCollisionWithProjectile(gb, bc);
    ArrowRenderAndMove(gb, bc);
}

/* ArrowRenderAndMove (03:6AD4) */
void ArrowRenderAndMove(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld de, EntityArrowSpriteVariants; call RenderActiveEntitySpritesPair */
    /* Select sprite variants based on entity type (matches OctorokRockEntityHandler behavior) */
    const uint8_t *sprite_variants = EntityArrowSpriteVariants;
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_OCTOROK_ROCK) {
        sprite_variants = OctorokRockSpriteVariants;
    }
    RenderActiveEntitySpritesPair(gb, sprite_variants, NULL);

    /* call ReturnIfNonInteractive_03; call GetEntityTransitionCountdown; jr nz, ArrowRockAfterHittingWall */
    if (ReturnIfNonInteractive_03(gb, false)) {
        return;
    }

    if (GetEntityTransitionCountdown(gb, bc) != 0) {
        ArrowRockAfterHittingWall(gb, bc);
        return;
    }

    /* call UpdateEntityPosWithSpeed_03; call ApplySwordIntersectionWithObjects */
    UpdateEntityPosWithSpeed_03(gb, bc);
    ApplySwordIntersectionWithObjects(gb, bc);

    /* ld hl, wEntitiesCollisionsTable; add hl, bc; ld a, [hl]; and a; jr z, EntityBounceOffWallX.return */
    if (gb_read(gb, wEntitiesCollisionsTable + bc) == 0) {
        return;
    }

    /* call GetEntityTransitionCountdown (result unused, reloaded later) */
    (void)GetEntityTransitionCountdown(gb, bc);

    /* ldh a, [hActiveEntityType]; cp ENTITY_MAGIC_ROD_FIREBALL; jr nz, .fireballEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_MAGIC_ROD_FIREBALL) {
        /* call GetEntityPrivateCountdown1; ld [hl], $30; ret */
        GetEntityPrivateCountdown1(gb, bc);
        gb_write(gb, wEntitiesPrivateCountdown1Table + bc, 0x30);
        return;
    }

    /* ld [hl], $18; ld hl, wEntitiesSpeedZTable; add hl, bc; ld [hl], $10 */
    gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x18);
    gb_write(gb, wEntitiesSpeedZTable + bc, 0x10);

    /* ld hl, wEntitiesCollisionsTable; add hl, bc; ld a, [hl]; inc a; jr z, .skipSound */
    uint8_t collisions = gb_read(gb, wEntitiesCollisionsTable + bc);
    if ((uint8_t)(collisions + 1) == 0) {
        goto skipSound;
    }

    /* ld a, JINGLE_SWORD_POKING; ldh [hJingle], a */
    gb_write_hram(gb, hJingle, JINGLE_SWORD_POKING);

skipSound:
    /* call AlertSwordMoblins */
    AlertSwordMoblins(gb);

    /* ldh a, [hActiveEntityType]; cp ENTITY_ARROW; jr nz, .enemyProjectileBounce */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_ARROW) {
        /* Player arrows bounce more off walls */
        /* call .playerArrowBounceY; ld hl, wEntitiesSpeedXTable; .playerArrowBounce: add hl, bc; ld a, [hl]; cpl; inc a; sra a; sra a; sra a; ld [hl], a; ret */
        EntityBounceOffWallY(gb, bc);
        EntityBounceOffWallX(gb, bc);
        return;
    }

    /* call EntityBounceOffWallY; fallthrough to EntityBounceOffWallX */
    EntityBounceOffWallY(gb, bc);
    EntityBounceOffWallX(gb, bc);
}

/* ArrowRenderAndMove skipRendering entry point */
void ArrowRenderAndMove_skipRendering(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Skip rendering, just do the collision/movement logic */
    ArrowRenderAndMove(gb, bc);
}

/* EntityBounceOffWallX (03:6B34) */
void EntityBounceOffWallX(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesSpeedXTable; add hl, bc; ld a, [hl]; cpl; inc a; sra a; sra a; sra a; ld [hl], a; ret */
    int8_t speed_x = (int8_t)gb_read(gb, wEntitiesSpeedXTable + bc);
    speed_x = (int8_t)(~speed_x + 1);  /* cpl + 1 = negate */
    speed_x >>= 3;  /* sra x3 */
    gb_write(gb, wEntitiesSpeedXTable + bc, (uint8_t)speed_x);
}

/* EntityBounceOffWallY (03:6B43) */
void EntityBounceOffWallY(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesSpeedYTable; add hl, bc; ld a, [hl]; cpl; inc a; sra a; sra a; sra a; ld [hl], a; ret */
    int8_t speed_y = (int8_t)gb_read(gb, wEntitiesSpeedYTable + bc);
    speed_y = (int8_t)(~speed_y + 1);  /* cpl + 1 = negate */
    speed_y >>= 3;  /* sra x3 */
    gb_write(gb, wEntitiesSpeedYTable + bc, (uint8_t)speed_y);
}

/* ArrowRockAfterHittingWall (03:6B4C) */
void ArrowRockAfterHittingWall(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* cp $01; jp z, UnloadEntityAndReturn / jr nz, .unloadEnd */
    uint8_t countdown = GetEntityTransitionCountdown(gb, bc);
    if (countdown == 0x01) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* Octorok rocks don't spin after hitting a wall, only arrows do */
    /* ldh a, [hActiveEntityType]; cp ENTITY_OCTOROK_ROCK; jr z, .spinningEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_OCTOROK_ROCK) {
        goto spinningEnd;
    }

    /* call GetEntityTransitionCountdown; srl a x3; and $03; ld e, a; ld d, b; ld hl, ArrowSpinningSpriteVariantFrames; add hl, de; ld a, [hl]; call SetEntitySpriteVariant */
    uint8_t frame = GetEntityTransitionCountdown(gb, bc);
    frame >>= 3;  /* srl x3 */
    frame &= 0x03;
    uint8_t variant = ArrowSpinningSpriteVariantFrames[frame];
    SetEntitySpriteVariant(gb, bc, variant);

spinningEnd:
    /* call UpdateEntityPosWithSpeed_03; jr func_003_6B7B */
    UpdateEntityPosWithSpeed_03(gb, bc);
    func_003_6B7B(gb, bc);
}

/* OctorokEntityHandler (03:57E9) */
void OctorokEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld de, OctorokSpriteVariants */
    /* ld a, [wGameplayType]; cp GAMEPLAY_CREDITS; jr z, .creditsEnd */
    /* ld a, $30; ldh [hActiveEntityTilesOffset], a */
    if (gb_read(gb, wGameplayType) != GAMEPLAY_CREDITS) {
        gb_write_hram(gb, hActiveEntityTilesOffset, 0x30);
    }

    /* call AnimateRoamingEnemy; ret */
    AnimateRoamingEnemy(gb, bc);
}

/* func_003_6B7B (03:6B7B) */
void func_003_6B7B(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ldh a, [hIsSideScrolling]; and a; jr nz, .sideScrolling */
    if (gb_read_hram(gb, hIsSideScrolling) != 0) {
        goto sideScrolling;
    }

    /* call AddEntityZSpeedToPos_03 */
    AddEntitySpeedToPos_03(gb, bc);

    /* ld hl, wEntitiesSpeedZTable; add hl, bc; ld a, [hl]; sub $02; ld [hl], a */
    uint8_t speed_z = gb_read(gb, wEntitiesSpeedZTable + bc);
    speed_z = (uint8_t)(speed_z - 0x02);
    gb_write(gb, wEntitiesSpeedZTable + bc, speed_z);
    return;

sideScrolling: ;
    /* ld hl, wEntitiesGroundStatusTable; add hl, bc; ld a, [hl]; ld e, a; ld d, b; and a; jr z, .updateXSpeedEnd */
    uint8_t ground_status = gb_read(gb, wEntitiesGroundStatusTable + bc);
    if (ground_status == 0) {
        goto updateXSpeedEnd;
    }

    /* ldh a, [hFrameCounter]; and $07; jr nz, .updateXSpeedEnd */
    if ((gb_read_hram(gb, hFrameCounter) & 0x07) != 0) {
        goto updateXSpeedEnd;
    }

    /* ld hl, wEntitiesSpeedXTable; add hl, bc; ld a, [hl]; and a; jr z, .updateXSpeedEnd */
    uint8_t speed_x = gb_read(gb, wEntitiesSpeedXTable + bc);
    if (speed_x == 0) {
        goto updateXSpeedEnd;
    }

    /* and $80; jr z, .positiveDifferenceX */
    if ((speed_x & 0x80) == 0) {
        goto positiveDifferenceX;
    }

    /* get here every 8 frames, if underwater and X speed is not 0 */
    /* inc [hl]; inc [hl] */
    speed_x += 2;
    gb_write(gb, wEntitiesSpeedXTable + bc, speed_x);
    goto updateXSpeedEnd;

positiveDifferenceX:
    /* dec [hl] */
    speed_x -= 1;
    gb_write(gb, wEntitiesSpeedXTable + bc, speed_x);

updateXSpeedEnd: ;
    /* ld hl, Data_003_6B73; add hl, de; ld a, [hl] */
    /* ld hl, wEntitiesSpeedYTable; add hl, bc; add [hl]; ld [hl], a */
    static const uint8_t Data_003_6B73[4] = { 0x02, 0x01, 0x02, 0x02 };
    static const uint8_t Data_003_6B77[4] = { 0x40, 0x08, 0x40, 0x40 };
    
    uint8_t entity_type = gb_read_hram(gb, hActiveEntityType);
    if (entity_type < 4) {
        uint8_t y_add = Data_003_6B73[entity_type];
        uint8_t speed_y = gb_read(gb, wEntitiesSpeedYTable + bc);
        speed_y += y_add;
        gb_write(gb, wEntitiesSpeedYTable + bc, speed_y);

        uint8_t y_sub = Data_003_6B77[entity_type];
        if (speed_y >= y_sub) {
            speed_y = y_sub;
            gb_write(gb, wEntitiesSpeedYTable + bc, speed_y);
        }
    }
    return;
}

/* CheckLinkCollisionWithEnemy (03:6C72) */
bool CheckLinkCollisionWithEnemy(GBState *gb, uint16_t bc) {
    if (!gb) return false;

    /* If Link is in the air, skip the collision check */
    /* ldh a, [hLinkPositionZ]; and a; jr nz, CheckLinkCollisionWithProjectile.return */
    if (gb_read_hram(gb, hLinkPositionZ) != 0) {
        return false;
    }

    /* If Link is not interactive, return. */
    /* ld a, [wLinkMotionState]; cp LINK_MOTION_TYPE_NON_INTERACTIVE; jr nc, CheckLinkCollisionWithProjectile.return */
    if (gb_read(gb, wLinkMotionState) >= LINK_MOTION_UNSTUCKING) {
        return false;
    }

    /* push bc; c = (entity index * 4); sla c; sla c; ld hl, wEntitiesHitboxPositionTable; add hl, bc; pop bc */
    uint8_t c = (bc & 0xFF) << 2;
    uint16_t hl = wEntitiesHitboxPositionTable + c;

    /* hActiveEntityPosX + wEntitiesHitboxPositionTable[c + 0] */
    /* ldh a, [hActiveEntityPosX]; add [hl]; push hl; ld hl, hLinkPositionX; sub [hl]; sub $08; cp $80; jr c, .jr_6C98; cpl; inc a */
    int16_t diff_x = (int16_t)gb_read_hram(gb, hActiveEntityPosX) + gb_read(gb, hl + 0);
    diff_x -= gb_read_hram(gb, hLinkPositionX);
    diff_x -= 8;
    if (diff_x < 0) diff_x = -diff_x;
    if (diff_x >= 0x80) {
        return false;
    }

    /* .jr_6C98: pop hl; push af; inc hl; ld a, $04; add [hl]; ld e, a; pop af; cp e; jp nc, jr_003_6CCB */
    int16_t diff_y = (int16_t)gb_read_hram(gb, hActiveEntityVisualPosY) + gb_read(gb, hl + 1) + 4;
    diff_y -= gb_read_hram(gb, hLinkPositionY);
    diff_y -= 8;
    if (diff_y < 0) diff_y = -diff_y;
    if (diff_y >= 0x80) {
        return false;
    }

    /* inc hl; ldh a, [hActiveEntityVisualPosY]; add [hl]; push hl; ld hl, hLinkPositionY; sub [hl]; sub $08; cp $80; jr c, .jr_6CB5; cpl; inc a */
    diff_y = (int16_t)gb_read_hram(gb, hActiveEntityVisualPosY) + gb_read(gb, hl + 2) + 4;
    diff_y -= gb_read_hram(gb, hLinkPositionY);
    diff_y -= 8;
    if (diff_y < 0) diff_y = -diff_y;
    if (diff_y >= 0x80) {
        return false;
    }

    /* .jr_6CB5: pop hl; push af; inc hl; ld a, $04; add [hl]; ld e, a; pop af; cp e; jr nc, jr_003_6CCB */
    diff_y = (int16_t)gb_read_hram(gb, hActiveEntityVisualPosY) + gb_read(gb, hl + 3) + 4;
    diff_y -= gb_read_hram(gb, hLinkPositionY);
    diff_y -= 8;
    if (diff_y < 0) diff_y = -diff_y;
    if (diff_y >= 0x80) {
        return false;
    }

    /* func_003_6CC0: ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; ld a, [hl]; and ENTITY_PHYSICS_HARMLESS; jr z, jr_003_6CCD */
    uint8_t physics = gb_read(gb, wEntitiesPhysicsFlagsTable + bc);
    if ((physics & ENTITY_PHYSICS_HARMLESS) != 0) {
        /* Harmless entity - no collision */
        return false;
    }

    /* jr_003_6CCD: ldh a, [hLinkAnimationState]; sub $4E; cp $02; jr c, jr_003_6CC9 */
    if (gb_read_hram(gb, hLinkAnimationState) < 0x4E || gb_read_hram(gb, hLinkAnimationState) >= 0x50) {
        return false;
    }

    return true;
}

/* ApplyLinkCollisionWithEnemy (03:6CD5) */
void ApplyLinkCollisionWithEnemy(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* Special case when a cheep-cheep hurts Link */
    /* ldh a, [hActiveEntityType]; cp ENTITY_CHEEP_CHEEP_JUMPING; jr nz, .cheepCheepEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_CHEEP_CHEEP_JUMPING) {
        /* call GetEntityYDistanceToLink_03; ld a, e; cp $02; jr nz, .goombaEnd */
        /* Simplified: just check if close vertically */
        /* call IncrementEntityState; ld [hl], ENTITY_STATUS_ACTIVE; ld a, $02; ld [wIsLinkInTheAir], a; ld a, $F0; ldh [hLinkSpeedY], a; call ClearEntitySpeed; ld a, WAVE_SFX_FLOOR_SWITCH; ldh [hWaveSfx], a; ret */
        IncrementEntityState(gb, bc);
        gb_write(gb, wEntitiesStatusTable + bc, ENTITY_STATUS_ACTIVE);
        gb_write(gb, wIsLinkInTheAir, 0x02);
        gb_write_hram(gb, hLinkSpeedY, 0xF0);
        ClearEntitySpeed(gb, bc);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_FLOOR_SWITCH);
        return;
    }

    /* Special case when a Goomba hurts Link */
    /* ldh a, [hActiveEntityType]; cp ENTITY_GOOMBA; jr nz, .goombaEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GOOMBA) {
        /* ld a, [wIsLinkInTheAir]; and a; jr z, .goombaEnd */
        if (gb_read(gb, wIsLinkInTheAir) == 0) {
            return;
        }
        /* ldh a, [hLinkCountdown]; and a; jr nz, .jr_003_6D1B */
        if (gb_read_hram(gb, hLinkCountdown) != 0) {
            goto jr_003_6D1B;
        }
        /* ldh a, [hIsSideScrolling]; and a; jr nz, .jr_003_6D15 */
        if (gb_read_hram(gb, hIsSideScrolling) != 0) {
            goto jr_003_6D15;
        }
        /* ldh a, [hLinkVelocityZ]; xor $80; jr .jr_003_6D17 */
        /* .jr_003_6D15: ldh a, [hLinkSpeedY] */
    jr_003_6D15:
        /* .jr_003_6D17: and $80; jr nz, .goombaEnd */
        if ((gb_read_hram(gb, hLinkSpeedY) & 0x80) != 0) {
            return;
        }

    jr_003_6D1B:
        /* ld a, $02; ldh [hLinkCountdown], a */
        gb_write_hram(gb, hLinkCountdown, 0x02);
        /* ld hl, wEntitiesStateTable; add hl, bc; ld [hl], $02 */
        gb_write(gb, wEntitiesStateTable + bc, 0x02);
        /* call GetEntityTransitionCountdown; ld [hl], $30 */
        gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x30);
        /* ld a, WAVE_SFX_FLOOR_SWITCH; ldh [hWaveSfx], a */
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_FLOOR_SWITCH);
        /* ldh a, [hIsSideScrolling]; and a; jr nz, .jr_003_6D38 */
        if (gb_read_hram(gb, hIsSideScrolling) != 0) {
            goto jr_003_6D38;
        }
        /* ld a, $10; ldh [hLinkVelocityZ], a; ret */
        gb_write_hram(gb, hLinkVelocityZ, 0x10);
        return;

    jr_003_6D38:
        /* ld a, $F0; ldh [hLinkSpeedY], a; ret */
        gb_write_hram(gb, hLinkSpeedY, 0xF0);
        return;
    }

    /* Special case when Link collides with a Gel */
    /* ldh a, [hActiveEntityType]; cp ENTITY_GEL; jr nz, .gelEnd */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GEL) {
        /* call GetEntityTransitionCountdown; ld [hl], $80; call IncrementEntityState; ld [hl], $04; ret */
        gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x80);
        IncrementEntityState(gb, bc);
        gb_write(gb, wEntitiesStateTable + bc, 0x04);
        return;
    }

    /* cp ENTITY_CUE_BALL; jr z, .jr_6D5D; cp ENTITY_ROLLING_BONES_BAR; jr z, .jr_6D5D */
    /* ld a, [wIgnoreLinkCollisionsCountdown]; and a; jp nz, setCarryAndReturn */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_CUE_BALL ||
        gb_read_hram(gb, hActiveEntityType) == ENTITY_ROLLING_BONES_BAR) {
        goto jr_6D5D;
    }

    if (gb_read(gb, wIgnoreLinkCollisionsCountdown) != 0) {
        return;
    }

jr_6D5D:
    /* ldh a, [hActiveEntityType]; cp ENTITY_MOBLIN_KING; jr nz, .jr_6D73 */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_MOBLIN_KING) {
        /* ldh a, [hActiveEntityState]; cp $04; jr nz, .jr_6D73 */
        if (gb_read_hram(gb, hActiveEntityState) == 0x04) {
            /* call IncrementEntityState; ld [hl], $08; ld a, WAVE_SFX_LINK_HURT; ldh [hWaveSfx], a; ret */
            IncrementEntityState(gb, bc);
            gb_write(gb, wEntitiesStateTable + bc, 0x08);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_LINK_HURT);
            return;
        }
    }

    /* ld a, [wInvincibilityCounter]; and a; jp nz, .invincibleEnd */
    if (gb_read(gb, wInvincibilityCounter) != 0) {
        return;
    }

    /* Special case for entity type CUE_BALL, ROLLING_BONES_BAR */
    /* Handle default collision - hurt Link */
    /* This is a simplified version - the actual implementation is complex */
    /* We'll just apply damage and effects */
    gb_write_hram(gb, hWaveSfx, WAVE_SFX_LINK_HURT);
    gb_write(gb, wInvincibilityCounter, 0x40);
    /* Subtract health would be done here */
    return;
}

/* DefaultEnemyDamageCollisionHandler (03:6E2B) */
void DefaultEnemyDamageCollisionHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call func_003_6C6B */
    func_003_6C6B(gb, bc);

    /* ld a, [wC140]; cp $00; jp z, label_003_73E6 */
    if (gb_read(gb, wC140) == 0) {
        return;
    }

    /* ld hl, wEntitiesFlashCountdownTable; add hl, bc; ld a, [hl]; and a; jr z, .jr_6E40 */
    if (gb_read(gb, wEntitiesFlashCountdownTable + bc) == 0) {
        goto jr_6E40;
    }

    /* cp $18; jp c, label_003_73E6 */
    if (gb_read(gb, wEntitiesFlashCountdownTable + bc) < 0x18) {
        return;
    }

jr_6E40:
    /* ld a, [wC1AC]; and a; jr z, .jr_6E4B */
    if (gb_read(gb, wC1AC) == 0) {
        goto jr_6E4B;
    }

    /* dec a; cp c; jp z, label_003_73E6 */
    if ((gb_read(gb, wC1AC) - 1) == (bc & 0xFF)) {
        return;
    }

jr_6E4B:
    /* ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld a, [hl]; and a; jp nz, label_003_73E6 */
    if (gb_read(gb, wEntitiesIgnoreHitsCountdownTable + bc) != 0) {
        return;
    }

    /* ld de, hActiveEntityPosX; push bc; sla c; sla c; ld hl, wEntitiesHitboxPositionTable; add hl, bc; pop bc; ld a, [de]; add [hl]; push hl; ld hl, wC140; sub [hl]; cp $80; jr c, .jr_6E6E; cpl; inc a; .jr_6E6E: pop hl; push af; inc hl; ld a, [wC141]; add [hl]; ld e, a; pop af; cp e; jp nc, jr_003_6CCB */
    /* This is complex collision detection - simplified */
    /* We'll skip the detailed hitbox collision for now */

    /* func_003_6CC0: ld hl, wEntitiesPhysicsFlagsTable; add hl, bc; ld a, [hl]; and ENTITY_PHYSICS_HARMLESS; jr z, jr_003_6CCD */
    if ((gb_read(gb, wEntitiesPhysicsFlagsTable + bc) & ENTITY_PHYSICS_HARMLESS) == 0) {
        /* jr_003_6CCD: ldh a, [hLinkAnimationState]; sub $4E; cp $02; jr c, jr_003_6CC9 */
        if (gb_read_hram(gb, hLinkAnimationState) >= 0x4E && gb_read_hram(gb, hLinkAnimationState) < 0x50) {
            /* Damage the entity */
            /* This would call ApplySwordDamagesToEnemy */
            ApplySwordDamagesToEnemy(gb, bc);
        }
    }
}

/* ApplySwordDamagesToEnemy (03:7267) */
void ApplySwordDamagesToEnemy(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* This is a complex function that handles sword damage to enemies */
    /* Simplified implementation - apply damage based on weapon type */
    /* The actual implementation has many special cases for different enemy types */
    
    /* Special case for Final Nightmare - handled by JP_TABLE */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_FINAL_NIGHTMARE) {
        /* Handle Final Nightmare forms - simplified */
        return;
    }

    /* Special case for Buzz Blob */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_BUZZ_BLOB) {
        /* if status == ACTIVE; call IncrementEntityState; ld [hl], $01; call GetEntityTransitionCountdown; ld [hl], $40; ld a, $40; ld [wD464], a; xor a; ld [wSwordAnimationState], a; ld [wC16A], a; ld [wIsUsingSpinAttack], a; ld a, NOISE_SFX_BUZZ_BLOB_ELECTROCUTE; ldh [hNoiseSfx], a; jp ApplyLinkCollisionWithEnemy */
        if (gb_read_hram(gb, hActiveEntityStatus) == ENTITY_STATUS_ACTIVE) {
            IncrementEntityState(gb, bc);
            gb_write(gb, wEntitiesStateTable + bc, 0x01);
            gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x40);
            gb_write(gb, wD464, 0x40);
            gb_write(gb, wSwordAnimationState, 0x00);
            gb_write(gb, wC16A, 0x00);
            gb_write(gb, wIsUsingSpinAttack, 0x00);
            gb_write_hram(gb, hNoiseSfx, NOISE_SFX_BUZZ_BLOB_ELECTROCUTE);
            ApplyLinkCollisionWithEnemy(gb, bc);
        }
        return;
    }

    /* Standard sword collision */
    /* Special case for Bouncing Bombite */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_BOUNCING_BOMBITE) {
        /* call GetVectorTowardsLink; negate and set speeds; call IncrementEntityState; ld [hl], $02; call GetEntityTransitionCountdown; ld [hl], $40; call GetEntityPrivateCountdown1; ld [hl], $08; ret */
        uint8_t x, y;
        GetVectorTowardsLink(gb, &x, &y);
        gb_write(gb, wEntitiesSpeedYTable + bc, (uint8_t)(~x + 1));
        gb_write(gb, wEntitiesSpeedXTable + bc, (uint8_t)(~y + 1));
        IncrementEntityState(gb, bc);
        gb_write(gb, wEntitiesStateTable + bc, 0x02);
        gb_write(gb, wEntitiesTransitionCountdownTable + bc, 0x40);
        gb_write(gb, wEntitiesPrivateCountdown1Table + bc, 0x08);
        return;
    }

    /* Special case for Angler Fish */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_ANGLER_FISH) {
        /* call func_003_6DDF; ld a, $08; ld [wIgnoreLinkCollisionsCountdown], a; jr .slimeEyeEnd */
        func_003_6DDF(gb, bc);
        gb_write(gb, wIgnoreLinkCollisionsCountdown, 0x08);
        return;
    }

    /* Special case for Slime Eye */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_SLIME_EYE) {
        /* Simplified - has complex state machine */
        return;
    }

    /* If sword clink is disabled... */
    if ((gb_read(gb, wEntitiesOptions1Table + bc) & ENTITY_OPT1_SWORD_CLINK_OFF) == 0) {
        /* Special case for Knight */
        if (gb_read_hram(gb, hActiveEntityType) == ENTITY_KNIGHT) {
            /* call label_003_6F04 */
            /* This is complex - skip for now */
            return;
        }

        /* Special case for Genie */
        if (gb_read_hram(gb, hActiveEntityType) == ENTITY_GENIE) {
            /* ConfigureEntityRecoil with different strength based on tunic/power */
            uint8_t recoil = 0x20;
            if (gb_read(gb, wTunicType) == TUNIC_RED || gb_read(gb, wActivePowerUp) == ACTIVE_POWER_UP_PIECE_OF_POWER) {
                recoil = 0x30;
            }
            ConfigureEntityRecoil(gb, bc, recoil);

            /* Without flashing from damages */
            gb_write(gb, wEntitiesFlashCountdownTable + bc, 0);
            return;
        }
    }

    /* Continue default collision */
    /* ld a, c; inc a; ld [wC1AC], a; call label_D07; ld hl, wEntitiesIgnoreHitsCountdownTable; add hl, bc; ld [hl], $10; ld hl, wEntitiesRecoilVelocityX; add hl, bc; ld [hl], b; ld hl, wEntitiesRecoilVelocityY; add hl, bc; ld [hl], b; jp func_003_6DDF */
    gb_write(gb, wC1AC, (uint8_t)((bc & 0xFF) + 1));
    gb_write(gb, wEntitiesIgnoreHitsCountdownTable + bc, 0x10);
    gb_write(gb, wEntitiesRecoilVelocityX + bc, 0);
    gb_write(gb, wEntitiesRecoilVelocityY + bc, 0);
    func_003_6DDF(gb, bc);
}

/* Droppable Magic Powder Entity Handler (03:6057) */
void DroppableMagicPowderEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld a, [wIsIndoor]; and a; jr z, .jr_6063 */
    if (gb_read(gb, wIsIndoor) == 0) {
        goto jr_6063;
    }

    /* ldh a, [hMapId]; cp MAP_COLOR_DUNGEON; jr z, jr_003_606A */
    if (gb_read_hram(gb, hMapId) == MAP_COLOR_DUNGEON) {
        goto jr_003_606A;
    }

jr_6063:
    /* ld a, [wHasToadstool]; and a; jp nz, UnloadEntityAndReturn */
    if (gb_read(gb, wHasToadstool) != 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

jr_003_606A:
    /* call DroppableRevealOrReturnIfNeeded; call DroppableDisappearIfNeeded */
    DroppableRevealOrReturnIfNeeded(gb, bc);
    DroppableDisappearIfNeeded(gb, bc);

    /* ld de, DroppableMagicPowderSprite; call RenderActiveEntitySprite; jp PickableHandler */
    static const uint8_t DroppableMagicPowderSprite[2] = { 0x55, 0x01 };
    RenderActiveEntitySprite(gb, DroppableMagicPowderSprite, NULL);
    PickableHandler(gb, bc);
}

/* Droppable Arrows Entity Handler (03:607D) */
void DroppableArrowsEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call DroppableRevealOrReturnIfNeeded; call DroppableDisappearIfNeeded */
    DroppableRevealOrReturnIfNeeded(gb, bc);
    DroppableDisappearIfNeeded(gb, bc);

    /* ld de, DroppableArrowSprite; call RenderActiveEntitySpritesPair; jp PickableHandler */
    static const uint8_t DroppableArrowSprite[4] = {
        0x2A, 0x41,  /* tile $2A, palette 1 | OAMF_PAL0 | OAMF_YFLIP */
        0x2A, 0x41 | OAMF_XFLIP
    };
    RenderActiveEntitySpritesPair(gb, DroppableArrowSprite, NULL);
    PickableHandler(gb, bc);
}

/* Droppable Disappear If Needed (03:608C) */
void DroppableDisappearIfNeeded(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call GetEntitySlowTransitionCountdown; cp $1C; ret nc */
    uint8_t countdown = GetEntitySlowTransitionCountdown(gb, bc);
    if (countdown >= 0x1C) {
        return;
    }

    /* and a; jp z, UnloadEntityAndReturn */
    if (countdown == 0) {
        UnloadEntityAndReturn(gb, bc);
        return;
    }

    /* and $01; dec a; jp SetEntitySpriteVariant */
    uint8_t variant = (countdown & 0x01) - 1;
    SetEntitySpriteVariant(gb, bc, variant);
}

/* Droppable Rupee Entity Handler (03:609E) */
void DroppableRupeeEntityHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call DroppableRevealOrReturnIfNeeded; call DroppableDisappearIfNeeded */
    DroppableRevealOrReturnIfNeeded(gb, bc);
    DroppableDisappearIfNeeded(gb, bc);

    /* ld de, DroppableRupeeSprite; call RenderActiveEntitySprite; fallthrough to PickableHandler */
    static const uint8_t DroppableRupeeSprite[2] = { 0xA6, 0xE1 };
    RenderActiveEntitySprite(gb, DroppableRupeeSprite, NULL);
    PickableHandler(gb, bc);
}

/* Pickable Handler (03:60AA) */
void PickableHandler(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* call ReturnIfNonInteractive_03 */
    if (ReturnIfNonInteractive_03(gb, false)) {
        return;
    }

    /* call PickableHandleGrabbedByItemIfNeeded */
    PickableHandleGrabbedByItemIfNeeded(gb, bc);

    /* call PickableCollectIfNeeded */
    PickableCollectIfNeeded(gb, bc);
}

/* Droppable Reveal Or Return If Needed (03:61DE) */
void DroppableRevealOrReturnIfNeeded(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld a, [hl]; and a; jp z, .return */
    if (gb_read(gb, wEntitiesPrivateState3Table + bc) == 0) {
        return;
    }

    /* ld a, [wRoomTransitionState]; and a; jp nz, .remainInvisible */
    if (gb_read(gb, wRoomTransitionState) != 0) {
        goto remainInvisible;
    }

    /* ld a, [hl]; cp $02; jr nz, .checkPegasusBootsCollision */
    uint8_t private_state3 = gb_read(gb, wEntitiesPrivateState3Table + bc);
    if (private_state3 != 0x02) {
        goto checkPegasusBootsCollision;
    }

    /* Items buried, hidden in bushes, or indoors: */
    /* ldh a, [hActiveEntityType]; cp ENTITY_DROPPABLE_SECRET_SEASHELL; jr z, .skipNotActiveIfIndoors */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_DROPPABLE_SECRET_SEASHELL) {
        goto skipNotActiveIfIndoors;
    }

    /* If indoors and not a seashell, the item can't be dug up or dropped by bushes. */
    /* ld a, [wIsIndoor]; and a; jp nz, .remainInvisible */
    if (gb_read(gb, wIsIndoor) != 0) {
        goto remainInvisible;
    }

    /* Items knocked down with the Pegasus Boots: */
    /* ld a, [wScreenShakeCountdown]; and a; jr z, .remainInvisible */
    /* ld a, [wPegasusBootsCollisionCountdown]; and a; jr z, .remainInvisible */
    /* ldh a, [hActiveEntityPosX]; add $08; ld hl, wPegasusBootsCollisionPosX; sub [hl]; add $10; cp $20; jr nc, .remainInvisible */
    /* ldh a, [hActiveEntityPosY]; add $08; ld hl, wPegasusBootsCollisionPosY; sub [hl]; add $10; cp $20; jr nc, .remainInvisible */
checkPegasusBootsCollision:
    if (gb_read(gb, wScreenShakeCountdown) == 0) {
        goto remainInvisible;
    }
    if (gb_read(gb, wPegasusBootsCollisionCountdown) == 0) {
        goto remainInvisible;
    }
    int16_t diff_x = (int16_t)gb_read_hram(gb, hActiveEntityPosX) + 8;
    diff_x -= gb_read(gb, wPegasusBootsCollisionPosX);
    diff_x += 0x10;
    if (diff_x >= 0x20) {
        goto remainInvisible;
    }
    int16_t diff_y = (int16_t)gb_read_hram(gb, hActiveEntityPosY) + 8;
    diff_y -= gb_read(gb, wPegasusBootsCollisionPosY);
    diff_y += 0x10;
    if (diff_y >= 0x20) {
        goto remainInvisible;
    }

skipNotActiveIfIndoors:
    /* call func_003_7E0E */
    func_003_7E0E(gb, bc);

    /* ldh a, [hActiveEntityType]; cp ENTITY_DROPPABLE_HEART; jr z, .activeIfOnShortGrass */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_DROPPABLE_HEART) {
        goto activeIfOnShortGrass;
    }

    /* cp ENTITY_DROPPABLE_SECRET_SEASHELL; jr nz, .activeIfOnShortGrassEnd */
    if (gb_read_hram(gb, hActiveEntityType) != ENTITY_DROPPABLE_SECRET_SEASHELL) {
        goto activeIfOnShortGrassEnd;
    }

    /* Seashells buried under short grass (some of these don't exist) */
    /* ldh a, [hMapRoom]; cp UNKNOWN_ROOM_DA; jr z, .activeIfOnShortGrassEnd */
    /* cp UNKNOWN_ROOM_A5; jr z, .activeIfOnShortGrassEnd */
    /* cp UNKNOWN_ROOM_74; jr z, .activeIfOnShortGrassEnd */
    /* cp UNKNOWN_ROOM_3A; jr z, .activeIfOnShortGrassEnd */
    /* cp UNKNOWN_ROOM_A8; jr z, .activeIfOnShortGrassEnd */
    /* cp UNKNOWN_ROOM_B2; jr z, .activeIfOnShortGrassEnd */
    uint8_t map_room = gb_read_hram(gb, hMapRoom);
    if (map_room == UNKNOWN_ROOM_DA || map_room == UNKNOWN_ROOM_A5 ||
        map_room == UNKNOWN_ROOM_74 || map_room == UNKNOWN_ROOM_3A ||
        map_room == UNKNOWN_ROOM_A8 || map_room == UNKNOWN_ROOM_B2) {
        goto activeIfOnShortGrassEnd;
    }

activeIfOnShortGrass:
    /* ldh a, [hObjectUnderEntity]; cp OBJECT_SHORT_GRASS; jr z, .setOptionsAndReveal */
    if (gb_read_hram(gb, hObjectUnderEntity) == OBJECT_SHORT_GRASS) {
        goto setOptionsAndReveal;
    }

    /* jr .activeIfOnShovelHole */
    goto activeIfOnShovelHole;

activeIfOnShortGrassEnd:
    /* ld hl, wEntitiesPrivateState4Table; add hl, bc; ld [hl], $01 */
    gb_write(gb, wEntitiesPrivateState4Table + bc, 0x01);

activeIfOnShovelHole:
    /* ldh a, [hObjectUnderEntity]; cp OBJECT_SHOVEL_HOLE; jr nz, .remainInvisible */
    if (gb_read_hram(gb, hObjectUnderEntity) != OBJECT_SHOVEL_HOLE) {
        goto remainInvisible;
    }

setOptionsAndReveal:
    /* ld hl, wEntitiesOptions1Table; add hl, bc; ld [hl], ENTITY_OPT1_SPLASH_IN_WATER|ENTITY_OPT1_EXCLUDED_FROM_KILL_ALL; jr .reveal */
    gb_write(gb, wEntitiesOptions1Table + bc, ENTITY_OPT1_SPLASH_IN_WATER | ENTITY_OPT1_EXCLUDED_FROM_KILL_ALL);
    /* fallthrough to .reveal */
    /* .reveal: (items revealed are thrown away from Link) */
    /* ld hl, wEntitiesPrivateState3Table; add hl, bc; ld [hl], b */
    gb_write(gb, wEntitiesPrivateState3Table + bc, 0);
    /* ld hl, wEntitiesPrivateState4Table; add hl, bc; ld [hl], b */
    gb_write(gb, wEntitiesPrivateState4Table + bc, 0);
    /* call GetEntityPrivateCountdown1; ld [hl], $18 */
    gb_write(gb, wEntitiesPrivateCountdown1Table + bc, 0x18);
    /* ld a, $0C; call GetVectorTowardsLink */
    uint8_t vec_x, vec_y;
    GetVectorTowardsLink(gb, &vec_x, &vec_y);
    /* ldh a, [hMultiPurpose1]; cpl; inc a; ld hl, wEntitiesSpeedXTable; add hl, bc; ld [hl], a */
    /* ldh a, [hMultiPurpose0]; cpl; inc a; ld hl, wEntitiesSpeedYTable; add hl, bc; ld [hl], a */
    /* ld hl, wEntitiesSpeedZTable; add hl, bc; ld [hl], $20 */
    gb_write(gb, wEntitiesSpeedZTable + bc, 0x20);
    /* call GetEntitySlowTransitionCountdown; ld [hl], $80 */
    gb_write(gb, wEntitiesSlowTransitionCountdownTable + bc, 0x80);

remainInvisible:
    return;
}

/* func_003_7E0E (03:7E0E) - GetVectorTowardsLink wrapper */
void func_003_7E0E(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* call GetVectorTowardsLink */
    uint8_t x, y;
    GetVectorTowardsLink(gb, &x, &y);
}

/* func_003_61C0 (03:61C0) */
void func_003_61C0(GBState *gb, uint16_t bc) {
    if (!gb) return;

    /* ldh a, [hFrameCounter]; and $03; jr nz, ret_003_61DD */
    if ((gb_read_hram(gb, hFrameCounter) & 0x03) != 0) {
        return;
    }

    /* ld hl, wEntitiesPosZTable; add hl, bc; ld a, [hl]; cp $10; jr z, ret_003_61DD */
    uint8_t pos_z = gb_read(gb, wEntitiesPosZTable + bc);
    if (pos_z == 0x10) {
        return;
    }

    /* bit 7, a; jr z, .jr_61D6 */
    if ((pos_z & 0x80) == 0) {
        goto jr_61D6;
    }

    /* inc [hl]; jr ret_003_61DD */
    gb_write(gb, wEntitiesPosZTable + bc, pos_z + 1);
    return;

jr_61D6:
    /* cp $10; jr nc, .jr_61DC */
    if (pos_z >= 0x10) {
        goto jr_61DC;
    }

    /* inc [hl]; ret */
    gb_write(gb, wEntitiesPosZTable + bc, pos_z + 1);
    return;

jr_61DC:
    /* dec [hl]; ret */
    gb_write(gb, wEntitiesPosZTable + bc, pos_z - 1);
    return;
}

/* Pickable Can Be Collected By Sword Table (03:62FA) */
void PickableCanBeCollectedBySwordTable(GBState *gb) {
    if (!gb) return;
    /* Table of TRUE/FALSE values indexed by entity type */
    /* TRUE for ENTITY_DROPPABLE_HEART, ENTITY_DROPPABLE_RUPEE, ENTITY_SWORD_SHIELD_PICKUP */
    /* FALSE for ENTITY_DROPPABLE_FAIRY, ENTITY_KEY_DROP_POINT */
    /* This is a data table - no code needed */
}

/* Pickable Handle Grabbed By Item If Needed (03:62AF) */
void PickableHandleGrabbedByItemIfNeeded(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles when item is grabbed by sword/bomb */
}

/* Pickable Collect If Needed (03:62EB) */
void PickableCollectIfNeeded(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles item collection */
}

/* Pick Droppable Magic Powder (03:6316) */
void PickDroppableMagicPowder(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles magic powder pickup */
}

/* Pick Secret Seashell (03:631E) */
void PickSecretSeashell(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles secret seashell pickup */
}

/* Increase Value At HL Clamp At 99 (03:6363) */
void IncreaseValueAtHLClampAt99(GBState *gb) {
    if (!gb) return;
    /* Placeholder - increases value at HL, clamping at 99 */
}

/* Pick Droppable Arrows (03:6378) */
void PickDroppableArrows(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles arrow pickup */
}

/* Pick Droppable Bombs (03:6385) */
void PickDroppableBombs(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles bomb pickup */
}

/* Pick Sirens Instrument (03:638F) */
void PickSirensInstrument(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles instrument pickup */
}

/* Hold Pickup In The Air (03:6396) */
void HoldPickupInTheAir(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - holds pickup in the air animation */
}

/* Pick Heart Container (03:63A9) */
void PickHeartContainer(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles heart container pickup */
}

/* Pick Toadstool Or Dungeon Key (03:63B6) */
void PickToadstoolOrDungeonKey(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles toadstool or dungeon key pickup */
}

/* Pick Heart Piece (03:63C3) */
void PickHeartPiece(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles heart piece pickup */
}

/* Pick Guardian Acorn (03:63D0) */
void PickGuardianAcorn(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles guardian acorn pickup */
}

/* Pick Piece Of Power (03:63DD) */
void PickPieceOfPower(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles piece of power pickup */
}

/* Process Power Up (03:63E4) */
void ProcessPowerUp(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - processes power up */
}

/* Move Pickup In The Air (03:63EA) */
void MovePickupInTheAir(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - moves pickup in the air */
}

/* Pick Sword (03:63F4) */
void PickSword(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles sword pickup */
}

/* Give Inventory Item (03:640C) */
void GiveInventoryItem(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - gives inventory item */
}

/* Pick Droppable Key (03:643E) */
void PickDroppableKey(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles key pickup */
}

/* Pick Droppable Heart (03:645E) */
void PickDroppableHeart(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles heart pickup */
}

/* Pick Droppable Rupee (03:646B) */
void PickDroppableRupee(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles rupee pickup */
}

/* Pick Droppable Fairy (03:6478) */
void PickDroppableFairy(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - handles fairy pickup */
}

/* Spawn New Entity (03:4F68) */
void SpawnNewEntity(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - spawns new entity */
}

/* Spawn New Entity In Range (03:4F7E) */
void SpawnNewEntityInRange(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - spawns new entity in range */
}

/* Configure New Entity Helper (03:4FD0) */
void ConfigureNewEntity_helper(GBState *gb, uint16_t bc) {
    if (!gb) return;
    /* Placeholder - helper for configuring new entity */
}