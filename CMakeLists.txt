cmake_minimum_required(VERSION 4.3.0)

project(etripator
    VERSION 1.0.0
    LANGUAGES C
)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS, "No build type specified. Force build type to Debug.")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

option(BUILD_TESTS "Enable unit tests" ON)

set(CMAKE_C_STANDARDS 11)
add_compile_options(-Wall -Wextra -Wshadow)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_compile_definitions(_DARWIN_C_SOURCE=__DARWIN_C_FULL)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(CheckSymbolExists)
check_symbol_exists(strdup "string.h" HAVE_STRDUP)

find_package(Jansson)

add_subdirectory(externals EXCLUDE_FROM_ALL)

add_subdirectory(src)

#[[

add_library(etripator STATIC ${etripator_SRC} ${etripator_HDR})
set_target_properties(etripator PROPERTIES C_STANDARD 11)
target_link_libraries(etripator PUBLIC jansson cwalk)
target_include_directories(etripator PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(etripator PUBLIC externals)

add_executable(etripator_cli cli/etripator.c cli/options.c)
set_target_properties(etripator_cli 
    PROPERTIES
        OUTPUT_NAME etripator
        C_STANDARD 11
)
target_include_directories(etripator_cli PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(etripator_cli PUBLIC externals)
target_link_libraries(etripator_cli etripator cargs)

#]]

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

find_package(Doxygen)

set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in)
set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY IMMEDIATE)

add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
    SOURCES ${DOXYFILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)

