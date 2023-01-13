cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "gcc")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(llp-lab1 LANGUAGES C)

file(GLOB_RECURSE C_FILES_LIB CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE H_FILES_LIB CONFIGURE_DEPENDS src/*.h)

file(GLOB_RECURSE C_FILES_TESTS CONFIGURE_DEPENDS src/tests/*.c)
file(GLOB_RECURSE H_FILES_TESTS CONFIGURE_DEPENDS src/tests/*.h)

add_compile_options(
  -pedantic-errors
  -Wall -Werror
  -pedantic-errors
  -g3
  -O0
  -fsanitize=undefined,address,leak
)

add_library(${PROJECT_NAME} STATIC ${C_FILES_LIB} ${H_FILES_LIB})
target_include_directories(${PROJECT_NAME} PUBLIC src/main)

add_executable(${PROJECT_NAME}-tests ${C_FILES_TESTS} ${H_FILES_TESTS})
target_include_directories(${PROJECT_NAME}-tests PUBLIC src/tests)
target_link_libraries(${PROJECT_NAME}-tests -fsanitize=undefined,address,leak ${PROJECT_NAME})

add_custom_target(testenv
  COMMAND ${CMAKE_COMMAND} -E make_directory testenv)

add_custom_target(test
  DEPENDS testenv
  WORKING_DIRECTORY testenv
  COMMAND ${PROJECT_NAME}-tests
  DEPENDS ${PROJECT_NAME}-tests
  COMMAND del database
)
