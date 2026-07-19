function(add_unit_test)
    set(options )
    set(oneValueArgs NAME)
    set(multiValueArgs SOURCES LIBRARIES INCLUDE_DIRECTORIES WRAP)
    cmake_parse_arguments(ut "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(target_name "${ut_NAME}_ut")

    list(REMOVE_DUPLICATES ut_SOURCES)
    list(REMOVE_DUPLICATES ut_WRAP)

    add_executable(${target_name} ${ut_SOURCES})
    target_link_libraries(${target_name} PRIVATE ${ut_LIBRARIES} unity fff)
    target_include_directories(${target_name} PRIVATE ${ut_INCLUDE_DIRECTORIES} ${EXTRA_INCLUDE})
    set_target_properties(${target_name} PROPERTIES C_STANDARD 11)

    if(ut_WRAP AND NOT CMAKE_COMPILER_IS_GNUCC)
        message(FATAL_ERROR "WRAP can only be used with GCC")
    endif()

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

cmake_path(SET ETRIPATOR_SRC_DIR NORMALIZE "${PROJECT_SOURCE_DIR}/src")
cmake_path(SET ETRIPATOR_INC_DIR NORMALIZE "${PROJECT_SOURCE_DIR}/include")

add_unit_test(
    NAME utils
    SOURCES 
        utils.c
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME string
    SOURCES 
        string.c
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME string_view
    SOURCES 
        string_view.c
        ${ETRIPATOR_SRC_DIR}/string_view.c
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME data
    SOURCES 
        data/buffer.c
        data.c
        ${ETRIPATOR_SRC_DIR}/data/buffer.c
        ${ETRIPATOR_SRC_DIR}/data.c
        ${ETRIPATOR_SRC_DIR}/string_view.c
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME memory
    SOURCES 
        memory.c 
        ${ETRIPATOR_SRC_DIR}/memory.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME memory_map
    SOURCES 
        memory_map.c 
        ${ETRIPATOR_SRC_DIR}/memory_map.c
        ${ETRIPATOR_SRC_DIR}/memory.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME rom
    SOURCES 
        rom.c 
        ${ETRIPATOR_SRC_DIR}/rom.c
        ${ETRIPATOR_SRC_DIR}/memory_map.c
        ${ETRIPATOR_SRC_DIR}/memory.c
        ${ETRIPATOR_SRC_DIR}/data.c
        ${ETRIPATOR_SRC_DIR}/data/buffer.c
        ${ETRIPATOR_SRC_DIR}/string_view.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME cd
    SOURCES 
        cd.c 
        ${ETRIPATOR_SRC_DIR}/cd.c
        ${ETRIPATOR_SRC_DIR}/memory_map.c
        ${ETRIPATOR_SRC_DIR}/memory.c
        ${ETRIPATOR_SRC_DIR}/data.c
        ${ETRIPATOR_SRC_DIR}/data/buffer.c
        ${ETRIPATOR_SRC_DIR}/string_view.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME message
    SOURCES 
        message.c 
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME label
    SOURCES 
        label.c 
        ${ETRIPATOR_SRC_DIR}/label.c
        ${ETRIPATOR_SRC_DIR}/label/load.c
        ${ETRIPATOR_SRC_DIR}/label/save.c
        ${ETRIPATOR_SRC_DIR}/string.c
        ${ETRIPATOR_SRC_DIR}/string_view.c
        ${ETRIPATOR_SRC_DIR}/data.c
        ${ETRIPATOR_SRC_DIR}/data/buffer.c
        ${ETRIPATOR_SRC_DIR}/json_helpers.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME comment
    SOURCES 
        comment.c 
        ${ETRIPATOR_SRC_DIR}/comment.c
        ${ETRIPATOR_SRC_DIR}/string.c
        ${ETRIPATOR_SRC_DIR}/string_view.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

add_unit_test(
    NAME json_helpers
    SOURCES 
        json_helpers.c 
        ${ETRIPATOR_SRC_DIR}/json_helpers.c
        ${ETRIPATOR_SRC_DIR}/string.c
        ${ETRIPATOR_SRC_DIR}/string_view.c
        ${ETRIPATOR_SRC_DIR}/data.c
        ${ETRIPATOR_SRC_DIR}/data/buffer.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES
        ${ETRIPATOR_INC_DIR}
)

#[[
add_unit_test(
    NAME disasm
    SOURCES 
        disasm.c
        ${ETRIPATOR_SRC_DIR}/string.c
        ${ETRIPATOR_SRC_DIR}/label.c
        ${ETRIPATOR_SRC_DIR}/memory_map.c
        ${ETRIPATOR_SRC_DIR}/memory.c
        ${ETRIPATOR_SRC_DIR}/data.c
        ${ETRIPATOR_SRC_DIR}/data/buffer.c
        ${ETRIPATOR_SRC_DIR}/rom.c
    INCLUDE_DIRECTORIES
        ${PROJECT_SOURCE_DIR}
        ${ETRIPATOR_INC_DIR}
)
#]]

#[[

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
        ${PROJECT_SOURCE_DIR}/section/load.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/jsonhelpers.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME ipl
    SOURCES 
        ipl.c 
        ${PROJECT_SOURCE_DIR}/section.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME output
    SOURCES
        output.c
        ${PROJECT_SOURCE_DIR}/output.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)

add_unit_test(
    NAME decode
    SOURCES
        decode.c
        ${PROJECT_SOURCE_DIR}/decode.c
        ${PROJECT_SOURCE_DIR}/output.c
        ${PROJECT_SOURCE_DIR}/opcodes.c
        ${PROJECT_SOURCE_DIR}/memory.c
        ${PROJECT_SOURCE_DIR}/memory_map.c
        ${PROJECT_SOURCE_DIR}/section.c
        ${PROJECT_SOURCE_DIR}/label.c
        ${PROJECT_SOURCE_DIR}/comment.c
        ${PROJECT_SOURCE_DIR}/message.c
        ${PROJECT_SOURCE_DIR}/message/console.c
    LIBRARIES cwalk jansson
    INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}
)
#]]