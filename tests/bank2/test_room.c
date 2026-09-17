#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/room_events.h"
#include "bank2/room_transition.h"
#include "constants/gameplay.h"
#include "constants/gfx.h"
#include "constants/hardware.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"
#include "constants/sfx.h"

#include <assert.h>
#include <stdint.h>

void test_bank2_room(void) {
    /* Test 60: Bank 2 Debug Warp Tables (DebugWarpRooms, DebugWarpMaps) */
    {
        assert(sizeof(DebugWarpRooms) == 11);
        assert(sizeof(DebugWarpMaps) == 11);
        assert(DebugWarpRooms[0] == 0x30);
        assert(DebugWarpRooms[1] == 0x33);
        assert(DebugWarpRooms[2] == 0x81);
        assert(DebugWarpRooms[3] == 0x01);
        assert(DebugWarpRooms[4] == 0x28);
        assert(DebugWarpRooms[5] == 0x56);
        assert(DebugWarpRooms[6] == 0x68);
        assert(DebugWarpRooms[7] == 0x87);
        assert(DebugWarpRooms[8] == 0xB3);
        assert(DebugWarpRooms[9] == 0xE6);
        assert(DebugWarpRooms[10] == 0x0A);

        assert(DebugWarpMaps[0] == MAP_BOTTLE_GROTTO);
        assert(DebugWarpMaps[2] == MAP_CATFISHS_MAW);
        assert(DebugWarpMaps[3] == MAP_TAIL_CAVE);
        assert(DebugWarpMaps[5] == MAP_KEY_CAVERN);
        assert(DebugWarpMaps[6] == MAP_ANGLERS_TUNNEL);
        assert(DebugWarpMaps[8] == MAP_FACE_SHRINE);
        assert(DebugWarpMaps[9] == MAP_EAGLES_TOWER);
        assert(DebugWarpMaps[10] == MAP_TURTLE_ROCK);
    }

    /* Test 61: GetRoomStatusAddress (02:5B9F) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Overworld room (wIsIndoor == 0) -> wOverworldRoomStatus + room */
        gb_write(&gb, wIsIndoor, 0);
        gb_write_hram(&gb, hMapRoom, 0x45);
        assert(GetRoomStatusAddress(&gb) == (wOverworldRoomStatus + 0x45));

        /* 2. Color dungeon (wIsIndoor != 0, hMapId == MAP_COLOR_DUNGEON) -> wColorDungeonRoomStatus + room */
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapId, MAP_COLOR_DUNGEON);
        gb_write_hram(&gb, hMapRoom, 0x12);
        assert(GetRoomStatusAddress(&gb) == (wColorDungeonRoomStatus + 0x12));

        /* 3. Regular Dungeon indoors A (wIsIndoor == 1, hMapId < MAP_INDOORS_B_START, e.g. MAP_KEY_CAVERN) -> wIndoorARoomStatus + room */
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapId, MAP_KEY_CAVERN);
        gb_write_hram(&gb, hMapRoom, 0x2A);
        assert(GetRoomStatusAddress(&gb) == (wIndoorARoomStatus + 0x2A));

        /* 4. Regular Dungeon indoors B (wIsIndoor == 1, MAP_INDOORS_B_START <= hMapId < MAP_INDOORS_B_END, e.g. MAP_TURTLE_ROCK) -> wIndoorBRoomStatus + room */
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapId, MAP_TURTLE_ROCK);
        gb_write_hram(&gb, hMapRoom, 0x3C);
        assert(GetRoomStatusAddress(&gb) == (wIndoorBRoomStatus + 0x3C));

        /* 5. Cave indoors A (wIsIndoor == 1, hMapId >= MAP_INDOORS_B_END, e.g. MAP_CAVE_WATER = 0x1F) -> wIndoorARoomStatus + room */
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapId, MAP_CAVE_WATER);
        gb_write_hram(&gb, hMapRoom, 0x05);
        assert(GetRoomStatusAddress(&gb) == (wIndoorARoomStatus + 0x05));
    }

    /* Test 62: EnqueueDoorUnlockedSfx (02:5420) & label_002_5425 Key Drop Point Init (02:5425) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. EnqueueDoorUnlockedSfx */
        EnqueueDoorUnlockedSfx(&gb);
        assert(gb_read_hram(&gb, hNoiseSfx) == NOISE_SFX_DOOR_UNLOCKED);

        /* 2. label_002_5425: Color Dungeon, regular room -> ENTITY_KEY_DROP_POINT, X=0x48, Y=0x3C, Z=0x70 */
        gb_init(&gb);
        gb_write_hram(&gb, hMapId, MAP_COLOR_DUNGEON);
        gb_write_hram(&gb, hMapRoom, 0x01);
        label_002_5425(&gb, mock_spawn_entity_slot3);
        assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x48);
        assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x3C);
        assert(gb_read(&gb, wEntitiesPosZTable + 3) == 0x70);

        /* 3. label_002_5425: Color Dungeon, Marin Bridge -> X=0x58 */
        gb_init(&gb);
        gb_write_hram(&gb, hMapId, MAP_COLOR_DUNGEON);
        gb_write_hram(&gb, hMapRoom, ROOM_OW_MARIN_BRIDGE);
        label_002_5425(&gb, mock_spawn_entity_slot3);
        assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x58);
        assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x3C);
        assert(gb_read(&gb, wEntitiesPosZTable + 3) == 0x70);

        /* 4. label_002_5425: Standard dungeon (hMapId < MAP_CAVE_B) -> ENTITY_KEY_DROP_POINT, X=0x28 */
        gb_init(&gb);
        gb_write_hram(&gb, hMapId, MAP_TAIL_CAVE);
        label_002_5425(&gb, mock_spawn_entity_slot3);
        assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x28);
        assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x3C);
        assert(gb_read(&gb, wEntitiesPosZTable + 3) == 0x70);

        /* 5. label_002_5425: Overworld / Cave (hMapId >= MAP_CAVE_B) -> ENTITY_HIDING_SLIME_KEY, X=0x28 */
        gb_init(&gb);
        gb_write_hram(&gb, hMapId, MAP_CAVE_B);
        label_002_5425(&gb, mock_spawn_entity_slot3);
        assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x28);
        assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x3C);
        assert(gb_read(&gb, wEntitiesPosZTable + 3) == 0x70);

        /* 6. label_002_5425: Spawn failure */
        gb_init(&gb);
        label_002_5425(&gb, mock_spawn_entity_fail);
        assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x00);
    }

    /* Test 63: TryOpenKeyDoor (02:53B0) */
    {
        GBState gb;

        /* 1. hMultiPurposeG == 0x40 -> Spawns pushed block */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurposeG, 0x40);
        gb_write_hram(&gb, hMultiPurpose4, 0x34); /* left grid: 0x30 */
        gb_write_hram(&gb, hMultiPurpose5, 0x68); /* top grid: 0x60 */
        gb_write(&gb, (uint16_t)(wEntitiesStatusTable + 3), 0x05);

        TryOpenKeyDoor(&gb, mock_sync_flags, mock_reveal_object, mock_spawn_entity_slot3);

        /* status decremented from 5 to 4 */
        assert(gb_read(&gb, (uint16_t)(wEntitiesStatusTable + 3)) == 0x04);
        /* pos_x = 0x30 + 0x08 = 0x38, pos_y = 0x60 + 0x10 = 0x70 */
        assert(gb_read(&gb, wEntitiesPosXTable + 3) == 0x38);
        assert(gb_read(&gb, wEntitiesPosYTable + 3) == 0x70);

        /* 2. Keys count == 0 -> does not unlock */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurposeG, 0x00);
        gb_write(&gb, wSmallKeysCount, 0x00);
        g_mock_sync_flags_calls = 0;
        g_mock_reveal_object_calls = 0;

        TryOpenKeyDoor(&gb, mock_sync_flags, mock_reveal_object, mock_spawn_entity_slot3);

        assert(gb_read(&gb, wSmallKeysCount) == 0);
        assert(g_mock_sync_flags_calls == 0);
        assert(g_mock_reveal_object_calls == 0);
        assert(gb_read_hram(&gb, hNoiseSfx) == 0);

        /* 3. Keys count > 0 -> Unlocks key door */
        gb_init(&gb);
        gb_write_hram(&gb, hMultiPurposeG, 0x00);
        gb_write(&gb, wSmallKeysCount, 0x03);
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapId, MAP_TAIL_CAVE);
        gb_write_hram(&gb, hMapRoom, 0x14);
        gb_write_hram(&gb, hMultiPurpose4, 0x56);
        gb_write_hram(&gb, hMultiPurpose5, 0x89);
        gb_write(&gb, (uint16_t)(wIndoorARoomStatus + 0x14), 0x01);
        g_mock_sync_flags_calls = 0;
        g_mock_reveal_object_calls = 0;

        TryOpenKeyDoor(&gb, mock_sync_flags, mock_reveal_object, mock_spawn_entity_slot3);

        /* Key count decremented */
        assert(gb_read(&gb, wSmallKeysCount) == 0x02);
        /* Sync flags called */
        assert(g_mock_sync_flags_calls == 1);
        /* Noise sfx set */
        assert(gb_read_hram(&gb, hNoiseSfx) == NOISE_SFX_DOOR_UNLOCKED);
        /* Room status updated with ROOM_STATUS_EVENT_3 (0x40) -> 0x01 | 0x40 = 0x41 */
        assert(gb_read(&gb, (uint16_t)(wIndoorARoomStatus + 0x14)) == 0x41);
        assert(gb_read_hram(&gb, hRoomStatus) == 0x41);
        /* IntersectedObject Left & Top */
        assert(gb_read_hram(&gb, hIntersectedObjectLeft) == 0x50);
        assert(gb_read_hram(&gb, hIntersectedObjectTop) == 0x80);
        /* Reveal object callback invoked */
        assert(g_mock_reveal_object_calls == 1);
        /* Transient VFX Poof coordinates */
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x58);
        assert(gb_read_hram(&gb, hMultiPurpose1) == 0x90);
    }

    /* Test 64: ExecuteDebugWarp (02:54AE) */
    {
        GBState gb;
        gb_init(&gb);
        gb_write(&gb, wDebugWarpIndex, 0);

        ExecuteDebugWarp(&gb);

        assert(gb_read(&gb, wWarp0MapCategory) == 0x01);
        assert(gb_read(&gb, wDebugWarpIndex) == 0x01);
        assert(gb_read(&gb, wWarp0Room) == DebugWarpRooms[0]);
        assert(gb_read(&gb, wWarp0Map) == DebugWarpMaps[0]);
        assert(gb_read(&gb, wWarp0DestinationX) == 0x50);
        assert(gb_read(&gb, wWarp0DestinationY) == 0x70);
        assert(gb_read_hram(&gb, hJingle) == JINGLE_PUZZLE_SOLVED);

        /* Wrap index at 0x0B -> 0 */
        gb_write(&gb, wDebugWarpIndex, 0x0A);
        ExecuteDebugWarp(&gb);
        assert(gb_read(&gb, wDebugWarpIndex) == 0x00);
        assert(gb_read(&gb, wWarp0Room) == DebugWarpRooms[10]);
        assert(gb_read(&gb, wWarp0Map) == DebugWarpMaps[10]);
    }

    /* Test 65: staircaseIsActive (02:552A) */
    {
        GBState gb;

        /* 1. Jumping over staircase (Z != 0) -> no trigger */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionZ, 0x04);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write_hram(&gb, hStaircasePosX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write_hram(&gb, hStaircasePosY, 0x40);
        gb_write_hram(&gb, hStaircase, STAIRCASE_ACTIVE);

        staircaseIsActive(&gb);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_ACTIVE);

        /* 2. Carrying lifted object -> no trigger */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionZ, 0x00);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write_hram(&gb, hStaircasePosX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write_hram(&gb, hStaircasePosY, 0x40);
        gb_write(&gb, wIsCarryingLiftedObject, 1);
        gb_write_hram(&gb, hStaircase, STAIRCASE_ACTIVE);

        staircaseIsActive(&gb);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_ACTIVE);

        /* 3. Outside staircase radius (dx >= 10, e.g. Link=0x50, Staircase=0x40 -> dx = 16 + 5 = 21 >= 10) */
        gb_init(&gb);
        gb_write_hram(&gb, hLinkPositionX, 0x50);
        gb_write_hram(&gb, hStaircasePosX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write_hram(&gb, hStaircasePosY, 0x40);
        gb_write_hram(&gb, hStaircase, STAIRCASE_ACTIVE);

        staircaseIsActive(&gb);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_ACTIVE);

        /* 4. Color Dungeon Entrance closed (room 0x77, outdoors, tombstone != 0x80) */
        gb_init(&gb);
        gb_write_hram(&gb, hMapRoom, ROOM_OW_COLOR_DUNGEON_ENTRANCE);
        gb_write(&gb, wIsIndoor, 0);
        gb_write(&gb, wColorDungonCorrectTombStones, 0x03);
        gb_write_hram(&gb, hLinkPositionX, 0x40);
        gb_write_hram(&gb, hStaircasePosX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x40);
        gb_write_hram(&gb, hStaircasePosY, 0x40);
        gb_write_hram(&gb, hStaircase, STAIRCASE_ACTIVE);

        staircaseIsActive(&gb);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_ACTIVE);

        /* 5. Color Dungeon Entrance open (room 0x77, outdoors, tombstone == 0x80) -> triggers warp and resets staircase */
        gb_write(&gb, wColorDungonCorrectTombStones, 0x80);
        staircaseIsActive(&gb);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_NONE);

        /* 6. Standard staircase trigger (within [-5, +4], e.g. Link=0x3F, Staircase=0x40 -> dx = -1 + 5 = 4 < 10) */
        gb_init(&gb);
        gb_write_hram(&gb, hMapRoom, 0x10);
        gb_write_hram(&gb, hLinkPositionX, 0x3F);
        gb_write_hram(&gb, hStaircasePosX, 0x40);
        gb_write_hram(&gb, hLinkPositionY, 0x42);
        gb_write_hram(&gb, hStaircasePosY, 0x40);
        gb_write_hram(&gb, hStaircase, STAIRCASE_ACTIVE);

        staircaseIsActive(&gb);
        assert(gb_read_hram(&gb, hStaircase) == STAIRCASE_NONE);
    }

}
