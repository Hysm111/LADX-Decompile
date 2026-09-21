#ifndef LADX_CONSTANTS_ENTITIES_H
#define LADX_CONSTANTS_ENTITIES_H

#define ENTITY_TRADING_ITEM 0xAF
#define BowWowEatableEntitiesTable 0x5218
#define HitboxPositions 0x3AAA
#define BossIntroDialogTable         0x3EDF
#define ENTITY_DESERT_LANMOLA        0x87
#define ENTITY_GRIM_CREEPER          0xBC
#define ENTITY_LEEVER                0x0E
#define ENTITY_OCTOROK               0x09
#define ENTITY_MOBLIN                0x0B
#define ENTITY_MOBLIN_SWORD          0x14
#define ENTITY_ZORA                  0xCB
#define ENTITY_HOPPER                0xF3
#define ENTITY_FLYING_HOPPER_BOMBS   0xF2
#define ENTITY_HARDHIT_BEETLE        0xF9
#define ENTITY_AVALAUNCH             0xF4
#define ENTITY_COLOR_GUARDIAN_BLUE   0xF6
#define ENTITY_COLOR_GUARDIAN_RED    0xF7
#define ENTITY_COLOR_DUNGEON_BOOK    0x4A
#define ENTITY_GIANT_BUZZ_BLOB       0xF8
#define ENTITY_SOUTH_FACE_SHRINE_DOOR 0x64
#define ENTITY_ROTOSWITCH_RED        0xEF
#define ENTITY_ROTOSWITCH_YELLOW     0xF0
#define ENTITY_ROTOSWITCH_BLUE       0xF1
#define ENTITY_COLOR_SHELL_RED       0xE9
#define ENTITY_COLOR_SHELL_GREEN     0xEA
#define ENTITY_COLOR_SHELL_BLUE      0xEB
#define ENTITY_GIBDO                 0x1F
#define ENTITY_WRECKING_BALL         0xA8
#define ENTITY_KEY_DROP_POINT        0x30
#define ENTITY_BOMB                  0x02
#define ENTITY_STALFOS_EVASIVE       0x1E

#define MAX_ENTITIES 16

#define ENTITY_STATUS_DISABLED   0
#define ENTITY_STATUS_DYING      1
#define ENTITY_STATUS_FALLING    2
#define ENTITY_STATUS_BURNING    3
#define ENTITY_STATUS_INIT       4
#define ENTITY_STATUS_ACTIVE     5
#define ENTITY_STATUS_STUNNED    6
#define ENTITY_STATUS_LIFTED     7
#define ENTITY_STATUS_THROWN     8

/* Values for wEntitiesGroundStatus */
#define ENTITY_GROUND_STATUS_NORMAL        0
#define ENTITY_GROUND_STATUS_DEEP_WATER    1
#define ENTITY_GROUND_STATUS_SHALLOW_WATER 2
#define ENTITY_GROUND_STATUS_TALL_GRASS    3

/* Values for wEntitiesOptions1Table. */
#define ENTITY_OPT1_EXCLUDED_FROM_KILL_ALL 0x02
#define ENTITY_OPT1_ALLOW_OUT_OF_BOUNDS    0x20

/* Values for wEntitiesPhysicsFlagsTable. */
#define ENTITY_PHYSICS_HARMLESS  0x80
#define ENTITY_PHYSICS_MASK      0xF0

#define ENTITY_RAFT_RAFT_OWNER   0x6A

#define PhysicsFlagsForEntity      0x4000
#define HitboxFlagsForEntity       0x40FB
#define HealthGroupForEntity       0x41F6
#define InitialHealthForGroup      0x47BC
#define Options1ForEntity          0x42F1

#define EntityHandlersTable      0x4000

#define ENTITY_ARROW                 0x00
#define ENTITY_BOOMERANG             0x01
#define ENTITY_BOMB                  0x02
#define ENTITY_HOOKSHOT_CHAIN        0x03
#define ENTITY_MAGIC_ROD_FIREBALL    0x04
#define ENTITY_LIFTABLE_ROCK         0x05
#define ENTITY_PUSHED_BLOCK          0x06
#define ENTITY_CHEST_WITH_ITEM       0x07
#define ENTITY_MAGIC_POWDER_SPRINKLE 0x08
#define ENTITY_DROPPABLE_HEART       0x2D
#define ENTITY_DROPPABLE_RUPEE       0x2E
#define ENTITY_DROPPABLE_FAIRY       0x2F
#define ENTITY_KEY_DROP_POINT        0x30
#define ENTITY_HEART_PIECE           0x35
#define ENTITY_HIDING_SLIME_KEY      0x3C
#define ENTITY_BOW_WOW               0x6D
#define ENTITY_WRECKING_BALL         0xA8
#define ENTITY_MARIN_AT_THE_SHORE    0xC1
#define ENTITY_GHOST                 0xD4
#define ENTITY_ROOSTER               0xD5
#define ENTITY_SWORD_BEAM            0xDF
#define ENTITY_MUSICAL_NOTE          0xC9
#define ENTITY_MASTER_STALFOS        0x5F
#define ENTITY_TEXT_DEBUGGER         0x6B

#define ENTITY_OPT1_IS_BOSS          0x80
#define ENTITY_OPT1_IS_MINI_BOSS     0x04
#define ENTITY_OPT1_EXCLUDED_FROM_KILL_ALL 0x02

#define ENTITIES_END                 0xFF

#define BANK_OverworldEntitiesPointersTable     0x16
#define OverworldEntitiesPointersTable          0x4000
#define IndoorsAEntitiesPointersTable           0x4200
#define IndoorsBEntitiesPointersTable           0x4400
#define ColorDungeonEntitiesPointersTable       0x4600

#endif /* LADX_CONSTANTS_ENTITIES_H */
