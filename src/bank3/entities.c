#include "bank3/entities.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/gameplay.h"
#include "constants/directions.h"
#include "home/entities.h"
#include "home/bank.h"
#include "home/audio.h"
#include "home/gameplay.h"
#include "constants/audio.h"

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

gibdoEnd:
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

colorShellEnd:
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

jr_4CEF:
    /* ldh a, [hActiveEntityType]; cp ENTITY_WRECKING_BALL; jr nz, jr_4D04 */
    if (gb_read_hram(gb, hActiveEntityType) == ENTITY_WRECKING_BALL) {
        /* ld a, $16; ld [wWreckingBallRoom], a */
        gb_write(gb, wWreckingBallRoom, 0x16);
        /* ld a, $50; ld [wWreckingBallPosX], a */
        gb_write(gb, wWreckingBallPosX, 0x50);
        /* ld a, $27; ld [wWreckingBallPosY], a */
        gb_write(gb, wWreckingBallPosY, 0x27);
    }

jr_4D04:
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

jr_003_4D22:
    /* call ExecuteActiveEntityHandler_trampoline */
    ExecuteActiveEntityHandler_trampoline(gb, NULL);

    /* call ReturnIfNonInteractive_03.allowInactiveEntity */
    if (ReturnIfNonInteractive_03(gb, true)) {
        return;
    }
    return;

jr_003_4D29:
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

jr_4D66:
    /* rra x4; and $03; ld e, a; ld d, b; ld hl, Data_003_4CA8; add hl, de */
    variant = (countdown >> 4) & 0x03;
    static const uint8_t Data_003_4CA8[4] = { 0x00, 0x01, 0x03, 0x06 };
    /* Note: The assembly continues but we don't have the full implementation here.
       The function would continue with more sprite rendering logic. */
    (void)Data_003_4CA8; /* Suppress unused warning */
}

/* Helper functions */

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