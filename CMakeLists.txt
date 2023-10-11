cmake_minimum_required (VERSION 3.16)

project(etripator
    VERSION 0.9.0
    LANGUAGES C)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_compile_definitions(_XOPEN_SOURCE=700)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

enable_testing()

include(CheckSymbolExists)

check_symbol_exists(strdup "string.h" HAVE_STRDUP)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

find_package(Doxygen)
find_package(Jansson)

set(CMAKE_C_STANDARDS 11)

add_subdirectory(externals)

set(etripator_SRC
    message.c 
    message/file.c
    message/console.c
    jsonhelpers.c
    decode.c
    section.c
    section/load.c
    section/save.c
    opcodes.c
    comment.c
    label.c
    label/load.c
    label/save.c
    irq.c
    memory.c
    memorymap.c
    rom.c
    cd.c
    ipl.c
)

set(etripator_HDR
    config.h
    message.h
    message/file.h
    message/console.h
    jsonhelpers.h
    decode.h
    section.h
    section/load.h
    section/save.h
    opcodes.h
    comment.h
    label.h
    irq.h
    memory.h
    memorymap.h
    rom.h
    cd.h
    ipl.h
)

add_library(etripator STATIC ${etripator_SRC} ${etripator_HDR})
set_target_properties(etripator PROPERTIES C_STANDARD 11)
target_include_directories(etripator PUBLIC ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE} externals)
target_compile_definitions(etripator PRIVATE _POSIX_C_SOURCE)
target_link_libraries(etripator PUBLIC ${JANSSON_LIBRARIES} cwalk)

add_executable(etripator_cli cli/etripator.c cli/options.c)
set_target_properties(etripator_cli 
    PROPERTIES
        OUTPUT_NAME etripator
        C_STANDARD 11
)
target_include_directories(etripator_cli PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(etripator_cli etripator argparse)

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

add_subdirectory(test)

install(TARGETS etripator_cli DESTINATION bin)
