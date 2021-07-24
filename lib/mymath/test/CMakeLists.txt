# CMakeLists file in /lib/test

cmake_minimum_required(VERSION 3.15)

set(TEST_PROJECT_NAME MYMATHLIBTEST)
set(TEST_PROJECT_EXE_NAME mymathTest)
set(LIBRARY_TEST_SOURCE mymath_test.cpp)

project(${TEST_PROJECT_NAME})

enable_testing()

if (NOT (TARGET gtest AND TARGET gtest_main))
    message(FATAL_ERROR "gtest and gtest_main targets NOT found")
endif()

message("library name : " ${LIBRARY_NAME})

add_executable(${TEST_PROJECT_EXE_NAME} ${LIBRARY_TEST_SOURCE})
target_link_libraries(${TEST_PROJECT_EXE_NAME}
    gtest 
    gtest_main 
    ${LIBRARY_NAME} # NOTE: This is defined from project above
)
add_test(${TEST_PROJECT_EXE_NAME} ${TEST_PROJECT_NAME})