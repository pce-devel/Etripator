if("${CMAKE_C_COMPILER_ID}" IN_LIST "GNU;Clang;AppleClang")
    add_compile_options(-Wall -Wshadow -Wextra)
endif()

function(add_unit_test)
    set(options )
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES LIBRARIES INCLUDE_DIRECTORIES WRAP)
    cmake_parse_arguments(ut "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(target_name "${ut_NAME}_ut")

    list(REMOVE_DUPLICATES ut_SOURCES)
    list(REMOVE_DUPLICATES ut_WRAP)

    add_executable(${target_name} ${ut_SOURCES})
    target_link_libraries(${target_name} PRIVATE ${ut_LIBRARIES} munit)
    target_include_directories(${target_name} PRIVATE ${ut_INCLUDE_DIRECTORIES} ${EXTRA_INCLUDE})
    set_target_properties(${target_name} PROPERTIES C_STANDARD 11)
    foreach(_wrap ${ut_WRAP})
        target_link_options(${target_name} PRIVATE LINKER:--wrap=${_wrap})
    endforeach()

    add_test(
        NAME ${target_name} 
        COMMAND $<TARGET_FILE:${target_name}>
        WORKING_DIRECTORY $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    )
    add_custom_command(
        TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/data $<TARGET_FILE_DIR:${target_name}>/data
    )
endfunction()

add_unit_test(
    NAME message
    SOURCES 
        message.c 
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME memory
    SOURCES 
        memory.c 
        ${PROJECT_SOURCE_DIR}/memory.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME memory_map
    SOURCES 
        memory_map.c 
        ${PROJECT_SOURCE_DIR}/memory_map.c
        ${PROJECT_SOURCE_DIR}/memory.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME rom
    SOURCES 
        rom.c 
        ${PROJECT_SOURCE_DIR}/rom.c
        ${PROJECT_SOURCE_DIR}/memory_map.c
        ${PROJECT_SOURCE_DIR}/memory.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk fff
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
    WRAP fopen fileno fstat fseek fread fclose
)

add_unit_test(
    NAME cd
    SOURCES 
        cd.c 
        ${PROJECT_SOURCE_DIR}/cd.c
        ${PROJECT_SOURCE_DIR}/memory_map.c
        ${PROJECT_SOURCE_DIR}/memory.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME label
    SOURCES 
        label.c 
        ${PROJECT_SOURCE_DIR}/label.c
        ${PROJECT_SOURCE_DIR}/label/load.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/jsonhelpers.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME comment
    SOURCES 
        comment.c 
        ${PROJECT_SOURCE_DIR}/comment.c
        ${PROJECT_SOURCE_DIR}/comment/load.c
        ${PROJECT_SOURCE_DIR}/comment/save.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/jsonhelpers.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME section
    SOURCES 
        section.c 
        ${PROJECT_SOURCE_DIR}/section.c
        ${PROJECT_SOURCE_DIR}/section/load.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/jsonhelpers.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME opcodes
    SOURCES 
        opcodes.c 
        ${PROJECT_SOURCE_DIR}/opcodes.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)
