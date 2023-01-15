cmake_minimum_required(VERSION 3.24)

project(cshell-tests
        VERSION 0.0.1
        DESCRIPTION "Tests for cshell"
        LANGUAGES C
        )

set(CMAKE_C_STANDARD 17)

set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)

set(TEST_HEADER_LIST
        ${HEADER_DIR}/tests.h
        )

set(TEST_SOURCE_LIST
#        ${SOURCE_DIR}/builtin_tests.c
#        ${SOURCE_DIR}/command_tests.c
#        ${SOURCE_DIR}/execute_tests.c
#        ${SOURCE_DIR}/input_tests.c
        ${SOURCE_DIR}/main.c
#        ${SOURCE_DIR}/shell_impl_tests.c
#        ${SOURCE_DIR}/shell_tests.c
        ${SOURCE_DIR}/util_tests.c
        )

include_directories(${CGREEN_PUBLIC_INCLUDE_DIRS} ${PROJECT_BINARY_DIR})
add_executable(libdc_shell_test ${TEST_SOURCE_LIST} ${TEST_HEADER_LIST} ${SOURCE_LIST} ${HEADER_LIST})

target_compile_features(libdc_shell_test PRIVATE c_std_17)

target_include_directories(libdc_shell_test PRIVATE ../include)
target_include_directories(libdc_shell_test PRIVATE /usr/local/include)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_include_directories(libdc_shell_test PRIVATE /opt/homebrew/include)
else ()
    target_include_directories(libdc_shell_test PRIVATE /usr/include)
endif ()

find_library(LIBCGREEN cgreen REQUIRED)
find_library(LIBDC_ERROR dc_error REQUIRED)
find_library(LIBDC_ENV dc_env REQUIRED)
find_library(LIBDC_C dc_c REQUIRED)
find_library(LIBDC_POSIX dc_posix REQUIRED)
find_library(LIBDC_UNIX dc_unix REQUIRED)
find_library(LIBDC_UTIL dc_util REQUIRED)
find_library(LIBDC_FSM dc_fsm REQUIRED)
find_library(LIB_CONFIG config REQUIRED)
find_library(LIBDC_APPLICATION dc_application REQUIRED)
find_library(LIBMEM_MANAGER mem_manager REQUIRED)

target_link_libraries(libdc_shell_test PRIVATE ${LIBCGREEN})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_ERROR})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_ENV})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_C})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_POSIX})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_UNIX})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_UTIL})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_FSM})
target_link_libraries(libdc_shell_test PUBLIC ${LIB_CONFIG})
target_link_libraries(libdc_shell_test PUBLIC ${LIBDC_APPLICATION})
target_link_libraries(libdc_shell_test PUBLIC ${LIBMEM_MANAGER})

add_test(NAME libdc_shell_test COMMAND libdc_shell_test)
