
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

set(etripator_SRC
    string.c
    string_view.c
    message.c 
    memory.c
    memory_map.c
    rom.c
    cd.c
    comment.c
    json_helpers.c
   #    disasm.c
)

add_library(etripator STATIC ${etripator_SRC} $<TARGET_OBJECTS:data> $<TARGET_OBJECTS:label>)
set_target_properties(etripator PROPERTIES C_STANDARD 23)
target_link_libraries(etripator PUBLIC jansson cwalk)
target_include_directories(etripator 
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/externals>
)
