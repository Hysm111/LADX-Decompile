#include "test_support.h"

#include "gb.h"
#include "constants/entities.h"
#include "constants/memory.h"

#include <stdbool.h>
#include <stdint.h>

int g_mock_ground_physics_calls = 0;
void mock_apply_ground_physics(GBState *gb) {
    (void)gb;
    g_mock_ground_physics_calls++;
}

int g_mock_map_transition_calls = 0;
int g_mock_side_scrolling_calls = 0;
void mock_side_scrolling_physics(GBState *gb) {
    (void)gb;
    g_mock_side_scrolling_calls++;
}

int g_mock_ocarina_calls = 0;
void mock_ocarina_handler(GBState *gb) {
    (void)gb;
    g_mock_ocarina_calls++;
}

int g_mock_753a_calls = 0;
void mock_func_002_753a(GBState *gb) {
    (void)gb;
    g_mock_753a_calls++;
}

int g_mock_update_link_anim_calls = 0;
void mock_update_link_animation(GBState *gb) {
    (void)gb;
    g_mock_update_link_anim_calls++;
}

int g_mock_4b49_calls = 0;
void mock_func_002_4b49(GBState *gb) {
    (void)gb;
    g_mock_4b49_calls++;
}
int g_mock_sword_collision_calls = 0;
void mock_check_collision(GBState *gb) {
    (void)gb;
    g_mock_sword_collision_calls++;
}
int g_mock_bg_collision_calls = 0;
void mock_bg_collision(GBState *gb) {
    (void)gb;
    g_mock_bg_collision_calls++;
}

int g_mock_select_music_calls = 0;
void mock_select_music(GBState *gb) {
    (void)gb;
    g_mock_select_music_calls++;
}

uint16_t mock_spawn_entity(GBState *gb, uint8_t entity_type) {
    gb_write(gb, (uint16_t)(wEntitiesTypeTable + 15), entity_type);
    return 15;
}

int g_mock_20_4b4a_calls = 0;
void mock_20_4b4a(GBState *gb) {
    (void)gb;
    g_mock_20_4b4a_calls++;
}

int g_mock_20_4ab3_calls = 0;
void mock_20_4ab3(GBState *gb) {
    (void)gb;
    g_mock_20_4ab3_calls++;
}

int g_mock_dig_action_calls = 0;
bool mock_dig_action(GBState *gb) {
    (void)gb;
    g_mock_dig_action_calls++;
    return true;
}

int g_mock_apply_vector_calls = 0;
void mock_apply_vector(GBState *gb) {
    (void)gb;
    g_mock_apply_vector_calls++;
}
void mock_check_map_transition(GBState *gb) {
    (void)gb;
    g_mock_map_transition_calls++;
}

uint16_t mock_spawn_entity_slot3(GBState *gb, uint8_t entity_type) {
    (void)entity_type;
    gb_write(gb, (uint16_t)(wEntitiesStatusTable + 3), ENTITY_STATUS_ACTIVE);
    return 3;
}

uint16_t mock_spawn_entity_fail(GBState *gb, uint8_t entity_type) {
    (void)gb;
    (void)entity_type;
    return 0xFFFF;
}

int g_mock_sync_flags_calls = 0;
void mock_sync_flags(GBState *gb) {
    (void)gb;
    g_mock_sync_flags_calls++;
}

int g_mock_reveal_object_calls = 0;
void mock_reveal_object(GBState *gb) {
    (void)gb;
    g_mock_reveal_object_calls++;
}

int g_mock_transient_vfx_calls = 0;
uint8_t g_mock_transient_vfx_last_slot = 0;
void mock_render_transient_vfx(GBState *gb, uint8_t slot) {
    (void)gb;
    g_mock_transient_vfx_calls++;
    g_mock_transient_vfx_last_slot = slot;
}

int g_mock_spawn_chest_calls = 0;
void mock_spawn_chest(GBState *gb) {
    (void)gb;
    g_mock_spawn_chest_calls++;
}

int g_mock_spawn_stairs_calls = 0;
void mock_spawn_stairs(GBState *gb) {
    (void)gb;
    g_mock_spawn_stairs_calls++;
}

int g_mock_room_triggers_calls = 0;
void mock_room_triggers(GBState *gb) {
    (void)gb;
    g_mock_room_triggers_calls++;
}

uint16_t mock_get_adjacent_room_status_addr(GBState *gb, uint16_t room) {
    (void)gb;
    return (uint16_t)(wIndoorARoomStatus + (room & 0xFF));
}

int g_mock_create_following_npc_calls = 0;
void mock_create_following_npc(GBState *gb) {
    (void)gb;
    g_mock_create_following_npc_calls++;
}

int g_mock_set_world_music_calls = 0;
void mock_set_world_music(GBState *gb) {
    (void)gb;
    g_mock_set_world_music_calls++;
}

int g_mock_clear_link_pos_calls = 0;
void mock_clear_link_pos(GBState *gb) {
    (void)gb;
    g_mock_clear_link_pos_calls++;
}

int g_mock_load_room_calls = 0;
void mock_load_room(GBState *gb, const void *data) {
    (void)gb; (void)data;
    g_mock_load_room_calls++;
}

int g_mock_replace_objects_calls = 0;
void mock_replace_objects(GBState *gb, uint8_t a, void (*cb)(GBState *)) {
    (void)gb; (void)a; (void)cb;
    g_mock_replace_objects_calls++;
}

int g_mock_load_entities_calls = 0;
void mock_load_entities(GBState *gb,
                        void (*cb1)(GBState *),
                        uint16_t (*cb2)(GBState *, uint8_t),
                        void (*cb3)(GBState *, uint8_t),
                        void (*cb4)(GBState *, uint8_t)) {
    (void)gb; (void)cb1; (void)cb2; (void)cb3; (void)cb4;
    g_mock_load_entities_calls++;
}

int g_mock_draw_link_calls = 0;
void mock_draw_link(GBState *gb) {
    (void)gb;
    g_mock_draw_link_calls++;
}

int g_mock_apply_motion_calls = 0;
void mock_apply_motion(GBState *gb,
                       void (*cb1)(GBState *),
                       void (*cb2)(GBState *),
                       void (*cb3)(GBState *)) {
    (void)gb; (void)cb1; (void)cb2; (void)cb3;
    g_mock_apply_motion_calls++;
}

int g_mock_select_tilesets_calls = 0;
void mock_select_tilesets(GBState *gb) {
    (void)gb;
    g_mock_select_tilesets_calls++;
}

int g_mock_reset_music_fade_calls = 0;
void mock_reset_music_fade(GBState *gb) {
    (void)gb;
    g_mock_reset_music_fade_calls++;
}

int g_mock_update_bg_region_calls = 0;
void mock_update_bg_region(GBState *gb,
                           void (*cb1)(GBState *),
                           void (*cb2)(GBState *),
                           void (*cb3)(GBState *),
                           void (*cb4)(GBState *),
                           void (*cb5)(GBState *)) {
    (void)gb; (void)cb1; (void)cb2; (void)cb3; (void)cb4; (void)cb5;
    g_mock_update_bg_region_calls++;
}

int g_mock_func_002_5928_calls = 0;
void mock_func_002_5928(GBState *gb, uint8_t y) {
    (void)gb; (void)y;
    g_mock_func_002_5928_calls++;
}

int g_mock_label_002_4d97_calls = 0;
void mock_label_002_4d97(GBState *gb) {
    (void)gb;
    g_mock_label_002_4d97_calls++;
}

int g_mock_reset_spin_attack_calls = 0;
void mock_reset_spin_attack(GBState *gb) {
    (void)gb;
    g_mock_reset_spin_attack_calls++;
}

int g_mock_func_1828_calls = 0;
void mock_func_1828(GBState *gb) {
    (void)gb;
    g_mock_func_1828_calls++;
}

int g_mock_apply_map_fade_calls = 0;
void mock_apply_map_fade(GBState *gb) {
    (void)gb;
    g_mock_apply_map_fade_calls++;
}

int g_mock_func_002_755b_calls = 0;
void mock_func_002_755b(GBState *gb) {
    (void)gb;
    g_mock_func_002_755b_calls++;
}

int g_mock_apply_link_motion_state_calls = 0;
void mock_apply_link_motion_state(GBState *gb, void (*cb1)(GBState *), void (*cb2)(GBState *), void (*cb3)(GBState *)) {
    (void)gb; (void)cb1; (void)cb2; (void)cb3;
    g_mock_apply_link_motion_state_calls++;
}

int g_mock_draw_link_sprite_calls = 0;
void mock_draw_link_sprite(GBState *gb) {
    (void)gb;
    g_mock_draw_link_sprite_calls++;
}

int g_mock_animate_entities_calls = 0;
void mock_animate_entities(GBState *gb, void (*cb)(GBState *)) {
    (void)gb; (void)cb;
    g_mock_animate_entities_calls++;
}

int g_mock_open_dialog_table0_calls = 0;
void mock_open_dialog_table0(GBState *gb, uint8_t dialog_index) {
    (void)gb; (void)dialog_index;
    g_mock_open_dialog_table0_calls++;
}

int g_mock_open_dialog_table2_calls = 0;
void mock_open_dialog_table2(GBState *gb, uint8_t dialog_index) {
    (void)gb; (void)dialog_index;
    g_mock_open_dialog_table2_calls++;
}

int g_mock_reset_entity_calls = 0;
void mock_reset_entity(GBState *gb) {
    (void)gb;
    g_mock_reset_entity_calls++;
}

int g_mock_configure_hitbox_calls = 0;
void mock_configure_hitbox(GBState *gb, uint16_t bc) {
    (void)gb; (void)bc;
    g_mock_configure_hitbox_calls++;
}

int g_mock_label_27F2_calls = 0;
void mock_label_27F2(GBState *gb) {
    (void)gb;
    g_mock_label_27F2_calls++;
}

int g_mock_get_entity_init_handler_calls = 0;
void mock_get_entity_init_handler(GBState *gb, uint8_t bank, void (*func)(GBState *)) {
    (void)gb; (void)bank; (void)func;
    g_mock_get_entity_init_handler_calls++;
}

int g_mock_unload_entity_calls = 0;
void mock_unload_entity(GBState *gb, uint16_t entity_index) {
    (void)gb; (void)entity_index;
    g_mock_unload_entity_calls++;
}

int g_mock_set_entity_sprite_variant_calls = 0;
void mock_set_entity_sprite_variant(GBState *gb, uint16_t entity_index, uint8_t variant) {
    (void)gb; (void)entity_index; (void)variant;
    g_mock_set_entity_sprite_variant_calls++;
}

int g_mock_get_object_physics_calls = 0;
uint8_t mock_get_object_physics(GBState *gb, uint8_t obj_id) {
    g_mock_get_object_physics_calls++;
    (void)gb;
    return obj_id; /* Return physics = object ID for testing */
}
