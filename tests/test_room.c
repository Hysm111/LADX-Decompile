#include <stdio.h>
#include <string.h>
#include "gb.h"
#include "home/room.h"
#include "constants/hardware.h"
#include "constants/memory.h"
#include "constants/maps.h"
#include "constants/rooms.h"
#include "constants/gfx.h"
#include "constants/memory.h"
#include "constants/tilesets.h"
#include "constants/sfx.h"

static int failures = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("FAIL: %s at %s:%d\n", msg, __FILE__, __LINE__); \
            failures++; \
        } \
    } while (0)

static void test_mark_trigger_as_resolved(void) {
    GBState gb;
    gb_init(&gb);

    /* Case 1: wRoomEventEffectExecuted already != 0 -> should do nothing */
    gb_write(&gb, wRoomEventEffectExecuted, 1);
    gb_write(&gb, wC1CF, 0xFF);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x00);
    gb_write(&gb, hJingle, 0x00);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wC1CF) == 0xFF, "wC1CF was modified when already executed");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 0x00, "wC5A6 was modified when already executed");
    TEST_ASSERT(gb_read(&gb, hJingle) == 0x00, "hJingle was modified when already executed");

    /* Case 2: wRoomEventEffectExecuted == 0, wC19D == 0 -> should resolve and play jingle */
    gb_write(&gb, wRoomEventEffectExecuted, 0);
    gb_write(&gb, wC1CF, 0xAA);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x00);
    gb_write(&gb, hJingle, 0x00);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wC1CF) == 0x00, "wC1CF not cleared");
    TEST_ASSERT(gb_read(&gb, wRoomEventEffectExecuted) == 1, "wRoomEventEffectExecuted not set to 1");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 1, "wC5A6 not set to 1");
    TEST_ASSERT(gb_read(&gb, hJingle) == JINGLE_PUZZLE_SOLVED, "JINGLE_PUZZLE_SOLVED not played");

    /* Case 3: wRoomEventEffectExecuted == 0, wC19D != 0 -> should resolve without jingle */
    gb_write(&gb, wRoomEventEffectExecuted, 0);
    gb_write(&gb, wC1CF, 0x55);
    gb_write(&gb, wC5A6, 0x00);
    gb_write(&gb, wC19D, 0x01);
    gb_write(&gb, hJingle, 0x00);

    MarkTriggerAsResolved(&gb);

    TEST_ASSERT(gb_read(&gb, wC1CF) == 0x00, "wC1CF not cleared");
    TEST_ASSERT(gb_read(&gb, wRoomEventEffectExecuted) == 1, "wRoomEventEffectExecuted not set to 1");
    TEST_ASSERT(gb_read(&gb, wC5A6) == 1, "wC5A6 not set to 1");
    TEST_ASSERT(gb_read(&gb, hJingle) == 0x00, "hJingle played when wC19D != 0");
}

/* Mock ROM buffer to test SelectRoomTilesets table lookups */
static uint8_t mock_tileset_rom[0x4000 * 0x25]; /* 0x25 banks */

#define ROM_BANK_20_OFFSET(addr) ((0x20 * 0x4000) + ((addr) - 0x4000))
#define ROM_BANK_8_OFFSET(addr) ((0x08 * 0x4000) + ((addr) - 0x4000))

static void test_select_room_tilesets_overworld(void) {
    GBState gb;
    gb_init(&gb);
    memset(mock_tileset_rom, 0, sizeof(mock_tileset_rom));
    gb_attach_rom(&gb, mock_tileset_rom, sizeof(mock_tileset_rom));

    /* Setup mock OverworldTilesetsTable */
    /* Room 0x00 -> de_low = 0 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldTilesetsTable_Addr + 0)] = 0x05; /* New tileset */

    /* Setup RoomSpritesheetGroupsTable for overworld room 0x00: group 0x02 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(RoomSpritesheetGroupsTable_Addr + 0x00)] = 0x02;

    /* Setup OverworldEntitySpritesheetsTable for group 2: group * 4 = 8 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 0)] = 0x10; /* slot 0 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 1)] = 0x20; /* slot 1 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 2)] = 0xFF; /* slot 2: keep */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 8 + 3)] = 0x40; /* slot 3: already 0x40 */

    /* Pre-load state */
    gb_write(&gb, wCurrentBank, 0x01);
    gb_write(&gb, wIsIndoor, 0); /* Overworld */
    gb_write(&gb, hMapRoom, 0x00);
    gb_write(&gb, hMapId, 0x00);
    gb_write(&gb, hWorldTileset, 0x01); /* Different from 0x05 */
    gb_write(&gb, hNeedsUpdatingBGTiles, 0);

    /* Pre-load entity spritesheets: slot 0 is 0x00, slot 1 is 0x00, slot 2 is 0x30, slot 3 is 0x40 */
    gb_write(&gb, wLoadedEntitySpritesheets + 0, 0x00);
    gb_write(&gb, wLoadedEntitySpritesheets + 1, 0x00);
    gb_write(&gb, wLoadedEntitySpritesheets + 2, 0x30);
    gb_write(&gb, wLoadedEntitySpritesheets + 3, 0x40);

    SelectRoomTilesets(&gb);

    /* Verify BG tileset updated and scheduled */
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == 0x05, "Overworld hWorldTileset not updated");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == TILESET_LOAD_WORLD, "hNeedsUpdatingBGTiles not set");

    /* Verify spritesheets scheduled */
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 0) == 0x10, "Spritesheet slot 0 not updated");
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 1) == 0x20, "Spritesheet slot 1 not updated");
    TEST_ASSERT(gb_read(&gb, wEntityTilesSpriteslotIndexA) == 0, "Variant A index not 0");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingEntityTilesA) == 1, "hNeedsUpdatingEntityTilesA not set");
    TEST_ASSERT(gb_read(&gb, wEntityTilesSpriteslotIndexB) == 1, "Variant B index not 1");
    TEST_ASSERT(gb_read(&gb, wNeedsUpdatingEntityTilesB) == 1, "wNeedsUpdatingEntityTilesB not set");

    /* Verify bank restored to saved bank */
    TEST_ASSERT(gb.rom_bank == 0x01, "ROM bank not restored to 0x01");
}

static void test_select_room_tilesets_special_cases(void) {
    GBState gb;
    gb_init(&gb);
    memset(mock_tileset_rom, 0, sizeof(mock_tileset_rom));
    gb_attach_rom(&gb, mock_tileset_rom, sizeof(mock_tileset_rom));

    /* 1. Egg room hack: room 0x07 increments to 0x08 -> de_low = 4 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldTilesetsTable_Addr + 4)] = 0x09;
    gb_write(&gb, wCurrentBank, 0x01);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hMapRoom, ROOM_OW_RIGHT_OF_EGG); /* 0x07 */
    gb_write(&gb, hMapId, 0x00);
    gb_write(&gb, hWorldTileset, 0x00);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == 0x09, "Egg room hack did not load index 4");

    /* 2. Overworld Siren room owl talked: group 0x23 -> 0x24 */
    mock_tileset_rom[ROM_BANK_20_OFFSET(RoomSpritesheetGroupsTable_Addr + ROOM_OW_SIREN)] = 0x23;
    mock_tileset_rom[ROM_BANK_20_OFFSET(OverworldEntitySpritesheetsTable_Addr + 0x24 * 4 + 0)] = 0x55;
    gb_write(&gb, wOverworldRoomStatus + ROOM_OW_SIREN, OW_ROOM_STATUS_OWL_TALKED);
    gb_write(&gb, hMapRoom, ROOM_OW_SIREN);
    gb_write(&gb, wLoadedEntitySpritesheets + 0, 0x00);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 0) == 0x55, "Siren room owl talked group 0x24 not used");

    /* 3. Color dungeon (Map 0xFF) indoors */
    mock_tileset_rom[ROM_BANK_20_OFFSET(ColorDungeonTilesetsTable_Addr + 0x05)] = 0x12;
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    gb_write(&gb, hMapRoom, 0x05);
    gb_write(&gb, hWorldTileset, 0x00);
    gb_write(&gb, hNeedsUpdatingEntityTilesA, 0);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == 0x12, "Color dungeon tileset not read from ColorDungeonTilesetsTable");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingEntityTilesA) == 1, "Color dungeon entity tiles A not set");
    TEST_ASSERT(gb.rom_bank == 0x01, "ROM bank not restored after Color Dungeon");

    /* 4. Indoor camera shop: Map 0x10, room 0xB5 -> group 0x3D */
    mock_tileset_rom[ROM_BANK_20_OFFSET(IndoorEntitySpritesheetsTable_Addr + 0x3D * 4 + 0)] = 0x77;
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_HOUSE);
    gb_write(&gb, hMapRoom, ROOM_INDOOR_B_CAMERA_SHOP);
    gb_write(&gb, wLoadedEntitySpritesheets + 0, 0x00);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, wLoadedEntitySpritesheets + 0) == 0x77, "Indoor camera shop did not use group 0x3D");

    /* 5. W_TILESET_NO_UPDATE indoors: updates hWorldTileset but does NOT set hNeedsUpdatingBGTiles */
    mock_tileset_rom[ROM_BANK_20_OFFSET(IndoorsTilesetsTable_Addr + 0x15)] = W_TILESET_NO_UPDATE;
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, MAP_TAIL_CAVE);
    gb_write(&gb, hMapRoom, 0x15);
    gb_write(&gb, hWorldTileset, 0x00);
    gb_write(&gb, hNeedsUpdatingBGTiles, 0);
    SelectRoomTilesets(&gb);
    TEST_ASSERT(gb_read(&gb, hWorldTileset) == W_TILESET_NO_UPDATE, "W_TILESET_NO_UPDATE not written to hWorldTileset");
    TEST_ASSERT(gb_read(&gb, hNeedsUpdatingBGTiles) == 0, "hNeedsUpdatingBGTiles set on NO_UPDATE");
}

static int mock_load_tileset_calls = 0;
static void mock_load_tileset_cb(GBState *gb) {
    mock_load_tileset_calls++;
    TEST_ASSERT(gb->rom_bank == 0x01, "LoadTileset0F not called in bank 0x01");
}

static int mock_get_chests_calls = 0;
static void mock_get_chests_cb(GBState *gb) {
    mock_get_chests_calls++;
    TEST_ASSERT(gb->rom_bank == 0x14, "GetChestsStatusForRoom not called in bank 0x14");
}

static int mock_func_2a07_calls = 0;
static void mock_func_2a07_cb(GBState *gb) {
    mock_func_2a07_calls++;
    TEST_ASSERT(gb->rom_bank == 0x01, "func_001_5A59 not called in bank 0x01");
}

static int mock_get_status_calls = 0;
static uint16_t mock_get_status_addr(GBState *gb, uint16_t de) {
    mock_get_status_calls++;
    TEST_ASSERT(gb->rom_bank == 0x14, "GetRoomStatusAddressForMapPosition bank not 0x14");
    return 0xD800 + de;
}

static void test_room_trampolines_and_physics(void) {
    GBState gb;
    gb_init(&gb);
    memset(mock_tileset_rom, 0, sizeof(mock_tileset_rom));
    gb_attach_rom(&gb, mock_tileset_rom, sizeof(mock_tileset_rom));

    /* 1. LoadTileset0F_trampoline */
    mock_load_tileset_calls = 0;
    gb.rom_bank = 0x05;
    LoadTileset0F_trampoline(&gb, mock_load_tileset_cb);
    TEST_ASSERT(mock_load_tileset_calls == 1, "LoadTileset0F callback not called");
    TEST_ASSERT(gb.rom_bank == 0x01, "LoadTileset0F final bank not 0x01");

    /* 2. GetChestsStatusForRoom_trampoline */
    mock_get_chests_calls = 0;
    gb_write(&gb, wCurrentBank, 0x04);
    gb.rom_bank = 0x04;
    GetChestsStatusForRoom_trampoline(&gb, mock_get_chests_cb);
    TEST_ASSERT(mock_get_chests_calls == 1, "GetChestsStatusForRoom callback not called");
    TEST_ASSERT(gb.rom_bank == 0x04, "GetChestsStatusForRoom bank not restored to 0x04");

    /* 3. func_2A07 */
    mock_func_2a07_calls = 0;
    gb_write(&gb, wCurrentBank, 0x06);
    gb.rom_bank = 0x06;
    func_2A07(&gb, mock_func_2a07_cb);
    TEST_ASSERT(mock_func_2a07_calls == 1, "func_2A07 callback not called");
    TEST_ASSERT(gb.rom_bank == 0x06, "func_2A07 bank not restored to 0x06");

    /* 4. GetObjectPhysicsFlags for overworld */
    mock_tileset_rom[ROM_BANK_8_OFFSET(OverworldObjectPhysicFlags + 0x0A)] = 0x42;
    mock_tileset_rom[ROM_BANK_8_OFFSET(Indoors1ObjectPhysicFlags + 0x0A)] = 0x84;

    gb_write(&gb, hMapId, 0x00); /* Not Color Dungeon */
    uint8_t flags = GetObjectPhysicsFlags(&gb, 0x0A);
    TEST_ASSERT(flags == 0x42, "Overworld object physics flags mismatch");
    TEST_ASSERT(gb.rom_bank == BANK_ObjectPhysicFlags, "Bank not set to BANK_ObjectPhysicFlags");

    /* 5. GetObjectPhysicsFlags for Color Dungeon */
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    flags = GetObjectPhysicsFlags(&gb, 0x0A);
    TEST_ASSERT(flags == 0x84, "Color dungeon object physics flags mismatch");

    /* 6. GetObjectPhysicsFlags_trampoline */
    gb_write(&gb, wCurrentBank, 0x02);
    gb.rom_bank = 0x02;
    gb_write(&gb, hMapId, 0x00);
    flags = GetObjectPhysicsFlags_trampoline(&gb, 0x0A);
    TEST_ASSERT(flags == 0x42, "Trampoline physics flags mismatch");
    TEST_ASSERT(gb.rom_bank == 0x02, "Trampoline bank not restored to 0x02");

    /* 7. GetObjectPhysicsFlagsAndRestoreBank3 */
    gb.rom_bank = 0x07;
    flags = GetObjectPhysicsFlagsAndRestoreBank3(&gb, 0x0A);
    TEST_ASSERT(flags == 0x42, "RestoreBank3 physics flags mismatch");
    TEST_ASSERT(gb.rom_bank == 0x03, "RestoreBank3 bank not set to 0x03");

    /* 8. GetRoomStatusAddressForMapPosition_trampoline */
    mock_get_status_calls = 0;
    gb_write(&gb, wCurrentBank, 0x03);
    gb.rom_bank = 0x03;
    uint16_t room_addr = GetRoomStatusAddressForMapPosition_trampoline(&gb, 0x2A, mock_get_status_addr);
    TEST_ASSERT(mock_get_status_calls == 1, "GetRoomStatusAddressForMapPosition callback not called");
    TEST_ASSERT(room_addr == 0xD82A, "GetRoomStatusAddressForMapPosition return address mismatch");
    TEST_ASSERT(gb.rom_bank == 0x03, "GetRoomStatusAddressForMapPosition bank not restored to 0x03");
}


static int mock_load_template_calls = 0;
static uint8_t mock_last_template_id = 0;
static void mock_load_room_template_cb(GBState *gb, uint8_t template_id) {
    (void)gb;
    mock_load_template_calls++;
    mock_last_template_id = template_id;
}

static int mock_load_world_map_calls = 0;
static void mock_load_world_map_cb(GBState *gb) {
    (void)gb;
    mock_load_world_map_calls++;
}

static void test_room_objects_and_macros(void) {
    GBState gb;

    /* 1. FillRoomWithConsecutiveObjects - horizontal */
    gb_init(&gb);
    gb_write(&gb, hMultiPurpose0, 0x00); /* horizontal */
    FillRoomWithConsecutiveObjects(&gb, wRoomObjects, 0x22, 4);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0) == 0x22, "Horizontal object 0 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 1) == 0x22, "Horizontal object 1 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 2) == 0x22, "Horizontal object 2 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 3) == 0x22, "Horizontal object 3 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 4) == 0x00, "Horizontal overflow occurred");

    /* 2. FillRoomWithConsecutiveObjects - vertical (advance +16) */
    gb_init(&gb);
    gb_write(&gb, hMultiPurpose0, 0x40); /* vertical */
    FillRoomWithConsecutiveObjects(&gb, wRoomObjects, 0x33, 3);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x00) == 0x33, "Vertical object 0 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x10) == 0x33, "Vertical object 1 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x20) == 0x33, "Vertical object 2 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x01) == 0x00, "Vertical adjacent tile polluted");

    /* 3. SetBankForRoom */
    gb_init(&gb);
    gb_write(&gb, hMapRoom, 0x10); /* < 0x80 */
    uint8_t bank = SetBankForRoom(&gb);
    TEST_ASSERT(bank == BANK_OverworldRoomsFirstHalf, "SetBankForRoom first half mismatch");
    TEST_ASSERT(gb.rom_bank == BANK_OverworldRoomsFirstHalf, "SetBankForRoom rSelectROMBank mismatch");

    gb_write(&gb, hMapRoom, 0x85); /* >= 0x80 */
    bank = SetBankForRoom(&gb);
    TEST_ASSERT(bank == BANK_OverworldRoomsSecondHalf, "SetBankForRoom second half mismatch");
    TEST_ASSERT(gb.rom_bank == BANK_OverworldRoomsSecondHalf, "SetBankForRoom second half rSelectROMBank mismatch");

    /* 4. SetupDestroyableObjectIfNeeded2 & SetupDestroyableObjectIfNeeded */
    /* On GBC with hIsGBC=1, BackupObjectInRAM2 writes to WRAM bank 2 if not ignored */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, hMapRoom, 0x10);

    /* Test OBJECT_SHORT_GRASS: ignored */
    SetupDestroyableObjectIfNeeded2(&gb, wRoomObjects, OBJECT_SHORT_GRASS);
    gb_write(&gb, rSVBK, 2);
    TEST_ASSERT(gb_read(&gb, wRoomObjects) == 0, "OBJECT_SHORT_GRASS should not be backed up");
    gb_write(&gb, rSVBK, 0);

    /* Test OBJECT_ROCKY_GROUND on non-Giant Skull: ignored */
    gb_write(&gb, hMapRoom, 0x10);
    SetupDestroyableObjectIfNeeded2(&gb, wRoomObjects, OBJECT_ROCKY_GROUND);
    gb_write(&gb, rSVBK, 2);
    TEST_ASSERT(gb_read(&gb, wRoomObjects) == 0, "Rocky ground should not be backed up outside Giant Skull");
    gb_write(&gb, rSVBK, 0);

    /* Test OBJECT_ROCKY_GROUND (0x09, in interactive list) on Giant Skull: backed up */
    gb_write(&gb, hMapRoom, ROOM_OW_GIANT_SKULL);
    gb_write(&gb, wRoomObjects, OBJECT_ROCKY_GROUND);
    SetupDestroyableObjectIfNeeded2(&gb, wRoomObjects, OBJECT_ROCKY_GROUND);
    gb_write(&gb, rSVBK, 2);
    TEST_ASSERT(gb_read(&gb, wRoomObjects) == OBJECT_ROCKY_GROUND, "Rocky ground should be backed up on Giant Skull");
    gb_write(&gb, rSVBK, 0);

    /* Test OBJECT_ROCKY_CAVE_DOOR on Eagle's tower: ignored */
    gb_write(&gb, hMapRoom, ROOM_OW_EAGLES_TOWER);
    SetupDestroyableObjectIfNeeded2(&gb, wRoomObjects + 1, OBJECT_ROCKY_CAVE_DOOR);
    gb_write(&gb, rSVBK, 2);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 1) == 0, "Rocky cave door ignored in Eagles Tower");
    gb_write(&gb, rSVBK, 0);

    /* Test SetupDestroyableObjectIfNeeded (macro version with bank 0x24) */
    gb_write(&gb, hMapRoom, 0x30);
    gb_write(&gb, wRoomObjects + 2, 0x5E); /* 0x5E is in interactive list */
    SetupDestroyableObjectIfNeeded(&gb, wRoomObjects + 2, 0x5E);
    gb_write(&gb, rSVBK, 2);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 2) == 0x5E, "Macro destroyable object backed up to WRAM2");
    gb_write(&gb, rSVBK, 0);
    TEST_ASSERT(gb.rom_bank == 0x24, "Macro destroyable object return bank not 0x24");

    /* 5. CopyObjectToActiveRoomMap */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, hMapRoom, 0x10);
    CopyObjectToActiveRoomMap(&gb, 0x05, 0x5E); /* 0x5E is in interactive list */
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x05) == 0x5E, "CopyObjectToActiveRoomMap object value mismatch");
    gb_write(&gb, rSVBK, 2);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x05) == 0x5E, "CopyObjectToActiveRoomMap WRAM2 backup mismatch");
    gb_write(&gb, rSVBK, 0);

    /* 6. FillRoomMapWithObject */
    gb_init(&gb);
    /* Pre-fill wRoomObjectsArea ($D700-$D7FF) with canary 0xEE */
    for (uint16_t addr = wRoomObjectsArea; addr < wRoomObjectsArea + 0x100; addr++) {
        gb_write(&gb, addr, 0xEE);
    }
    FillRoomMapWithObject(&gb, 0x5A);

    TEST_ASSERT(gb_read(&gb, hMultiPurposeH) == 0x5A, "FillRoomMapWithObject hMultiPurposeH not set");
    /* Verify row 1 (0xD710-0xD71F): col 0 ($D710) is 0xEE, cols 1-10 ($D711-$D71A) are 0x5A, cols 11-15 ($D71B-$D71F) are 0xEE */
    TEST_ASSERT(gb_read(&gb, 0xD710) == 0xEE, "Border col 0 should not be overwritten");
    for (uint16_t col = 1; col <= 10; col++) {
        TEST_ASSERT(gb_read(&gb, 0xD710 + col) == 0x5A, "Active room object tile mismatch");
    }
    for (uint16_t col = 11; col <= 15; col++) {
        TEST_ASSERT(gb_read(&gb, 0xD710 + col) == 0xEE, "Border col > 10 should not be overwritten");
    }

    /* 7. LoadRoomTemplate_trampoline */
    gb_init(&gb);
    mock_load_template_calls = 0;
    mock_last_template_id = 0;
    gb_write(&gb, hRoomBank, 0x0A);
    LoadRoomTemplate_trampoline(&gb, 0x03, mock_load_room_template_cb);
    TEST_ASSERT(mock_load_template_calls == 1, "LoadRoomTemplate callback not called");
    TEST_ASSERT(mock_last_template_id == 0x03, "LoadRoomTemplate template_id mismatch");
    TEST_ASSERT(gb.rom_bank == 0x0A, "LoadRoomTemplate did not restore hRoomBank (0x0A)");

    /* 9. ObjectPositionToRoomObjectAddress */
    TEST_ASSERT(ObjectPositionToRoomObjectAddress(0x25) == (wRoomObjects + 0x25), "ObjectPositionToRoomObjectAddress mismatch");

    /* 10. CopyIndoorsMacroObjectsToRoom */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, hMapRoom, 0x10);
    gb_write(&gb, wC19C, 0);

    /* Setup mock offsets in WRAM at 0xC500, and mock IDs at 0xC510 */
    uint16_t offsets_addr = 0xC500;
    uint16_t ids_addr = 0xC510;
    gb_write(&gb, offsets_addr + 0, 0x00);
    gb_write(&gb, offsets_addr + 1, 0x01);
    gb_write(&gb, offsets_addr + 2, 0xFF); /* Sentinel */

    gb_write(&gb, ids_addr + 0, OBJECT_CAVE_DOOR);
    gb_write(&gb, ids_addr + 1, 0x5E);

    uint16_t base_hl = wRoomObjects + 0x10; /* 0xD721 */
    CopyIndoorsMacroObjectsToRoom(&gb, base_hl, offsets_addr, ids_addr);

    TEST_ASSERT(gb_read(&gb, base_hl + 0x00) == OBJECT_CAVE_DOOR, "Door object not copied");
    TEST_ASSERT(gb_read(&gb, base_hl + 0x01) == 0x5E, "Second object not copied");
    TEST_ASSERT(gb_read(&gb, wC19C) == 1, "wC19C counter not incremented for door");
    TEST_ASSERT(gb_read(&gb, wWarpPositions + 0) == 0x10, "wWarpPositions[0] incorrect");

    /* 11. CopyOutdoorsMacroObjectsToRoom */
    gb_init(&gb);
    gb_write(&gb, hIsGBC, 1);
    gb_write(&gb, hMapRoom, 0x30);
    gb_write(&gb, wC19C, 2);

    gb_write(&gb, offsets_addr + 0, 0x00);
    gb_write(&gb, offsets_addr + 1, 0x10);
    gb_write(&gb, offsets_addr + 2, 0xFF); /* Sentinel */

    gb_write(&gb, ids_addr + 0, OBJECT_ROCKY_CAVE_DOOR);
    gb_write(&gb, ids_addr + 1, 0x5E);

    base_hl = wRoomObjects + 0x05; /* 0xD716 */
    CopyOutdoorsMacroObjectsToRoom(&gb, base_hl, offsets_addr, ids_addr);

    TEST_ASSERT(gb_read(&gb, base_hl + 0x00) == OBJECT_ROCKY_CAVE_DOOR, "Outdoor door not copied");
    TEST_ASSERT(gb_read(&gb, base_hl + 0x10) == 0x5E, "Outdoor second object not copied");
    TEST_ASSERT(gb_read(&gb, wC19C) == 3, "wC19C counter not updated to 3");
    TEST_ASSERT(gb_read(&gb, wWarpPositions + 2) == 0x05, "wWarpPositions[2] incorrect");
    TEST_ASSERT(gb.rom_bank == 0x24, "CopyOutdoorsMacroObjectsToRoom bank not 0x24");

    /* 8. LoadWorldMapBGMap_trampoline */
    gb_init(&gb);
    mock_load_world_map_calls = 0;
    LoadWorldMapBGMap_trampoline(&gb, mock_load_world_map_cb);
    TEST_ASSERT(mock_load_world_map_calls == 1, "LoadWorldMapBGMap callback not called");
    TEST_ASSERT(gb.rom_bank == BANK_LoadWorldMapBGMap, "LoadWorldMapBGMap bank not 0x20");
}

static void test_indoor_doors(void) {
    GBState gb;

    /* 1. MakeListOfDoorPositions */
    gb_init(&gb);
    MakeListOfDoorPositions(&gb, DOOR_TYPE_SHUTTER_BOTTOM, 0x37);
    TEST_ASSERT(gb_read(&gb, wDoorPositions + DOOR_TYPE_SHUTTER_BOTTOM) == 0x37, "MakeListOfDoorPositions YX mismatch");
    TEST_ASSERT(gb_read(&gb, wDoorYPositions + DOOR_TYPE_SHUTTER_BOTTOM) == 0x30, "MakeListOfDoorPositions Y mismatch");
    TEST_ASSERT(gb_read(&gb, wDoorXPositions + DOOR_TYPE_SHUTTER_BOTTOM) == 0x70, "MakeListOfDoorPositions X mismatch");

    /* 2. UpdateIndoorRoomStatus */
    /* Case A: Indoors A map ($01) */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    gb_write(&gb, hMapRoom, 0x15);
    gb_write(&gb, wIndoorARoomStatus + 0x15, 0x10); /* Pre-existing flag */
    UpdateIndoorRoomStatus(&gb, ROOM_STATUS_DOOR_OPEN_UP);
    TEST_ASSERT(gb_read(&gb, wIndoorARoomStatus + 0x15) == (0x10 | ROOM_STATUS_DOOR_OPEN_UP), "UpdateIndoorRoomStatus Indoors A mismatch");
    TEST_ASSERT(gb_read(&gb, hRoomStatus) == (0x10 | ROOM_STATUS_DOOR_OPEN_UP), "UpdateIndoorRoomStatus hRoomStatus mismatch");

    /* Case B: Indoors B map ($08, inside [0x06, 0x1A)) */
    gb_write(&gb, hMapId, 0x08);
    gb_write(&gb, hMapRoom, 0x22);
    gb_write(&gb, wIndoorBRoomStatus + 0x22, 0x00);
    UpdateIndoorRoomStatus(&gb, ROOM_STATUS_DOOR_OPEN_DOWN);
    TEST_ASSERT(gb_read(&gb, wIndoorBRoomStatus + 0x22) == ROOM_STATUS_DOOR_OPEN_DOWN, "UpdateIndoorRoomStatus Indoors B mismatch");
    TEST_ASSERT(gb_read(&gb, hRoomStatus) == ROOM_STATUS_DOOR_OPEN_DOWN, "UpdateIndoorRoomStatus Indoors B hRoomStatus mismatch");

    /* Case C: Color Dungeon map ($FF) */
    gb_write(&gb, hMapId, MAP_COLOR_DUNGEON);
    gb_write(&gb, hMapRoom, 0x05);
    gb_write(&gb, wColorDungeonRoomStatus + 0x05, 0x02);
    UpdateIndoorRoomStatus(&gb, ROOM_STATUS_DOOR_OPEN_RIGHT);
    TEST_ASSERT(gb_read(&gb, wColorDungeonRoomStatus + 0x05) == (0x02 | ROOM_STATUS_DOOR_OPEN_RIGHT), "UpdateIndoorRoomStatus Color Dungeon mismatch");

    /* 3. Open Doors (Top, Bottom, Left, Right) */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    gb_write(&gb, hMapRoom, 0x00);

    /* OpenDoorTop: $43, $44 */
    LoadObject_OpenDoorTop(&gb, 0x14);
    uint16_t door_addr = wRoomObjects + 0x14;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0x43, "OpenDoorTop tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0x44, "OpenDoorTop tile 1 mismatch");
    TEST_ASSERT((gb_read(&gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_UP) != 0, "OpenDoorTop status flag missing");

    /* OpenDoorBottom: $8C, $08 */
    LoadObject_OpenDoorBottom(&gb, 0x24);
    door_addr = wRoomObjects + 0x24;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0x8C, "OpenDoorBottom tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0x08, "OpenDoorBottom tile 1 mismatch");
    TEST_ASSERT((gb_read(&gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_DOWN) != 0, "OpenDoorBottom status flag missing");

    /* OpenDoorLeft: $09, $0A (vertical) */
    LoadObject_OpenDoorLeft(&gb, 0x30);
    door_addr = wRoomObjects + 0x30;
    TEST_ASSERT(gb_read(&gb, door_addr + 0x00) == 0x09, "OpenDoorLeft tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x10) == 0x0A, "OpenDoorLeft tile 1 mismatch");
    TEST_ASSERT((gb_read(&gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_LEFT) != 0, "OpenDoorLeft status flag missing");

    /* OpenDoorRight: $0B, $0C (vertical) */
    LoadObject_OpenDoorRight(&gb, 0x38);
    door_addr = wRoomObjects + 0x38;
    TEST_ASSERT(gb_read(&gb, door_addr + 0x00) == 0x0B, "OpenDoorRight tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x10) == 0x0C, "OpenDoorRight tile 1 mismatch");
    TEST_ASSERT((gb_read(&gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_RIGHT) != 0, "OpenDoorRight status flag missing");

    /* 4. Key Doors: closed vs open */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    gb_write(&gb, hMapRoom, 0x00);
    /* Closed Key Door Top: $2D, $2E */
    LoadObject_KeyDoorTop(&gb, 0x12);
    door_addr = wRoomObjects + 0x12;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0x2D, "KeyDoorTop closed tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0x2E, "KeyDoorTop closed tile 1 mismatch");
    TEST_ASSERT(gb_read(&gb, wDoorPositions + DOOR_TYPE_KEY_TOP) == 0x12, "KeyDoorTop position not saved");

    /* Already open Key Door Top: should become open door ($43, $44) */
    gb_write(&gb, hRoomStatus, ROOM_STATUS_DOOR_OPEN_UP);
    LoadObject_KeyDoorTop(&gb, 0x12);
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0x43, "KeyDoorTop open tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0x44, "KeyDoorTop open tile 1 mismatch");

    /* Key Door Bottom closed: $2F, $30 */
    gb_write(&gb, hRoomStatus, 0);
    LoadObject_KeyDoorBottom(&gb, 0x20);
    door_addr = wRoomObjects + 0x20;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0x2F, "KeyDoorBottom closed tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0x30, "KeyDoorBottom closed tile 1 mismatch");

    /* Key Door Left closed: $31, $32 */
    LoadObject_KeyDoorLeft(&gb, 0x40);
    door_addr = wRoomObjects + 0x40;
    TEST_ASSERT(gb_read(&gb, door_addr + 0x00) == 0x31, "KeyDoorLeft closed tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x10) == 0x32, "KeyDoorLeft closed tile 1 mismatch");

    /* Key Door Right closed: $33, $34 */
    LoadObject_KeyDoorRight(&gb, 0x50);
    door_addr = wRoomObjects + 0x50;
    TEST_ASSERT(gb_read(&gb, door_addr + 0x00) == 0x33, "KeyDoorRight closed tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x10) == 0x34, "KeyDoorRight closed tile 1 mismatch");

    /* 5. Shutter Doors */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    gb_write(&gb, wShutterDoorsMask, 0);
    LoadObject_ShutterDoorTop(&gb, 0x14);
    TEST_ASSERT((gb_read(&gb, wShutterDoorsMask) & DOOR_TYPE_SHUTTER_TOP_BIT) != 0, "ShutterDoorTop mask not set");
    TEST_ASSERT(gb_read(&gb, wShutterDoorsMask2) == gb_read(&gb, wShutterDoorsMask), "wShutterDoorsMask2 mismatch");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x14) == 0x43, "ShutterDoorTop open tile 0 mismatch");

    LoadObject_ShutterDoorBottom(&gb, 0x24);
    TEST_ASSERT((gb_read(&gb, wShutterDoorsMask) & DOOR_TYPE_SHUTTER_BOTTOM_BIT) != 0, "ShutterDoorBottom mask not set");

    LoadObject_ShutterDoorLeft(&gb, 0x30);
    TEST_ASSERT((gb_read(&gb, wShutterDoorsMask) & DOOR_TYPE_SHUTTER_LEFT_BIT) != 0, "ShutterDoorLeft mask not set");

    LoadObject_ShutterDoorRight(&gb, 0x38);
    TEST_ASSERT((gb_read(&gb, wShutterDoorsMask) & DOOR_TYPE_SHUTTER_RIGHT_BIT) != 0, "ShutterDoorRight mask not set");

    /* 6. Boss Door */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    LoadObject_BossDoor(&gb, 0x15);
    door_addr = wRoomObjects + 0x15;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0xA4, "BossDoor tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0xA5, "BossDoor tile 1 mismatch");
    TEST_ASSERT(gb_read(&gb, wDoorPositions + DOOR_TYPE_BOSS_TOP) == 0x15, "BossDoor position not saved");

    /* 7. Stairs Door ($AF, $B0) */
    LoadObject_StairsDoor(&gb, 0x25);
    door_addr = wRoomObjects + 0x25;
    TEST_ASSERT(gb_read(&gb, door_addr + 0x00) == 0xAF, "StairsDoor tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x10) == 0xB0, "StairsDoor tile 1 mismatch");

    /* 8. Revolving Door ($B1, $B2) */
    LoadObject_RevolvingDoor(&gb, 0x35);
    door_addr = wRoomObjects + 0x35;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0xB1, "RevolvingDoor tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0xB2, "RevolvingDoor tile 1 mismatch");

    /* 9. One Way Arrow ($45, $46) */
    LoadObject_OneWayArrow(&gb, 0x45);
    door_addr = wRoomObjects + 0x45;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0x45, "OneWayArrow tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0x46, "OneWayArrow tile 1 mismatch");

    /* 10. Dungeon Entrance */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    LoadObject_DungeonEntrance(&gb, 0x10);
    door_addr = wRoomObjects + 0x10;
    TEST_ASSERT(gb_read(&gb, door_addr + 0x00) == 0xB3, "DungeonEntrance (0,0) tile mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x01) == 0xB4, "DungeonEntrance (0,1) tile mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x10) == 0xB6, "DungeonEntrance (1,0) tile mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 0x23) == 0xBD, "DungeonEntrance (2,3) tile mismatch");
    TEST_ASSERT((gb_read(&gb, hRoomStatus) & ROOM_STATUS_DOOR_OPEN_DOWN) != 0, "DungeonEntrance status not set");

    /* 11. Indoor Entrance: normal vs stolen bug */
    /* Normal */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    LoadObject_IndoorEntrance(&gb, 0x28);
    door_addr = wRoomObjects + 0x28;
    TEST_ASSERT(gb_read(&gb, door_addr + 0) == 0xC1, "IndoorEntrance tile 0 mismatch");
    TEST_ASSERT(gb_read(&gb, door_addr + 1) == 0xC2, "IndoorEntrance tile 1 mismatch");

    /* Stolen bug trigger */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x08); /* Inside Indoors B */
    gb_write(&gb, hMapRoom, ROOM_INDOOR_B_KANALET_MAIN_ENTRANCE);
    gb_write(&gb, wHasStolenFromShop, 1);
    LoadObject_IndoorEntrance(&gb, 0x28);
    TEST_ASSERT((gb_read(&gb, wShutterDoorsMask) & DOOR_TYPE_SHUTTER_BOTTOM_BIT) != 0, "Stolen shop bug should trigger shutter bottom");

    /* 12. DispatchIndoorDoorObject */
    gb_init(&gb);
    gb_write(&gb, hMapId, 0x01);
    TEST_ASSERT(DispatchIndoorDoorObject(&gb, OBJECT_DOOR_TYPE_KEY_TOP, 0x12) == true, "Dispatch key door top failed");
    TEST_ASSERT(DispatchIndoorDoorObject(&gb, OBJECT_DUNGEON_ENTRANCE, 0x10) == true, "Dispatch dungeon entrance failed");
    TEST_ASSERT(DispatchIndoorDoorObject(&gb, 0x04, 0x10) == false, "Dispatch non-door object should return false");
}

static void test_load_room_object(void) {
    GBState gb;

    /* 1. Two-byte single block object on Overworld */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, 0xC500, 0x15); /* pos */
    gb_write(&gb, 0xC501, 0x09); /* obj_type: rocky ground */
    size_t consumed = LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(consumed == 2, "Two-byte object should consume 2 bytes");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x15) == 0x09, "Object not copied to wRoomObjects");

    /* 2. Three-byte multiple-block horizontal object */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, 0xC500, 0x83); /* 3 blocks horizontal */
    gb_write(&gb, 0xC501, 0x20); /* pos */
    gb_write(&gb, 0xC502, 0x09); /* rocky ground */
    consumed = LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(consumed == 3, "Three-byte object should consume 3 bytes");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x20) == 0x09, "Block 0 not placed");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x21) == 0x09, "Block 1 not placed");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x22) == 0x09, "Block 2 not placed");

    /* 3. Three-byte multiple-block vertical object */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, 0xC500, 0xC2); /* 2 blocks vertical (bit 6 set) */
    gb_write(&gb, 0xC501, 0x12); /* pos */
    gb_write(&gb, 0xC502, 0x09);
    consumed = LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(consumed == 3, "Three-byte object should consume 3 bytes");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x12) == 0x09, "Vertical Block 0 not placed");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x22) == 0x09, "Vertical Block 1 not placed (+16)");

    /* 4. Overworld Waterfall */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, 0xC500, 0x10);
    gb_write(&gb, 0xC501, OBJECT_WATERFALL);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wC50E) == OBJECT_WATERFALL, "Waterfall wC50E not set");

    /* 5. Overworld Weather Vane */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, 0xC500, 0x24); /* pos bit 5 is set (0x20) */
    gb_write(&gb, 0xC501, OBJECT_WEATHER_VANE_BASE);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x24) == OBJECT_GROUND_STAIRS, "Weather vane base should become stairs");

    /* 6. Overworld Monkey Bridge built */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hRoomStatus, 0x10); /* bit 4 set: bridge built */
    gb_write(&gb, 0xC500, 0x18);
    gb_write(&gb, 0xC501, OBJECT_MONKEY_BRIDGE_TOP);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x18) == OBJECT_MONKEY_BRIDGE_BUILT, "Monkey bridge should become built bridge");

    /* 7. Overworld Closed Gate opened */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hRoomStatus, 0x10); /* bit 4 set: gate opened */
    gb_write(&gb, 0xC500, 0x14);
    gb_write(&gb, 0xC501, OBJECT_CLOSED_GATE);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x14) == OBJECT_CAVE_DOOR, "Closed gate should become cave door");

    /* 8. Overworld Bombable Cave Door bombed */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hRoomStatus, 0x04); /* bit 2 set: bombed */
    gb_write(&gb, 0xC500, 0x22);
    gb_write(&gb, 0xC501, OBJECT_BOMBABLE_CAVE_DOOR);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x22) == OBJECT_ROCKY_CAVE_DOOR, "Bombable cave door should become rocky cave door");

    /* 9. Overworld Bush masking stairs cut */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, hRoomStatus, 0x10);
    gb_write(&gb, hMapRoom, ROOM_OW_RIGHT_OF_EGG);
    gb_write(&gb, 0xC500, 0x33);
    gb_write(&gb, 0xC501, OBJECT_BUSH_GROUND_STAIRS);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x33) == OBJECT_GROUND_STAIRS, "Bush over stairs should become ground stairs");

    /* 10. Overworld stairs position calculation */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 0);
    gb_write(&gb, 0xC500, 0x47); /* Y=4, X=7 */
    gb_write(&gb, 0xC501, OBJECT_GROUND_STAIRS);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, hStaircase) == STAIRCASE_INACTIVE, "hStaircase not set to inactive");
    TEST_ASSERT(gb_read(&gb, hStaircasePosY) == 0x40 + 0x10, "hStaircasePosY mismatch");
    TEST_ASSERT(gb_read(&gb, hStaircasePosX) == (0x07 << 4) + 0x08, "hStaircasePosX mismatch");

    /* 11. Indoor Conveyor belt */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, wConveyorBeltsCount, 2);
    gb_write(&gb, 0xC500, 0x11);
    gb_write(&gb, 0xC501, OBJECT_CONVEYOR_BOTTOM);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wConveyorBeltsCount) == 3, "wConveyorBeltsCount not incremented");

    /* 12. Indoor Switch & Mobile block */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, 0xC500, 0x12);
    gb_write(&gb, 0xC501, OBJECT_SWITCH_BUTTON);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomSwitchableObject) == ROOM_SWITCHABLE_OBJECT_SWITCH_BUTTON, "Switch button not registered");

    gb_write(&gb, 0xC502, 0x13);
    gb_write(&gb, 0xC503, OBJECT_RAISED_BLOCK);
    LoadRoomObject(&gb, 0xC502);
    TEST_ASSERT(gb_read(&gb, wRoomSwitchableObject) == ROOM_SWITCHABLE_OBJECT_MOBILE_BLOCK, "Movable block not registered");

    /* 13. Indoor Bombable walls */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hRoomStatus, 0x04); /* Top wall bombed */
    gb_write(&gb, 0xC500, 0x14);
    gb_write(&gb, 0xC501, OBJECT_BOMBABLE_WALL_TOP);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x14) == OBJECT_BOMBED_PASSAGE_VERTICAL, "Top bombable wall not replaced");

    gb_write(&gb, hRoomStatus, 0x02); /* Left wall bombed */
    gb_write(&gb, 0xC502, 0x20);
    gb_write(&gb, 0xC503, OBJECT_BOMBABLE_WALL_LEFT);
    LoadRoomObject(&gb, 0xC502);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x20) == OBJECT_BOMBED_PASSAGE_HORIZONTAL, "Left bombable wall not replaced");

    /* 14. Indoor Chest opened */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hRoomStatus, 0x10); /* Chest opened */
    gb_write(&gb, 0xC500, 0x25);
    gb_write(&gb, 0xC501, OBJECT_CHEST_CLOSED);
    LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x25) == OBJECT_CHEST_OPEN, "Closed chest should be replaced with open chest");

    /* 15. Indoor Hidden stairs: not visible */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hRoomStatus, 0x00); /* Stairs not visible */
    gb_write(&gb, 0xC500, 0x26);
    gb_write(&gb, 0xC501, OBJECT_HIDDEN_STAIRS_DOWN);
    gb_write(&gb, wRoomObjects + 0x26, 0x00);
    consumed = LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(consumed == 2, "Hidden stairs should consume 2 bytes");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x26) == 0x00, "Hidden stairs should not be placed when not visible");

    /* 16. Indoor Door dispatch */
    gb_init(&gb);
    gb_write(&gb, wIsIndoor, 1);
    gb_write(&gb, hMapId, 0x01);
    gb_write(&gb, 0xC500, 0x30);
    gb_write(&gb, 0xC501, OBJECT_DOOR_TYPE_KEY_TOP);
    consumed = LoadRoomObject(&gb, 0xC500);
    TEST_ASSERT(consumed == 2, "Door object should consume 2 bytes");
    TEST_ASSERT(gb_read(&gb, wRoomObjects + 0x30) == 0x2D, "Door object should be loaded via door handler");
}
void run_room_tests(void) {
    printf("[*] Running LoadRoomObject tests...\n");
    test_load_room_object();

    printf("[*] Running Indoor doors tests...\n");
    test_indoor_doors();

    printf("[*] Running Room objects and macros tests...\n");
    test_room_objects_and_macros();

    printf("[*] Running MarkTriggerAsResolved tests...\n");
    test_mark_trigger_as_resolved();

    printf("[*] Running SelectRoomTilesets tests...\n");
    test_select_room_tilesets_overworld();
    test_select_room_tilesets_special_cases();

    printf("[*] Running Room trampolines and physics tests...\n");
    test_room_trampolines_and_physics();

    if (failures == 0) {
        printf("  [PASS] All room.asm functions verified successfully!\n\n");
    }
}
