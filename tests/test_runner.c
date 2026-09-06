#include <stdio.h>
#include <stdlib.h>

extern int run_clear_memory_tests(void);
extern int run_copy_data_tests(void);
extern int run_bank_tests(void);
extern void run_lcd_tests(void);
extern void run_entities_tests(void);
extern void run_audio_tests(void);
extern void run_dialog_tests(void);
extern void run_room_tests(void);
extern void run_gameplay_tests(void);
extern void run_link_tests(void);
extern void run_vfx_tests(void);
extern void run_ui_tests(void);
extern int run_gfx_tests(void);
extern void run_check_items_to_use_tests(void);
extern void run_animated_tiles_tests(void);

int main(void) {
    int total_failures = 0;

    printf("========================================\n");
    printf("   LADX Decompilation Verification Tests\n");
    printf("========================================\n\n");

    total_failures += run_clear_memory_tests();
    total_failures += run_copy_data_tests();
    total_failures += run_bank_tests();
    run_lcd_tests();
    run_entities_tests();
    run_audio_tests();
    run_dialog_tests();
    run_room_tests();
    run_gameplay_tests();
    run_link_tests();
    run_vfx_tests();
    run_ui_tests();
    total_failures += run_gfx_tests();
    run_check_items_to_use_tests();
    run_animated_tiles_tests();

    printf("========================================\n");
    if (total_failures == 0) {
        printf("ALL TESTS PASSED SUCCESSFULLY!\n");
        printf("========================================\n");
        return 0;
    } else {
        printf("FAILED: %d test(s) failed!\n", total_failures);
        printf("========================================\n");
        return 1;
    }
}
