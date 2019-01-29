cmake_minimum_required (VERSION 3.1)

project(etripator
    VERSION 0.7.0
    LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

find_package(Doxygen)
find_package(Jansson)

set(CMAKE_C_STANDARDS 11)

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
    label.h
    label/load.h
    label/save.h
    irq.h
    memory.h
    memorymap.h
    rom.h
    cd.h
    ipl.h
)

if(WIN32 AND MSVC)
    list(APPEND
        platform/windows/time.c
        platform/windows/basename.c
        etripator_SRC
    )
    list(APPEND
        platform/windows/time.h
        platform/windows/basename.h
        platform/windows/stdint.h
        platform/windows/inttypes.h
        platform/windows/config_win.h
        etripator_HDR
    )
    set(EXTRA_INCLUDE ${CMAKE_SOURCE_DIR}/platform/windows)
endif(WIN32 AND MSVC)

add_library(etripator STATIC ${etripator_SRC} ${etripator_HDR})
set_property(TARGET etripator PROPERTY C_STANDARD 99)
target_include_directories(etripator PUBLIC ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE})
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(etripator PRIVATE -Wall -Wshadow -Wextra)
endif(CMAKE_COMPILER_IS_GNUCXX)
target_compile_definitions(etripator PRIVATE _POSIX_C_SOURCE)
target_link_libraries(etripator ${JANSSON_LIBRARIES})

add_executable(etripator_cli cli/etripator.c cli/options.c)
target_include_directories(etripator_cli PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(etripator_cli etripator)

set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in)
set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY IMMEDIATE)

add_custom_target(doc
         COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
         SOURCES ${DOXYFILE}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMENT "Generating API documentation with Doxygen"
         VERBATIM )

add_subdirectory(test)