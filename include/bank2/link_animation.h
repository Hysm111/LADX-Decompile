#ifndef LADX_BANK2_LINK_ANIMATION_H
#define LADX_BANK2_LINK_ANIMATION_H

#include "gb.h"

extern const uint8_t DirectionToLinkAnimationState[4];
/**
 * If Link is carrying a lifted object (wIsCarryingLiftedObject >= 2), sets his animation state
 * and blocks interactive motion. (02:4338)
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_4338(GBState *gb);

/**
 * Decrements attack step animation countdown and updates Link animation state from facing direction. (02:434A)
 *
 * @param gb Pointer to Game Boy system state.
 */
void func_002_434A(GBState *gb);

/* Bank 2 Sword & Spin Attack Tables */
extern const uint8_t LinkDirectionToStaticSwordCollitionCheckNeeded[24];
extern const uint8_t LinkDirectionTo_wC141[24];
extern const uint8_t LinkDirectionToOffset[24];
extern const uint8_t LinkDirectionTo_wC143[24];
extern const uint8_t LinkDirectionToSwordDirection[24];
extern const uint8_t LinkDirectionToLinkAnimationState1[24];
extern const int8_t LinkDirectionTo_wC13A[24];
extern const int8_t LinkDirectionTo_wC139[24];
extern const int8_t LinkDirectionTo_wC13C[24];
extern const int8_t LinkDirectionTo_wC13B[24];
extern const uint8_t SwordAnimationStateToUnknow[8];
extern const uint8_t UnkownToLinkStateTable[16];
extern const uint8_t FrameCounterToLinkDirection[4];
extern const uint8_t LinkDirectionToSwordAnimationState[32];
extern const uint8_t LinkDirectionToAbsolute[32];

/**
 * Resets sword animation state and spin attack variables (02:48B0).
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_48B0(GBState *gb);

/**
 * Updates sword direction, link animation state, and computes sword collision box (02:4827).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void label_002_4827(GBState *gb, void (*check_collision)(GBState *));

/**
 * Progresses sword swing animation from wC16D timer (02:476B).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void label_002_476B(GBState *gb, void (*check_collision)(GBState *));

/**
 * Updates spin attack 360-degree rotation animation and collision box (02:4709).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void UpdateSpinAttackAnimation(GBState *gb, void (*check_collision)(GBState *));

/**
 * Updates Link automatic animations: whirlpool rotation, jump frames, sword/spin attack (02:478C).
 *
 * @param gb Pointer to Game Boy system state.
 * @param check_collision Optional callback for static sword collision check.
 */
void UpdateLinkAnimation(GBState *gb, void (*check_collision)(GBState *));

/* Bank 2 Walking & Movement Tables */
extern const uint8_t LinkAnimationsList_WalkingNoShield[8];
extern const uint8_t LinkAnimationsList_WalkCarryingDefaultShield[8];
extern const uint8_t LinkAnimationsList_WalkUsingDefaultShield[8];
extern const uint8_t LinkAnimationsList_WalkCarryingMirrorShield[8];
extern const uint8_t LinkAnimationsList_WalkUsingMirrorShield[8];
extern const uint8_t LinkAnimationsList_PushingObject[8];
extern const uint8_t LinkAnimationsList_LiftingObject[8];
extern const uint8_t Data_002_4948[8];
extern const uint8_t Data_002_4950[8];
extern const uint8_t LinkAnimationsList_WalkSideScrolling[8];
extern const uint8_t Data_002_49CA[72];

#endif /* LADX_BANK2_LINK_ANIMATION_H */
