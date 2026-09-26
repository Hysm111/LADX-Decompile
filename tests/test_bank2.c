#include <stdio.h>
#include <time.h>
#include "bank2/test_bank2.h"

void run_bank2_tests(void) {
    printf("[*] Running Bank 2 unit tests...\n");

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_tables();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_tables: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_audio();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_audio: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_chest();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_chest: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_ocarina_use();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_ocarina_use: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_hookshot();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_hookshot: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_link_motion();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_link_motion: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_sword();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_sword: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_link_animation();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_link_animation: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_ocarina_playing();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_ocarina_playing: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_shovel();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_shovel: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_revolving_door();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_revolving_door: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_swimming();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_swimming: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_falling();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_falling: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_got_item();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_got_item: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_recovery();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_recovery: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_magic_rod();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_magic_rod: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_room();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_room: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_vfx();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_vfx: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_room_events();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_room_events: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_room_effects();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_room_effects: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_room_effect_appearance();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_room_effect_appearance: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_key_drop_effect();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_key_drop_effect: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_object_reveal();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_object_reveal: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_shutter_effects();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_shutter_effects: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_room_triggers();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_room_triggers: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_room_dispatch();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_room_dispatch: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_clamp_item_count();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_clamp_item_count: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_func_002_60E0();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_func_002_60E0: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_room_transition();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_room_transition: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_link_motion_helpers();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_link_motion_helpers: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    clock_gettime(CLOCK_MONOTONIC, &start);
    test_bank2_link_ground_physics();
    clock_gettime(CLOCK_MONOTONIC, &end); printf("  test_bank2_link_ground_physics: %.3fs\n", end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1e9);

    printf("[+] Bank 2 unit tests passed successfully!\n");
}
