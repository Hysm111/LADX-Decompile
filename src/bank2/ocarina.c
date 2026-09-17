#include "bank2/ocarina.h"
#include "bank2/audio.h"
#include "constants/entities.h"
#include "constants/gameplay.h"
#include "constants/hardware.h"
#include "constants/joypad.h"
#include "constants/link.h"
#include "constants/memory.h"
#include "constants/sfx.h"
#include "home/dialog.h"
#include "home/entities.h"
#include "home/link.h"

const int8_t Data_002_4A12[2] = { 0x08, (int8_t)0xF8 };
const int8_t Data_002_4A14[2] = { 0x06, 0x01 };

void UseOcarina(GBState *gb) {
    if (!gb) return;

    /* ld a, [wLinkPlayingOcarinaCountdown]; ld hl, wIsLinkInTheAir; or [hl];
     * ld hl, wIsUsingHookshot; or [hl]; ret nz */
    uint8_t countdown = gb_read(gb, wLinkPlayingOcarinaCountdown);
    uint8_t in_air = gb_read(gb, wIsLinkInTheAir);
    uint8_t hookshot = gb_read(gb, wIsUsingHookshot);
    if ((countdown | in_air | hookshot) != 0) {
        return;
    }

    /* xor a; ld [wC5A4], a; ld [wC5A5], a */
    gb_write(gb, wC5A4, 0);
    gb_write(gb, wC5A5, 0);

    /* call CopyLinkFinalPositionToPosition */
    CopyLinkFinalPositionToPosition(gb);

    /* ld a, [wOcarinaSongFlags]; and $07; jr nz, .playSong */
    uint8_t song_flags = gb_read(gb, wOcarinaSongFlags) & 0x07;
    if (song_flags == 0) {
        /* wLinkPlayingOcarinaCountdown = $D0; hWaveSfx = WAVE_SFX_OCARINA_NOSONG */
        gb_write(gb, wLinkPlayingOcarinaCountdown, 0xD0);
        gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_NOSONG);
        return;
    }

    /* ld a, [wSelectedSongIndex] */
    uint8_t song_idx = gb_read(gb, wSelectedSongIndex);
    switch (song_idx) {
        case 0:
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0xDC);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_BALLAD);
            break;
        case 1:
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0xD0);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_MAMBO);
            break;
        case 2:
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0xBB);
            gb_write_hram(gb, hWaveSfx, WAVE_SFX_OCARINA_FROG);
            break;
        default:
            break;
    }
}

void LinkPlayingOcarinaHandler(GBState *gb,
                               void (*select_music_track)(GBState *),
                               uint16_t (*spawn_new_entity)(GBState *, uint8_t)) {
    if (!gb) return;

    /* ld a, [wLinkPlayingOcarinaCountdown]; and a; ret z */
    uint8_t countdown = gb_read(gb, wLinkPlayingOcarinaCountdown);
    if (countdown == 0) {
        return;
    }

    /* ld hl, hLinkInteractiveMotionBlocked; ld [hl], $02 */
    gb_write_hram(gb, hLinkInteractiveMotionBlocked, 0x02);

    /* cp $FF; jr nz, jr_002_4A7C */
    if (countdown == 0xFF) {
        /* ld a, [wD210]; add $01; ld [wD210], a; ld a, [wD211]; adc $00; ld [wD211], a */
        uint16_t d210_val = (uint16_t)(gb_read(gb, wD210) | (gb_read(gb, wD211) << 8));
        d210_val++;
        gb_write(gb, wD210, (uint8_t)(d210_val & 0xFF));
        gb_write(gb, wD211, (uint8_t)(d210_val >> 8));

        /* cp $08; jr nz, .jr_4A53; ld a, [wD210]; cp $D0; jr nz, .jr_4A53 */
        if (d210_val == 0x08D0) {
            /* xor a; ld [wLinkPlayingOcarinaCountdown], a; ld [wC167], a */
            gb_write(gb, wLinkPlayingOcarinaCountdown, 0x00);
            gb_write(gb, wC167, 0x00);
            /* ld a, $03; ld [wC5A3], a */
            gb_write(gb, wC5A3, 0x03);

            /* ld a, [wD465]; cp $47; ret z */
            if (gb_read(gb, wD465) == 0x47) {
                return;
            }

            /* jr jr_002_4A6C */
            uint8_t slot = gb_read(gb, wD461);
            gb_write(gb, (uint16_t)(wEntitiesStateTable + slot), 0x00);
            if (select_music_track) {
                select_music_track(gb);
            } else {
                SelectMusicTrackAfterTransition(gb);
            }
            return;
        }

        /* .jr_4A53: ld a, [wD465]; cp $47; jr z, jr_002_4A7A */
        if (gb_read(gb, wD465) != 0x47) {
            /* ldh a, [hJoypadState]; and J_A | J_B; jr z, jr_002_4A7A */
            if ((gb_read_hram(gb, hJoypadState) & (J_A | J_B)) != 0) {
                /* xor a; ld [wLinkPlayingOcarinaCountdown], a; ld [wC167], a */
                gb_write(gb, wLinkPlayingOcarinaCountdown, 0x00);
                gb_write(gb, wC167, 0x00);
                /* ld a, $03; ld [wC5A3], a */
                gb_write(gb, wC5A3, 0x03);

                /* jr_002_4A6C */
                uint8_t slot = gb_read(gb, wD461);
                gb_write(gb, (uint16_t)(wEntitiesStateTable + slot), 0x00);
                if (select_music_track) {
                    select_music_track(gb);
                } else {
                    SelectMusicTrackAfterTransition(gb);
                }
                return;
            }
        }
        /* jr_002_4A7A -> jr jr_002_4AD1 */
    } else {
        /* jr_002_4A7C */
        ClearLinkPositionIncrement(gb);
        ResetSpinAttack(gb);

        /* dec [hl]; jr nz, jr_002_4AD1 */
        countdown--;
        gb_write(gb, wLinkPlayingOcarinaCountdown, countdown);
        if (countdown == 0) {
            /* ld a, [wIsMarinFollowingLink]; and a; jr z, .jr_4AA2 */
            if (gb_read(gb, wIsMarinFollowingLink) != 0) {
                /* ld a, [wSelectedSongIndex]; cp $01; jr z, jr_002_4AB2 */
                /* ld a, [wIsIndoor]; and a; jr nz, jr_002_4AB2 */
                if (gb_read(gb, wSelectedSongIndex) != 0x01 && gb_read(gb, wIsIndoor) == 0) {
                    /* call_open_dialog Dialog277 */
                    OpenDialogInTable2(gb, 0x77);
                }
            } else {
                /* .jr_4AA2: ld a, [wOcarinaSongFlags]; and a; jr nz, jr_002_4AB2 */
                if (gb_read(gb, wOcarinaSongFlags) == 0) {
                    /* call_open_dialog Dialog08E */
                    OpenDialogInTable0(gb, 0x8E);
                    gb_write(gb, wC167, 0x00);
                    return;
                }
            }

            /* jr_002_4AB2 */
            gb_write(gb, wC167, 0x00);
            /* ld a, [wSelectedSongIndex]; cp $01; jr nz, .ret_4AD0 */
            if (gb_read(gb, wSelectedSongIndex) == 0x01) {
                /* ld a, TRANSITION_GFX_MANBO_IN; ld [wTransitionGfx], a */
                gb_write(gb, wTransitionGfx, TRANSITION_GFX_MANBO_IN);
                /* xor a; ld [wTransitionGfxFrameCount], a; ld [wTransitionSequenceCounter], a; ld [wC16C], a */
                gb_write(gb, wTransitionGfxFrameCount, 0x00);
                gb_write(gb, wTransitionSequenceCounter, 0x00);
                gb_write(gb, wC16C, 0x00);
                /* ld a, JINGLE_MANBO_WARP; ldh [hJingle], a */
                gb_write_hram(gb, hJingle, JINGLE_MANBO_WARP);
            }
            return;
        }
    }

    /* jr_002_4AD1 */
    uint8_t c5a4 = (uint8_t)(gb_read(gb, wC5A4) + 1);
    gb_write(gb, wC5A4, c5a4);
    if (c5a4 >= 0x38) {
        gb_write(gb, wC5A4, 0x00);
        uint8_t c5a5 = (uint8_t)(gb_read(gb, wC5A5) ^ 0x01);
        gb_write(gb, wC5A5, c5a5);
    }

    /* .jr_4AE8 */
    uint8_t c5a5 = gb_read(gb, wC5A5);
    uint8_t anim_state = (c5a5 != 0) ? LINK_ANIMATION_STATE_UNKNOWN_75 : (uint8_t)(LINK_ANIMATION_STATE_UNKNOWN_75 + 1);
    gb_write_hram(gb, hLinkAnimationState, anim_state);

    /* ld a, $02; ld [wC167], a; ld [wC111], a */
    gb_write(gb, wC167, 0x02);
    gb_write(gb, wC111, 0x02);

    /* ld a, [wLinkPlayingOcarinaCountdown]; cp $10; ret c */
    countdown = gb_read(gb, wLinkPlayingOcarinaCountdown);
    if (countdown < 0x10) {
        return;
    }

    /* ld a, [wC5A4]; cp $14; jr nz, .ret_4B40 */
    if (gb_read(gb, wC5A4) != 0x14) {
        return;
    }

    /* ld a, ENTITY_MUSICAL_NOTE; call SpawnNewEntity_trampoline; jr c, .ret_4B40 */
    uint16_t slot = SpawnNewEntity_trampoline(gb, ENTITY_MUSICAL_NOTE, spawn_new_entity);
    if (slot == 0xFFFF) {
        return;
    }

    /* ldh a, [hLinkPositionY]; sub $08; ld [wEntitiesPosYTable + de], a */
    uint8_t note_y = (uint8_t)(gb_read_hram(gb, hLinkPositionY) - 0x08);
    gb_write(gb, (uint16_t)(wEntitiesPosYTable + slot), note_y);

    /* ld a, [wC5A5]; ld c, a; ld b, d; ld hl, Data_002_4A12; add hl, bc */
    /* ldh a, [hLinkPositionX]; add [hl]; ld [wEntitiesPosXTable + de], a */
    uint8_t bc_idx = (uint8_t)(gb_read(gb, wC5A5) & 1);
    uint8_t note_x = (uint8_t)(gb_read_hram(gb, hLinkPositionX) + (uint8_t)Data_002_4A12[bc_idx]);
    gb_write(gb, (uint16_t)(wEntitiesPosXTable + slot), note_x);

    /* ld hl, Data_002_4A14; add hl, bc; ld a, [hl]; ld [wEntitiesSpeedXTable + de], a */
    gb_write(gb, (uint16_t)(wEntitiesSpeedXTable + slot), (uint8_t)Data_002_4A14[bc_idx]);

    /* ld [wEntitiesSpeedYTable + de], $FC */
    gb_write(gb, (uint16_t)(wEntitiesSpeedYTable + slot), 0xFC);

    /* ld [wEntitiesInertiaTable + de], $40 */
    gb_write(gb, (uint16_t)(wEntitiesInertiaTable + slot), 0x40);
}
