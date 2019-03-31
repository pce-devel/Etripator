add_library(munit STATIC ${CMAKE_CURRENT_LIST_DIR}/munit/munit.c)
target_include_directories(munit PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/munit>)
set_property(TARGET munit PROPERTY INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/munit>)

add_executable(section_tests section.c ../section.c ../section/load.c ../section/save.c ../jsonhelpers.c ../message.c ../message/file.c ../message/console.c)
set_property(TARGET section_tests PROPERTY C_STANDARD 99)
target_compile_options(section_tests PRIVATE -Wall -Wshadow -Wextra)
target_link_libraries(section_tests munit ${JANSSON_LIBRARIES})
target_include_directories(section_tests PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME section_tests 
         COMMAND $<TARGET_FILE:section_tests>
         WORKING_DIRECTORY $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

add_executable(label_tests label.c ../label.c ../message.c ../message/file.c ../message/console.c)
set_property(TARGET label_tests PROPERTY C_STANDARD 99)
target_compile_options(label_tests PRIVATE -Wall -Wshadow -Wextra)
target_link_libraries(label_tests munit ${JANSSON_LIBRARIES})
target_include_directories(label_tests PRIVATE ${PROJECT_SOURCE_DIR})
add_test(NAME label_tests 
         COMMAND $<TARGET_FILE:label_tests>)

add_custom_command(TARGET section_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:section_tests>/data)
