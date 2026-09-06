#ifndef LADX_CONSTANTS_ROOMS_H
#define LADX_CONSTANTS_ROOMS_H

/* Overworld and Indoor Room IDs */
#define ROOM_OW_RIGHT_OF_EGG         0x07
#define ROOM_OW_CAMERA_SHOP          0x37
#define ROOM_OW_SIREN                0xC9
#define ROOM_OW_WALRUS               0xFD
#define ROOM_INDOOR_B_CAMERA_SHOP    0xB5

#define ROOM_OW_EAGLES_TOWER         0x0E
#define UNKNOWN_ROOM_0C              0x0C
#define UNKNOWN_ROOM_1B              0x1B
#define ROOM_OW_GIANT_SKULL          0x97
#define ROOM_SECTION_OW_SECOND_HALF  0x80

/* Overworld Room Banks */
#define BANK_OverworldRoomsFirstHalf  0x09
#define BANK_OverworldRoomsSecondHalf 0x1A

/* Room Template & World Map Loader Addresses */
#define BANK_LoadRoomTemplate        0x14
#define LoadRoomTemplate             0x4880
#define BANK_LoadWorldMapBGMap       0x20
#define LoadWorldMapBGMap            0x588B

/* Room Status Door Open Flags */
#define ROOM_STATUS_DOOR_OPEN_RIGHT  0x01
#define ROOM_STATUS_DOOR_OPEN_LEFT   0x02
#define ROOM_STATUS_DOOR_OPEN_UP     0x04
#define ROOM_STATUS_DOOR_OPEN_DOWN   0x08

/* Door Types */
#define DOOR_TYPE_KEY_TOP            0x00
#define DOOR_TYPE_KEY_BOTTOM         0x01
#define DOOR_TYPE_KEY_LEFT           0x02
#define DOOR_TYPE_KEY_RIGHT          0x03
#define DOOR_TYPE_SHUTTER_TOP        0x04
#define DOOR_TYPE_SHUTTER_BOTTOM     0x05
#define DOOR_TYPE_SHUTTER_LEFT       0x06
#define DOOR_TYPE_SHUTTER_RIGHT      0x07
#define DOOR_TYPE_BOSS_TOP           0x08

/* Shutter Door Mask Bits */
#define DOOR_TYPE_SHUTTER_TOP_BIT    0x01
#define DOOR_TYPE_SHUTTER_BOTTOM_BIT 0x02
#define DOOR_TYPE_SHUTTER_LEFT_BIT   0x04
#define DOOR_TYPE_SHUTTER_RIGHT_BIT  0x08

/* Additional Room Constants */
#define ROOM_INDOOR_B_KANALET_MAIN_ENTRANCE 0xD3
#define ROOM_OW_RIGHT_OF_EGG         0x07
#define UNKNOWN_ROOM_4A              0x4A
#define UNKNOWN_ROOM_75              0x75
#define UNKNOWN_ROOM_AA              0xAA
#define UNKNOWN_ROOM_C4              0xC4

/* Bank 0 Macro Tables ROM Addresses */
#define KeyDoorTopObjectIds_Addr             0x35F8
#define KeyDoorBottomObjectIds_Addr          0x3613
#define KeyDoorLeftObjectIds_Addr            0x362E
#define KeyDoorRightObjectIds_Addr           0x3649
#define OpenDoorTopObjectIds_Addr            0x36B0
#define OpenDoorBottomObjectIds_Addr         0x36E8
#define OpenDoorLeftObjectIds_Addr           0x36FC
#define OpenDoorRightObjectIds_Addr          0x3710
#define BossDoorObjectIds_Addr               0x3724
#define StairsDoorObjectIds_Addr             0x375C
#define RevolvingDoorObjectIds_Addr          0x376B
#define OneWayArrowObjectIds_Addr            0x377A
#define DungeonEntranceObjectOffsets_Addr    0x3789
#define DungeonEntranceObjectIds_Addr        0x3796
#define EntranceObjectIds_Addr               0x37B4
#define HorizontalObjectOffsets_Addr         0x37E1
#define VerticalObjectOffsets_Addr           0x37E4

/* Overworld Room Status Flags */
#define OW_ROOM_STATUS_OWL_TALKED    0x20

/* Static Object Physics Flags ROM tables in Bank $08 */
#define BANK_ObjectPhysicFlags       0x08
#define OverworldObjectPhysicFlags   0x4AD4
#define Indoors1ObjectPhysicFlags    0x4BD4

#endif /* LADX_CONSTANTS_ROOMS_H */
