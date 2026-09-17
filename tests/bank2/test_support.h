#ifndef LADX_TESTS_BANK2_TEST_SUPPORT_H
#define LADX_TESTS_BANK2_TEST_SUPPORT_H

#include "gb.h"

/* Shared mock state preserves the original bank2 runner's callback semantics. */
extern int g_mock_ground_physics_calls;
extern int g_mock_map_transition_calls;
extern int g_mock_side_scrolling_calls;
extern int g_mock_ocarina_calls;
extern int g_mock_753a_calls;
extern int g_mock_update_link_anim_calls;
extern int g_mock_4b49_calls;
extern int g_mock_sword_collision_calls;
extern int g_mock_bg_collision_calls;
extern int g_mock_select_music_calls;
extern int g_mock_20_4b4a_calls;
extern int g_mock_20_4ab3_calls;
extern int g_mock_dig_action_calls;
extern int g_mock_apply_vector_calls;
extern int g_mock_sync_flags_calls;
extern int g_mock_reveal_object_calls;
extern int g_mock_transient_vfx_calls;
extern uint8_t g_mock_transient_vfx_last_slot;
extern int g_mock_spawn_chest_calls;
extern int g_mock_spawn_stairs_calls;
extern int g_mock_room_triggers_calls;

void mock_apply_ground_physics(GBState *gb);
void mock_side_scrolling_physics(GBState *gb);
void mock_ocarina_handler(GBState *gb);
void mock_func_002_753a(GBState *gb);
void mock_update_link_animation(GBState *gb);
void mock_func_002_4b49(GBState *gb);
void mock_check_collision(GBState *gb);
void mock_bg_collision(GBState *gb);
void mock_select_music(GBState *gb);
uint16_t mock_spawn_entity(GBState *gb, uint8_t entity_type);
void mock_20_4b4a(GBState *gb);
void mock_20_4ab3(GBState *gb);
bool mock_dig_action(GBState *gb);
void mock_apply_vector(GBState *gb);
void mock_check_map_transition(GBState *gb);
uint16_t mock_spawn_entity_slot3(GBState *gb, uint8_t entity_type);
uint16_t mock_spawn_entity_fail(GBState *gb, uint8_t entity_type);
void mock_sync_flags(GBState *gb);
void mock_reveal_object(GBState *gb);
void mock_render_transient_vfx(GBState *gb, uint8_t slot);
void mock_spawn_chest(GBState *gb);
void mock_spawn_stairs(GBState *gb);
void mock_room_triggers(GBState *gb);
uint16_t mock_get_adjacent_room_status_addr(GBState *gb, uint16_t room);

#endif /* LADX_TESTS_BANK2_TEST_SUPPORT_H */
