#include <stdio.h>
#include <stdlib.h>

extern int run_clear_memory_tests(void);
extern int run_copy_data_tests(void);
extern int run_bank_tests(void);
extern void run_lcd_tests(void);
extern void run_entities_tests(void);
extern void run_audio_tests(void);

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
