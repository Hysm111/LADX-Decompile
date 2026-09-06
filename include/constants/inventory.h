#ifndef LADX_CONSTANTS_INVENTORY_H
#define LADX_CONSTANTS_INVENTORY_H

#include "common.h"

#define INVENTORY_SLOT_COUNT            0x0C

/* Inventory Items */
#define INVENTORY_EMPTY                 0x00
#define INVENTORY_SWORD                 0x01
#define INVENTORY_BOMBS                 0x02
#define INVENTORY_POWER_BRACELET        0x03
#define INVENTORY_SHIELD                0x04
#define INVENTORY_BOW                   0x05
#define INVENTORY_HOOKSHOT              0x06
#define INVENTORY_MAGIC_ROD             0x07
#define INVENTORY_PEGASUS_BOOTS         0x08
#define INVENTORY_OCARINA               0x09
#define INVENTORY_ROCS_FEATHER          0x0A
#define INVENTORY_SHOVEL                0x0B
#define INVENTORY_MAGIC_POWDER          0x0C
#define INVENTORY_BOOMERANG             0x0D

/* Item Usage Context flags */
#define ITEM_USAGE_NEAR_NPC             0x01
#define ITEM_USAGE_READING_TEXT         0x02

/* Attack Step Flags */
#define ATTACK_STEP_ITEM_MAGIC_ROD      0x80

/* Sword Animation States */
#define SWORD_ANIMATION_STATE_SWING_START   0x01
#define SWORD_ANIMATION_STATE_SWING_MIDDLE  0x03
#define SWORD_ANIMATION_STATE_SWING_END     0x04
#define SWORD_ANIMATION_STATE_HOLDING       0x05

#endif /* LADX_CONSTANTS_INVENTORY_H */
