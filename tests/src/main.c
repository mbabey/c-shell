#include "../include/tests.h"

int main(int argc, char *argv[])
{
    TestSuite    *suite;
    TestReporter *reporter;
    int          suite_result;
    
    suite    = create_test_suite();
    reporter = create_text_reporter();
    
    add_suite(suite, util_tests());
//    add_suite(suite, input_tests());
//    add_suite(suite, shell_impl_tests());
//    add_suite(suite, command_tests());
//    add_suite(suite, builtin_tests());
//    add_suite(suite, execute_tests());
//    add_suite(suite, shell_tests());
    
    if (argc > 1)
    {
        suite_result = run_single_test(suite, argv[1], reporter);
    } else
    {
        suite_result = run_test_suite(suite, reporter);
    }
    
    destroy_test_suite(suite);
    destroy_reporter(reporter);
    
    return suite_result;
}
