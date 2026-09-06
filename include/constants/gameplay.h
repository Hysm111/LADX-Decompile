#ifndef LADX_CONSTANTS_GAMEPLAY_H
#define LADX_CONSTANTS_GAMEPLAY_H

#include "common.h"

#define NAME_LENGTH                     5

/* Values for wGameplayType */
#define GAMEPLAY_INTRO                  0x00
#define GAMEPLAY_CREDITS                0x01
#define GAMEPLAY_FILE_SELECT            0x02
#define GAMEPLAY_FILE_NEW               0x03
#define GAMEPLAY_FILE_DELETE            0x04
#define GAMEPLAY_FILE_COPY              0x05
#define GAMEPLAY_FILE_SAVE              0x06
#define GAMEPLAY_WORLD_MAP              0x07
#define GAMEPLAY_CUTSCENE               0x08
#define GAMEPLAY_MARIN_BEACH            0x09
#define GAMEPLAY_WF_MURAL               0x0A
#define GAMEPLAY_WORLD                  0x0B
#define GAMEPLAY_INVENTORY              0x0C
#define GAMEPLAY_PHOTO_ALBUM            0x0D
#define GAMEPLAY_PHOTO_DIZZY_LINK       0x0E
#define GAMEPLAY_PHOTO_NICE_LINK        0x0F
#define GAMEPLAY_PHOTO_MARIN_CLIFF      0x10
#define GAMEPLAY_PHOTO_MARIN_WELL       0x11
#define GAMEPLAY_PHOTO_MABE             0x12
#define GAMEPLAY_PHOTO_ULRIRA           0x13
#define GAMEPLAY_PHOTO_BOW_WOW          0x14
#define GAMEPLAY_PHOTO_THIEF            0x15
#define GAMEPLAY_PHOTO_FISHERMAN        0x16
#define GAMEPLAY_PHOTO_ZORA             0x17
#define GAMEPLAY_PHOTO_KANALET          0x18
#define GAMEPLAY_PHOTO_GHOST            0x19
#define GAMEPLAY_PHOTO_BRIDGE           0x1A

/* Values for wGameplaySubtype */
#define GAMEPLAY_INTRO_SEA              0x03
#define GAMEPLAY_INTRO_LINK_FACE        0x04
#define GAMEPLAY_WORLD_INTERACTIVE      0x07

/* Values for wLinkMotionState */
#define LINK_MOTION_DEFAULT             0x00
#define LINK_MOTION_SWIMMING            0x01
#define LINK_MOTION_UNSTUCKING          0x02
#define LINK_MOTION_MAP_FADE_OUT        0x03
#define LINK_MOTION_MAP_FADE_IN         0x04
#define LINK_MOTION_REVOLVING_DOOR      0x05
#define LINK_MOTION_FALLING_DOWN        0x06
#define LINK_MOTION_PASS_OUT            0x07
#define LINK_MOTION_RECOVER             0x08
#define LINK_MOTION_TELEPORT            0x09

#endif /* LADX_CONSTANTS_GAMEPLAY_H */
