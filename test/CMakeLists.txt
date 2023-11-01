if("${CMAKE_C_COMPILER_ID}" IN_LIST "GNU;Clang;AppleClang")
    add_compile_options(-Wall -Wshadow -Wextra)
endif()

add_executable(section_tests section.c)
target_link_libraries(section_tests munit etripator)
target_include_directories(section_tests PRIVATE ${PROJECT_SOURCE_DIR} ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE})
add_test(NAME section_tests 
         COMMAND $<TARGET_FILE:section_tests>
         WORKING_DIRECTORY $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)
set_target_properties(section_tests PROPERTIES C_STANDARD 11)
add_custom_command(TARGET section_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:section_tests>/data)

add_executable(label_tests label.c)
target_link_libraries(label_tests munit etripator)
target_include_directories(label_tests PRIVATE ${PROJECT_SOURCE_DIR} ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE})
add_test(NAME label_tests 
         COMMAND $<TARGET_FILE:label_tests>)
set_target_properties(label_tests PROPERTIES C_STANDARD 11)

add_executable(comment_tests comment.c)
target_link_libraries(comment_tests munit etripator)
target_include_directories(comment_tests PRIVATE ${PROJECT_SOURCE_DIR} ${JANSSON_INCLUDE_DIRS} ${EXTRA_INCLUDE})
add_test(NAME comment_tests 
         COMMAND $<TARGET_FILE:comment_tests>)
set_target_properties(comment_tests PROPERTIES C_STANDARD 11)
