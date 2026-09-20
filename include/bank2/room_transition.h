#ifndef LADX_BANK2_ROOM_TRANSITION_H
#define LADX_BANK2_ROOM_TRANSITION_H

#include "gb.h"

#include "constants/directions.h"
#include "constants/gameplay.h"
#include "constants/maps.h"

/* Bank 2 Debug Warp Tables (02:5471+) */
extern const uint8_t DebugWarpRooms[11];
extern const uint8_t DebugWarpMaps[11];

/* Room Transition Data Tables (02:78D8+) */
extern const int8_t RoomTransitionLinkXIncrement[4];
extern const int8_t RoomTransitionLinkYIncrement[4];
extern const int8_t RoomTransitionXIncrement[4];
extern const int8_t RoomTransitionYIncrement[4];
extern const uint8_t WindFishEggMazeSequence[32];
extern const uint8_t OverworldRoomIncrement[4];
extern const uint8_t IndoorRoomIncrement[4];
extern const uint8_t RoomTransitionBGOriginHigh[4];
extern const uint8_t RoomTransitionBGOriginLow[4];
extern const uint8_t RoomTransitionBGInitialUpdateRegionHigh[4];
extern const uint8_t RoomTransitionBGInitialUpdateRegionLow[4];
extern const uint8_t RoomUpdateTileAmount[4];
extern const uint8_t RoomTransitionFramesToMidScreen[4];
extern const uint8_t RoomTransitionOffset[8];
extern const uint8_t RoomTransitionTargetScrollX[4];
extern const uint8_t RoomTransitionTargetScrollY[4];
extern const int8_t Data_002_7C04[8];
extern const int8_t Data_002_7C0C[8];
extern const int8_t Data_002_7C40[8];
extern const int8_t Data_002_7C48[8];

/**
 * Triggers debug warp sequence by incrementing debug warp index and fading out (02:54AE).
 *
 * @param gb Pointer to Game Boy system state.
 */
void ExecuteDebugWarp(GBState *gb);

/**
 * Checks Link proximity, jump state, lifted object, and color dungeon conditions to trigger staircase warp (02:552A).
 *
 * @param gb Pointer to Game Boy system state.
 */
void staircaseIsActive(GBState *gb);

/**
 * Iterates through transient VFX slots (15 down to 0) and processes active/inactive staircase state (02:54E4).
 *
 * @param gb Pointer to Game Boy system state.
 * @param render_transient_vfx Optional callback to render transient VFX in slot.
 */
void renderTranscientVFXs(GBState *gb, void (*render_transient_vfx)(GBState *, uint8_t));

/**
 * Clears indoor room status, decrements dialog/photo album cooldowns, and updates VFX/staircase (02:5487).
 *
 * @param gb Pointer to Game Boy system state.
 * @param render_transient_vfx Optional callback to render transient VFX in slot.
 */
void label_002_5487(GBState *gb, void (*render_transient_vfx)(GBState *, uint8_t));

/**
 * Loads the minimap for the current dungeon (02:6709).
 *
 * @param gb Pointer to Game Boy system state.
 */
void LoadMinimap(GBState *gb);

/**
 * Applies room transition state machine (02:78E8-02:79D9).
 * Handles sliding transitions between rooms of the same map.
 *
 * @param gb Pointer to Game Boy system state.
 * @param bg_collision_handler Optional callback for background collision handling.
 * @param create_following_npc Optional callback for creating following NPC.
 * @param set_world_music_track Optional callback for setting world music track.
 * @param clear_link_position_increment Optional callback for clearing link position increment.
 */
void ApplyRoomTransition(GBState *gb,
                         void (*bg_collision_handler)(GBState *),
                         void (*create_following_npc)(GBState *),
                         void (*set_world_music_track)(GBState *),
                         void (*clear_link_position_increment)(GBState *));

/**
 * Room transition prepare handler (02:79FA-02:7ADB).
 * Handles Wind Fish's Egg maze, indoor/overworld room increment, and loads room.
 *
 * @param gb Pointer to Game Boy system state.
 * @param load_room Optional callback for loading room.
 * @param replace_objects56and57 Optional callback for replacing objects 56 and 57.
 * @param load_room_entities Optional callback for loading room entities.
 * @param draw_link_sprite Optional callback for drawing Link sprite.
 * @param apply_link_motion_state Optional callback for applying link motion state.
 * @param select_room_tilesets Optional callback for selecting room tilesets.
 * @param reset_music_fade_timer Optional callback for resetting music fade timer.
 */
void RoomTransitionPrepareHandler(GBState *gb,
                                  void (*load_room)(GBState *, const void *),
                                  void (*replace_objects56and57)(GBState *, uint8_t, void (*)(GBState *)),
                                  void (*load_room_entities)(GBState *,
                                                             void (*)(GBState *),
                                                             uint16_t (*)(GBState *, uint8_t),
                                                             void (*)(GBState *, uint8_t),
                                                             void (*)(GBState *, uint8_t)),
                                  void (*draw_link_sprite)(GBState *),
                                  void (*apply_link_motion_state)(GBState *,
                                                                  void (*)(GBState *),
                                                                  void (*)(GBState *),
                                                                  void (*)(GBState *)),
                                  void (*select_room_tilesets)(GBState *),
                                  void (*reset_music_fade_timer)(GBState *));

/**
 * Room transition load tiles handler (02:7B3E-02:7B4B).
 * Selects room tilesets and handles switch blocks.
 *
 * @param gb Pointer to Game Boy system state.
 * @param select_room_tilesets Optional callback for selecting room tilesets.
 */
void RoomTransitionLoadTiles(GBState *gb, void (*select_room_tilesets)(GBState *));

/**
 * Room transition configure scroll targets handler (02:7B7F-02:7BFC).
 * Configures scroll targets, BG update region, and transition timing.
 *
 * @param gb Pointer to Game Boy system state.
 */
void RoomTransitionConfigureScrollTargets(GBState *gb);

/**
 * Room transition first half handler (02:7C00-02:7C02).
 * Updates BG region.
 *
 * @param gb Pointer to Game Boy system state.
 * @param update_bg_region Optional callback for updating BG region.
 * @param func_020_4a76 Optional callback for func_020_4a76.
 * @param get_bg_attr_addr Optional callback for getting BG attribute address.
 * @param switch_to_tilemap_bank Optional callback for switching to tilemap bank.
 * @param func_020_49d9 Optional callback for func_020_49d9.
 * @param update_origin Optional callback for updating origin.
 */
void RoomTransitionFirstHalfHandler(GBState *gb,
                                    void (*update_bg_region)(GBState *,
                                                             void (*)(GBState *),
                                                             void (*)(GBState *),
                                                             void (*)(GBState *),
                                                             void (*)(GBState *),
                                                             void (*)(GBState *)),
                                    void (*func_020_4a76)(GBState *),
                                    void (*get_bg_attr_addr)(GBState *),
                                    void (*switch_to_tilemap_bank)(GBState *),
                                    void (*func_020_49d9)(GBState *),
                                    void (*update_origin)(GBState *));

/**
 * Room transition second half handler (02:7C03-02:7C03).
 * The scroll increment has already been done earlier: nothing more to do.
 *
 * @param gb Pointer to Game Boy system state.
 */
void RoomTransitionSecondHalfHandler(GBState *gb);

/**
 * Conveyor belt physics handler (02:7C14-02:7C3F).
 * Moves Link according to conveyor belt direction.
 *
 * @param gb Pointer to Game Boy system state.
 */
void label_002_7C14(GBState *gb);

/**
 * Lava/Deep water physics handler (02:7C50-02:7C9E).
 * Handles Link movement in lava, deep water, and river rapids.
 *
 * @param gb Pointer to Game Boy system state.
 * @param bg_collision_handler Optional callback for background collision handling.
 */
void label_002_7C50(GBState *gb, void (*bg_collision_handler)(GBState *));

#endif /* LADX_BANK2_ROOM_TRANSITION_H */
