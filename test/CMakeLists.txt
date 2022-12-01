if(WIN32 AND MSVC)
    set(etripator_PLATFORM_SRC
        ../platform/windows/basename.c
    )
    set(etripator_PLATFORM_HDR
        ../platform/windows/basename.h
        ../platform/windows/config_win.h
    )
endif()

add_executable(section_tests section.c ../section.c ../section/load.c ../section/save.c ../jsonhelpers.c ../message.c ../message/file.c ../message/console.c ${etripator_PLATFORM_SRC} ${etripator_PLATFORM_HDR})
target_compile_features(section_tests PUBLIC c_std_11)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(section_tests PRIVATE -Wall -Wshadow -Wextra)
endif()
target_link_libraries(section_tests munit ${JANSSON_LIBRARIES})
target_include_directories(section_tests PRIVATE ${PROJECT_SOURCE_DIR} ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE})
add_test(NAME section_tests 
         COMMAND $<TARGET_FILE:section_tests>
         WORKING_DIRECTORY $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

add_executable(label_tests label.c ../label.c ../message.c ../message/file.c ../message/console.c ${etripator_PLATFORM_SRC} ${etripator_PLATFORM_HDR})
target_compile_features(label_tests PUBLIC c_std_11)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(label_tests PRIVATE -Wall -Wshadow -Wextra)
endif()
target_link_libraries(label_tests munit ${JANSSON_LIBRARIES})
target_include_directories(label_tests PRIVATE ${PROJECT_SOURCE_DIR} ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE})
add_test(NAME label_tests 
         COMMAND $<TARGET_FILE:label_tests>)

add_custom_command(TARGET section_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:section_tests>/data)
