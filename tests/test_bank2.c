#include <stdio.h>
#include "bank2/test_bank2.h"

void run_bank2_tests(void) {
    printf("[*] Running Bank 2 unit tests...\n");

    test_bank2_tables();
    test_bank2_audio();
    test_bank2_chest();
    test_bank2_ocarina_use();
    test_bank2_hookshot();
    test_bank2_link_motion();
    test_bank2_sword();
    test_bank2_link_animation();
    test_bank2_ocarina_playing();
    test_bank2_shovel();
    test_bank2_revolving_door();
    test_bank2_swimming();
    test_bank2_falling();
    test_bank2_got_item();
    test_bank2_recovery();
    test_bank2_magic_rod();
    test_bank2_room();
    test_bank2_vfx();
    test_bank2_room_events();
    test_bank2_room_effects();
    test_bank2_room_effect_appearance();
    test_bank2_key_drop_effect();
    test_bank2_shutter_effects();

    printf("[+] Bank 2 unit tests passed successfully!\n");
}
