cmake_minimum_required (VERSION 3.1)

project(etripator C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

enable_testing()

set(ETRIPATOR_MAJOR_VERSION 0)
set(ETRIPATOR_MINOR_VERSION 7)
set(ETRIPATOR_PATCH_VERSION 0)
set(ETRIPATOR_VERSION "${ETRIPATOR_MAJOR_VERSION}.${ETRIPATOR_MINOR_VERSION}.${ETRIPATOR_PATCH_VERSION}")

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
#    decode.c
    section.c
#    sectionsloader.c
#    opcodes.c
    label.c
#    label/loader.c
#    label/writer.c
    irq.c
    memory.c
    memorymap.c
    rom.c
    cd.c
    ipl.c
#    options.c
#    etripator.c
)

if(WIN32 AND MSVC)
    set(etripator_SRC ${etripator_SRC}
        platform/windows/getopt1.c
        platform/windows/getopt.c
        platform/windows/time.c
        platform/windows/basename.c
    )
    set(EXTRA_INCLUDE ${CMAKE_SOURCE_DIR}/platform/windows)
endif(WIN32 AND MSVC)

#add_executable(etripator ${etripator_SRC})
add_library(etripator STATIC ${etripator_SRC})
target_include_directories(etripator PUBLIC ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE})
if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(etripator PRIVATE -Wall -Wshadow -Wextra)
endif(CMAKE_COMPILER_IS_GNUCXX)
target_compile_definitions(etripator PRIVATE _POSIX_C_SOURCE)
target_link_libraries(etripator ${JANSSON_LIBRARIES})

set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in)
set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY IMMEDIATE)

add_custom_target(doc
         COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
         SOURCES ${DOXYFILE}
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
         COMMENT "Generating API documentation with Doxygen"
         VERBATIM )
         
add_library(munit STATIC ${CMAKE_CURRENT_SOURCE_DIR}/test/munit/munit.c)
target_include_directories(munit PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test/munit>)
set_property(TARGET munit PROPERTY INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test/munit>)

add_executable(section_tests test/section.c section.c jsonhelpers.c message.c message/file.c message/console.c)
target_compile_options(section_tests PRIVATE -Wall -Wshadow -Wextra)
target_link_libraries(section_tests munit ${JANSSON_LIBRARIES})
target_include_directories(section_tests PRIVATE ${PROJECT_SOURCE_DIR})
add_test(section_tests section_tests)

add_executable(label_tests test/label.c label.c message.c message/file.c message/console.c)
target_compile_options(label_tests PRIVATE -Wall -Wshadow -Wextra)
target_link_libraries(label_tests munit ${JANSSON_LIBRARIES})
target_include_directories(label_tests PRIVATE ${PROJECT_SOURCE_DIR})
add_test(label_tests label_tests)
# [todo] copy data directory
