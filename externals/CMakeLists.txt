add_subdirectory(cargs)
add_subdirectory(cwalk)

#[[
add_library(munit STATIC ${CMAKE_CURRENT_LIST_DIR}/munit/munit.c)
target_include_directories(munit PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/munit>)
set_property(TARGET munit PROPERTY INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/munit>)
target_compile_definitions(munit PRIVATE MUNIT_NO_FORK)
#]]

add_subdirectory(fff)

if(BUILD_TESTS)
    set(UNITY_EXTENSION_FIXTURE ON CACHE INTERNAL "" FORCE)
    add_subdirectory(unity EXCLUDE_FROM_ALL)
endif()