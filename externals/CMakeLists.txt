add_library(argparse STATIC argparse/argparse.c argparse/argparse.h)

add_library(munit STATIC ${CMAKE_CURRENT_LIST_DIR}/munit/munit.c)
target_include_directories(munit PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/munit>)
set_property(TARGET munit PROPERTY INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/munit>)
