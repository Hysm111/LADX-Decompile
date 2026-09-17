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
