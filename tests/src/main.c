#include "../include/tests.h"
#include <stdlib.h>

int main()
{
    TestSuite *suite;
    TestReporter *reporter;
    int suite_result;
    
    suite = create_test_suite();
    reporter = create_text_reporter();
    
    add_suite(suite, buitlin_tests());
    add_suite(suite, command_tests());
    add_suite(suite, execute_tests());
    add_suite(suite, input_tests());
    add_suite(suite, shell_impl_tests());
    add_suite(suite, util_tests());
    
    return EXIT_SUCCESS;
}
