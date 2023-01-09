#include "../include/tests.h"
#include "../../include/util.h"
#include "../../include/input.h"
#include <dc_env/env.h>
#include <dc_error/error.h>

// NOLINTBEGIN

Describe(input);

static void test_read_command_line(FILE *strstream, size_t *line_length, const char *str);

static struct dc_error *error;
static struct dc_env *environ;

BeforeEach(input)
{
    error = dc_error_create(false);
    environ = dc_env_create(error, false, NULL);
}

AfterEach(input)
{
    dc_error_reset(error);
}

Ensure(input, read_command_line)
{
    char str[BUFSIZ];
    FILE *strstream;
    size_t line_length;
    
    strstream = fmemopen(str, sizeof(str), "r");
    strcpy(str, "hello\n");
    line_length = 10;
    test_read_command_line(strstream, &line_length, "hello\n");
}

static void test_read_command_line(FILE *strstream, size_t *line_length, const char *expected_line)
{
    char *line;
    
    line = read_command_line(environ, error, strstream, NULL, line_length);
    
    assert_that(line, is_equal_to_string(expected_line));
    assert_that(*line_length, is_equal_to(strlen(line)));
}

TestSuite *input_tests(void)
{
    TestSuite *suite;
    
    suite = create_test_suite();
    add_test_with_context(suite, input, read_command_line);
    
    return suite;
}


// NOLINTEND
