#include "test_bank2.h"
#include "test_support.h"

#include "gb.h"
#include "bank2/room_events.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/maps.h"
#include "constants/memory.h"
#include "constants/rooms.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void test_bank2_room_events(void) {
    /* Test 76: ExecuteRoomEvents (02:593B) */
    {
        GBState gb;
        gb_init(&gb);

        /* Verify table sizes */
        assert(sizeof(ShutterDoorsMaskTable) == 4);
        assert(sizeof(OpeningDoorTileIds) == 72);
        assert(sizeof(DoorXOffsets) == 18);
        assert(sizeof(DoorYOffsets) == 18);
        assert(sizeof(OpeningDoorTileOffsets) == 16);
        assert(sizeof(OpenDoorObjectIdsTable) == 16);
        assert(sizeof(DoorToOpenStatusFlagTable) == 9);
        assert(sizeof(DoorToAdjacentRoomTable) == 9);
        assert(sizeof(DoorToAdjacentOpenStatusFlagTable) == 9);
        assert(sizeof(ClosingDoorTileIds) == 32);
        assert(sizeof(Data_002_5BE4) == 16);
        assert(sizeof(ClosingDoorTileOffsets) == 8);
        assert(sizeof(ClosedShutterDoorObjectIdsTable) == 8);

        /* 1. Guard checks: wDialogState, wRoomTransitionState, wInventoryAppearing, wIsIndoor */
        g_mock_room_triggers_calls = 0;
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wDialogState, 1);
        ExecuteRoomEvents(&gb, mock_room_triggers, NULL, NULL);
        assert(g_mock_room_triggers_calls == 0);

        gb_write(&gb, wDialogState, 0);
        gb_write(&gb, wRoomTransitionState, 1);
        ExecuteRoomEvents(&gb, mock_room_triggers, NULL, NULL);
        assert(g_mock_room_triggers_calls == 0);

        gb_write(&gb, wRoomTransitionState, 0);
        gb_write(&gb, wInventoryAppearing, 1);
        ExecuteRoomEvents(&gb, mock_room_triggers, NULL, NULL);
        assert(g_mock_room_triggers_calls == 0);

        gb_write(&gb, wInventoryAppearing, 0);
        gb_write(&gb, wIsIndoor, 0);
        ExecuteRoomEvents(&gb, mock_room_triggers, NULL, NULL);
        assert(g_mock_room_triggers_calls == 0);

        /* 2. Normal execution triggers room triggers and effects */
        gb_write(&gb, wIsIndoor, 1);
        ExecuteRoomEvents(&gb, mock_room_triggers, NULL, NULL);
        assert(g_mock_room_triggers_calls == 1);

        /* 3. Door opening in progress (wDoorsOpeningOrClosing == 1) blocks Link motion */
        gb_init(&gb);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wDoorsOpeningOrClosing, 1);
        gb_write(&gb, wDoorEvent, 0);
        ExecuteRoomEvents(&gb, NULL, NULL, mock_get_adjacent_room_status_addr);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
        assert(gb_read(&gb, wDBAC) == 1);

        /* 4. Door closing in progress (wDoorsOpeningOrClosing == 2) blocks Link motion */
        gb_init(&gb);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wDoorsOpeningOrClosing, 2);
        gb_write(&gb, wDoorEvent, 4);
        ExecuteRoomEvents(&gb, NULL, NULL, NULL);
        assert(gb_read_hram(&gb, hLinkInteractiveMotionBlocked) == 1);
        assert(gb_read(&gb, wDBAC) == 1);

        /* 5. Enqueue doors opening (wEnqueueDoorsOpening == 1) picks first set bit */
        gb_init(&gb);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wDoorsOpeningOrClosing, 0);
        gb_write(&gb, wEnqueueDoorsOpening, 1);
        gb_write(&gb, wShutterDoorsMask, 0x05); /* bits 0 and 2 set (doors 4 and 6) */
        ExecuteRoomEvents(&gb, NULL, NULL, NULL);
        assert(gb_read(&gb, wDoorEvent) == 4);
        assert(gb_read(&gb, wDoorsOpeningOrClosing) == 1);
        assert(gb_read(&gb, wDBAC) == 0);
        assert(gb_read(&gb, wShutterDoorsMask) == 0x04); /* bit 0 cleared */
        assert(gb_read(&gb, wEnqueueDoorsOpening) == 1);

        /* Reset doors opening state to trigger next enqueue */
        gb_write(&gb, wDoorsOpeningOrClosing, 0);
        ExecuteRoomEvents(&gb, NULL, NULL, NULL);
        assert(gb_read(&gb, wDoorEvent) == 6);
        assert(gb_read(&gb, wDoorsOpeningOrClosing) == 1);
        assert(gb_read(&gb, wShutterDoorsMask) == 0x00); /* bit 2 cleared */

        /* When mask is empty, clears wEnqueueDoorsOpening */
        gb_write(&gb, wDoorsOpeningOrClosing, 0);
        ExecuteRoomEvents(&gb, NULL, NULL, NULL);
        assert(gb_read(&gb, wEnqueueDoorsOpening) == 0);

        /* 6. Enqueue doors closing (wEnqueueDoorsClosing == 1) picks first set bit */
        gb_init(&gb);
        gb_write(&gb, wIsIndoor, 1);
        gb_write(&gb, wDoorsOpeningOrClosing, 0);
        gb_write(&gb, wEnqueueDoorsClosing, 1);
        gb_write(&gb, wShutterDoorsMask2, 0x02); /* bit 1 set (door 5) */
        ExecuteRoomEvents(&gb, NULL, NULL, NULL);
        assert(gb_read(&gb, wDoorEvent) == 5);
        assert(gb_read(&gb, wDoorsOpeningOrClosing) == 2);
        assert(gb_read(&gb, wDBAC) == 0);
        assert(gb_read(&gb, wShutterDoorsMask2) == 0x00); /* bit 1 cleared */

        /* When mask is empty, clears wEnqueueDoorsClosing */
        gb_write(&gb, wDoorsOpeningOrClosing, 0);
        ExecuteRoomEvents(&gb, NULL, NULL, NULL);
        assert(gb_read(&gb, wEnqueueDoorsClosing) == 0);
    }

    /* Test 77: DoorOpening (02:5A7B) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Initial phase (half-open door animation frame) */
        gb_write(&gb, wDoorEvent, 0); /* DOOR_TYPE_KEY_TOP */
        gb_write(&gb, (uint16_t)(wDoorXPositions + 0), 0x38);
        gb_write(&gb, (uint16_t)(wDoorYPositions + 0), 0x10);
        gb_write(&gb, wDBAC, 0);
        gb_write(&gb, wDrawCommandsSize, 0);

        DoorOpening(&gb, NULL, mock_get_adjacent_room_status_addr);
        assert(gb_read(&gb, wDBAC) == 1);
        assert(gb_read(&gb, wDrawCommandsSize) == 10);
        /* Command 0: 5 bytes */
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 2)) == 0x01);
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 3)) == OpeningDoorTileIds[0]);
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 4)) == OpeningDoorTileIds[1]);
        /* Command 1: 5 bytes + terminator 0 */
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 7)) == 0x01);
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 8)) == OpeningDoorTileIds[2]);
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 9)) == OpeningDoorTileIds[3]);
        assert(gb_read(&gb, (uint16_t)(wDrawCommand + 10)) == 0x00);

        /* 2. Completion phase (fully open door tiles and object/status updates) */
        gb_init(&gb);
        gb_write(&gb, wDoorEvent, 0); /* DOOR_TYPE_KEY_TOP */
        gb_write(&gb, (uint16_t)(wDoorPositions + 0), 0x14);
        gb_write(&gb, wDBAC, 7);
        gb_write(&gb, wDoorsOpeningOrClosing, 1);
        gb_write(&gb, wC1A8, 1);
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapRoom, 0x2A);
        gb_write(&gb, wIndoorRoom, 0x2A);

        DoorOpening(&gb, NULL, mock_get_adjacent_room_status_addr);
        assert(gb_read(&gb, wDoorsOpeningOrClosing) == 0);
        assert(gb_read(&gb, wC1A8) == 0);
        assert(gb_read_hram(&gb, hMultiPurposeE) == 0);
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x14);

        /* Check wRoomObjects replacement */
        assert(gb_read(&gb, (uint16_t)(wRoomObjects + 0x14 + OpeningDoorTileOffsets[0])) == OpenDoorObjectIdsTable[0]);
        assert(gb_read(&gb, (uint16_t)(wRoomObjects + 0x14 + OpeningDoorTileOffsets[8])) == OpenDoorObjectIdsTable[8]);

        /* Check room status updated */
        uint8_t cur_status = gb_read(&gb, (uint16_t)(wIndoorARoomStatus + 0x2A));
        assert(cur_status & ROOM_STATUS_DOOR_OPEN_UP);
        assert(gb_read_hram(&gb, hRoomStatus) == cur_status);

        /* Check adjacent room status updated (room 0x2A - 8 = 0x22, flag DOOR_OPEN_DOWN) */
        uint8_t adj_status = gb_read(&gb, (uint16_t)(wIndoorARoomStatus + 0x22));
        assert(adj_status & ROOM_STATUS_DOOR_OPEN_DOWN);

        /* 3. Boss door 8 maps to key door 0 for object replacement */
        gb_init(&gb);
        gb_write(&gb, wDoorEvent, 8); /* DOOR_TYPE_BOSS_TOP */
        gb_write(&gb, (uint16_t)(wDoorPositions + 8), 0x30);
        gb_write(&gb, wDBAC, 7);
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapRoom, 0x15);
        gb_write(&gb, wIndoorRoom, 0x15);

        DoorOpening(&gb, NULL, mock_get_adjacent_room_status_addr);
        assert(gb_read(&gb, (uint16_t)(wRoomObjects + 0x30 + OpeningDoorTileOffsets[0])) == OpenDoorObjectIdsTable[0]);
        assert(gb_read(&gb, (uint16_t)(wRoomObjects + 0x30 + OpeningDoorTileOffsets[8])) == OpenDoorObjectIdsTable[8]);
    }

    /* Test 78: DoorClosing (02:5C04) */
    {
        GBState gb;
        gb_init(&gb);

        /* 1. Displacement guard: Link entry point within 0x20 of door is moved to Link pos */
        gb_write(&gb, wDoorEvent, 4); /* DOOR_TYPE_SHUTTER_TOP */
        gb_write(&gb, (uint16_t)(wDoorXPositions + 4), 0x40);
        gb_write(&gb, (uint16_t)(wDoorYPositions + 4), 0x10);
        /* left = 0x08 + 0x40 = 0x48. top0 = 0x00 + 0x10 = 0x10. */
        gb_write(&gb, wLinkMapEntryPositionX, 0x48);
        gb_write(&gb, wLinkMapEntryPositionY, 0x10);
        gb_write_hram(&gb, hLinkPositionX, 0x65);
        gb_write_hram(&gb, hLinkPositionY, 0x75);
        gb_write(&gb, wDBAC, 0);

        DoorClosing(&gb, NULL);
        assert(gb_read(&gb, wLinkMapEntryPositionX) == 0x65);
        assert(gb_read(&gb, wLinkMapEntryPositionY) == 0x75);
        assert(gb_read(&gb, wDBAC) == 1);

        /* 2. Completion phase: updates room objects and clears status flag */
        gb_init(&gb);
        gb_write(&gb, wDoorEvent, 4); /* DOOR_TYPE_SHUTTER_TOP */
        gb_write(&gb, (uint16_t)(wDoorPositions + 4), 0x20);
        gb_write(&gb, wDBAC, 7);
        gb_write(&gb, wDoorsOpeningOrClosing, 2);
        gb_write(&gb, wC1A8, 1);
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapRoom, 0x35);
        gb_write(&gb, (uint16_t)(wIndoorARoomStatus + 0x35), 0xFF);

        DoorClosing(&gb, NULL);
        assert(gb_read(&gb, wDoorsOpeningOrClosing) == 0);
        assert(gb_read(&gb, wC1A8) == 0);
        assert(gb_read_hram(&gb, hMultiPurposeE) == 0);
        assert(gb_read_hram(&gb, hMultiPurpose0) == 0x20);

        /* Check wRoomObjects replacement */
        assert(gb_read(&gb, (uint16_t)(wRoomObjects + 0x20 + ClosingDoorTileOffsets[0])) == ClosedShutterDoorObjectIdsTable[0]);
        assert(gb_read(&gb, (uint16_t)(wRoomObjects + 0x20 + ClosingDoorTileOffsets[4])) == ClosedShutterDoorObjectIdsTable[4]);

        /* Check room status cleared DOOR_OPEN_UP bit */
        uint8_t closed_status = gb_read(&gb, (uint16_t)(wIndoorARoomStatus + 0x35));
        assert(closed_status == (uint8_t)(0xFF & ~ROOM_STATUS_DOOR_OPEN_UP));
        assert(gb_read_hram(&gb, hRoomStatus) == closed_status);

        /* 3. Status address branches: Overworld and Color Dungeon */
        /* Overworld */
        gb_init(&gb);
        gb_write(&gb, wDoorEvent, 5); /* DOOR_TYPE_SHUTTER_BOTTOM */
        gb_write(&gb, (uint16_t)(wDoorPositions + 5), 0x10);
        gb_write(&gb, wDBAC, 7);
        gb_write(&gb, wIsIndoor, 0);
        gb_write_hram(&gb, hMapRoom, 0x12);
        gb_write(&gb, (uint16_t)(wOverworldRoomStatus + 0x12), 0xFF);
        DoorClosing(&gb, NULL);
        assert(gb_read(&gb, (uint16_t)(wOverworldRoomStatus + 0x12)) == (uint8_t)(0xFF & ~ROOM_STATUS_DOOR_OPEN_DOWN));

        /* Color Dungeon */
        gb_init(&gb);
        gb_write(&gb, wDoorEvent, 6); /* DOOR_TYPE_SHUTTER_LEFT */
        gb_write(&gb, (uint16_t)(wDoorPositions + 6), 0x10);
        gb_write(&gb, wDBAC, 7);
        gb_write(&gb, wIsIndoor, 1);
        gb_write_hram(&gb, hMapId, MAP_COLOR_DUNGEON);
        gb_write_hram(&gb, hMapRoom, 0x05);
        gb_write(&gb, (uint16_t)(wColorDungeonRoomStatus + 0x05), 0xFF);
        DoorClosing(&gb, NULL);
        assert(gb_read(&gb, (uint16_t)(wColorDungeonRoomStatus + 0x05)) == (uint8_t)(0xFF & ~ROOM_STATUS_DOOR_OPEN_LEFT));
    }

}
