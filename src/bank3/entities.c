#include "bank3/entities.h"
#include "constants/entities.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/gameplay.h"
#include "home/entities.h"
#include "home/bank.h"
#include "home/audio.h"

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
    uint8_t d = (bc >> 8) & 0xFF;

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

    /* call GetEntityPrivateCountdown1 - stub */
    /* Implementation continues... */
}

void EntityInitSideViewPlatformVertical(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitZol(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitMarinAtTheShore(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitBomber(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitBushCrawler(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitTarinBeekeeper(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitTelephone(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitRichard(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void SetMusicTrackIfHasSword(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void SetMusicTrack(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitFinalNightmare(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitDreamShrineBed(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitFishermanUnderBridge(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitKikiTheMonkey(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitFireballShooter(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitAntiKirby(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitMovingBlockMover(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitDesertLanmola(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitFloatingItem2(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitFloatingItem(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void SetZPosForFloatingItem(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitKid71(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitKid72(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitMrWrite(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitBigFairy(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitBowWow(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitOwlEvent(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitSword(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void UnloadEntityIfRoomStatusSet(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitMarin(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitTarin(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitMadamMeowMeow(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitRaftRaftOwner(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitNpcFacingDown(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitStoreOwner(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitWitch(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitShopOwner(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void EntityInitWithRandomDirection(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}

void SetEntityDirection(GBState *gb) {
    if (!gb) return;
    /* Implementation continues... */
}