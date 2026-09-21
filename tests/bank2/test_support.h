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
extern int g_mock_get_object_physics_calls;
extern int g_mock_label_002_4d97_calls;
extern int g_mock_create_following_npc_calls;
extern int g_mock_set_world_music_calls;
extern int g_mock_clear_link_pos_calls;
extern int g_mock_load_room_calls;
extern int g_mock_replace_objects_calls;
extern int g_mock_load_entities_calls;
extern int g_mock_draw_link_calls;
extern int g_mock_apply_motion_calls;
extern int g_mock_select_tilesets_calls;
extern int g_mock_reset_music_fade_calls;
extern int g_mock_update_bg_region_calls;
extern int g_mock_func_002_5928_calls;
extern int g_mock_label_002_4d97_calls;
extern int g_mock_reset_spin_attack_calls;
extern int g_mock_func_1828_calls;
extern int g_mock_apply_map_fade_calls;
extern int g_mock_func_002_755b_calls;
extern int g_mock_apply_link_motion_state_calls;
extern int g_mock_draw_link_sprite_calls;
extern int g_mock_animate_entities_calls;
extern int g_mock_open_dialog_table0_calls;
extern int g_mock_open_dialog_table2_calls;
extern int g_mock_reset_entity_calls;
extern int g_mock_configure_hitbox_calls;
extern int g_mock_label_27F2_calls;
extern int g_mock_get_entity_init_handler_calls;
extern int g_mock_unload_entity_calls;
extern int g_mock_set_entity_sprite_variant_calls;

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
void mock_create_following_npc(GBState *gb);
void mock_set_world_music(GBState *gb);
void mock_clear_link_pos(GBState *gb);
void mock_load_room(GBState *gb, const void *data);
void mock_replace_objects(GBState *gb, uint8_t a, void (*cb)(GBState *));
void mock_load_entities(GBState *gb,
                        void (*cb1)(GBState *),
                        uint16_t (*cb2)(GBState *, uint8_t),
                        void (*cb3)(GBState *, uint8_t),
                        void (*cb4)(GBState *, uint8_t));
void mock_draw_link(GBState *gb);
void mock_apply_motion(GBState *gb,
                       void (*cb1)(GBState *),
                       void (*cb2)(GBState *),
                       void (*cb3)(GBState *));
void mock_select_tilesets(GBState *gb);
void mock_reset_music_fade(GBState *gb);
void mock_update_bg_region(GBState *gb,
                           void (*cb1)(GBState *),
                           void (*cb2)(GBState *),
                           void (*cb3)(GBState *),
                           void (*cb4)(GBState *),
                           void (*cb5)(GBState *));
void mock_func_002_5928(GBState *gb, uint8_t y);
void mock_label_002_4d97(GBState *gb);
void mock_reset_spin_attack(GBState *gb);
void mock_func_1828(GBState *gb);
void mock_apply_map_fade(GBState *gb);
void mock_func_002_755b(GBState *gb);
void mock_apply_link_motion_state(GBState *gb, void (*cb1)(GBState *), void (*cb2)(GBState *), void (*cb3)(GBState *));
void mock_draw_link_sprite(GBState *gb);
void mock_animate_entities(GBState *gb, void (*cb)(GBState *));
void mock_open_dialog_table0(GBState *gb, uint8_t dialog_index);
void mock_open_dialog_table2(GBState *gb, uint8_t dialog_index);

#endif /* LADX_TESTS_BANK2_TEST_SUPPORT_H */
