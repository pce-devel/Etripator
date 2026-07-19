
add_library(data OBJECT 
    data.c
    data/buffer.c
    data/file.c
)
set_target_properties(data PROPERTIES C_STANDARD 23)
target_include_directories(data 
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

add_library(label OBJECT 
    label.c
    label/load.c
    label/save.c
)
set_target_properties(label PROPERTIES C_STANDARD 23)
target_link_libraries(label PUBLIC jansson cwalk)
target_include_directories(label 
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

add_library(comment OBJECT 
    comment.c
    comment/load.c
    comment/save.c
)
set_target_properties(comment PROPERTIES C_STANDARD 23)
target_link_libraries(comment PUBLIC jansson cwalk)
target_include_directories(comment 
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

set(etripator_SRC
    string.c
    string_view.c
    json_helpers.c
    message.c 
    memory.c
    memory_map.c
    rom.c
    cd.c
    section.c
#    disasm.c
)

add_library(etripator STATIC ${etripator_SRC} $<TARGET_OBJECTS:data> $<TARGET_OBJECTS:label> $<TARGET_OBJECTS:comment>)
set_target_properties(etripator PROPERTIES C_STANDARD 23)
target_link_libraries(etripator PUBLIC jansson cwalk)
target_include_directories(etripator 
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/externals>
)
