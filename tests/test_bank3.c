#include <stdio.h>
#include "bank3/test_bank3.h"

void run_bank3_tests(void) {
    printf("[*] Running Bank 3 unit tests...\n");

    test_bank3_entities();
    test_bank3_entities_physics();
    test_bank3_entities_collision();

    printf("[+] Bank 3 unit tests passed successfully!\n");
}