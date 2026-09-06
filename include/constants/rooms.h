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

/* Overworld Room Status Flags */
#define OW_ROOM_STATUS_OWL_TALKED    0x20

/* Static Object Physics Flags ROM tables in Bank $08 */
#define BANK_ObjectPhysicFlags       0x08
#define OverworldObjectPhysicFlags   0x4AD4
#define Indoors1ObjectPhysicFlags    0x4BD4

#endif /* LADX_CONSTANTS_ROOMS_H */
