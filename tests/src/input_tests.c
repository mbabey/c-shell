#include "../include/tests.h"
#include "../../include/util.h"
#include "../../include/input.h"
#include <dc_env/env.h>
#include <dc_error/error.h>
#include <dc_c/dc_stdlib.h>

// NOLINTBEGIN

Describe(input);

static void test_read_command_line(const char *initial_line, ...);

static struct dc_error *error;
static struct dc_env   *environ;

BeforeEach(input)
{
    error   = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(input)
{
    dc_error_reset(error);
}

Ensure(input, read_command_line)
{
    test_read_command_line("z\n", "z", NULL);
    test_read_command_line("hello\nworld\n", "hello", "world", NULL);
    test_read_command_line("  hello  \n", "hello", NULL);
    test_read_command_line("  cruel gagoonga\t\n", "cruel gagoonga", NULL);
    test_read_command_line("./a.out hello < in.txt > out.txt 2> err.txt\n",
                           "./a.out hello < in.txt > out.txt 2> err.txt", NULL);
}

static void test_read_command_line(const char *initial_line, ...)
{
    char    *str;
    char    *expected_line;
    FILE    *strstream;
    va_list strings;
    size_t  line_length;
    
    line_length = strlen(initial_line + 1);
    str         = dc_malloc(environ, error, line_length);
    
    strstream = fmemopen(str, sizeof(str), "r");
    strcpy(str, initial_line);
    
    // va loop
    va_start(strings, initial_line);
    
    do
    {
        char *line;
        
        line = read_command_line(environ, error, strstream, NULL, &line_length);
        
        expected_line = va_arg(strings, char *);
        
        if (expected_line == NULL)
        {
            assert_that(line, is_equal_to_string(""));
            assert_that(line_length, is_equal_to(0));
        } else
        {
            assert_that(line, is_equal_to_string(expected_line));
            assert_that(line_length, is_equal_to(strlen(line)));
        }
    } while (expected_line);
    
    va_end(strings);
    
    fclose(strstream);
    dc_free(environ, str);
}

TestSuite *input_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, input, read_command_line);
    
    return suite;
}


// NOLINTEND
