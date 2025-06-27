project(examples)

set(CMD_COMPILE_SHADER
    "${CMD_COMPILE_SHADER} && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/basic_triangle/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}\
    && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/vertex_buffer/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}\
    && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/index_vertex_buffer/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}\
    && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/uniform/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}\
    && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/drawobjects/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}\
    && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/cube/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}\
    && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/camera_move/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}\
    && COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/instancing/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
    PARENT_SCOPE)

# example for gix-engine
add_executable(clear_screen clear_screen/main.c clear_screen/scene.c)
target_link_libraries(clear_screen gix-engine)

# example for gix-engine with shader
add_executable(basic_triangle basic_triangle/main.c
    basic_triangle/scene.c
)
target_link_libraries(basic_triangle gix-engine)
# add_custom_target(compile_shader_basic_triangle
#     COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/basic_triangle/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
#     VERBATIM)
add_dependencies(basic_triangle compile_shader)

# example for gix-engine with vertex buffer
add_executable(vertex_buffer vertex_buffer/main.c
    vertex_buffer/scene.c
)
target_link_libraries(vertex_buffer gix-engine)
# add_custom_target(compile_shader_vertex_buffer
#     COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/vertex_buffer/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
#     VERBATIM)
add_dependencies(vertex_buffer compile_shader)

#example for gix-engine with vertex buffer and index buffer
add_executable(index_vertex_buffer index_vertex_buffer/main.c
    index_vertex_buffer/scene.c
)
target_link_libraries(index_vertex_buffer gix-engine)
# add_custom_target(compile_shader_index_vertex_buffer
#     COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/index_vertex_buffer/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
#     VERBATIM)
add_dependencies(index_vertex_buffer compile_shader)

#example for gix-engine with uniform buffer
add_executable(uniform uniform/main.c
    uniform/scene.c
)
target_link_libraries(uniform gix-engine)
# add_custom_target(compile_shader_uniform
#     COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/uniform/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
#     VERBATIM)
add_dependencies(uniform compile_shader)

# example for gix-engine with drawobjects
add_executable(drawobjects drawobjects/main.c
    drawobjects/scene.c
)
target_link_libraries(drawobjects gix-engine)
# add_custom_target(compile_shader_drawobjects
#     COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/drawobjects/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
#     VERBATIM)
add_dependencies(drawobjects compile_shader)

# example for gix-engine with cube
add_executable(cube cube/main.c
    cube/scene.c)
target_link_libraries(cube gix-engine)
# add_custom_target(compile_shader_cube
#     COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/cube/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
#     VERBATIM)
add_dependencies(cube compile_shader)
# example for gix-engine with camera move
add_executable(camera_move camera_move/main.c
    camera_move/scene.c camera_move/cm_scene.c)
target_link_libraries(camera_move gix-engine)
# add_custom_target(compile_shader_camera_move
#     COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/camera_move/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
#     VERBATIM)
add_dependencies(camera_move compile_shader)

#example for gix-engine with camera move
add_executable(instancing instancing/main.c
    instancing/scene.c)
target_link_libraries(instancing gix-engine)
add_custom_target(compile_shader_instancing
    COMMAND /bin/sh -c "set -e; COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/instancing/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${SCRIPT_COMPILE_SHADER_FILE}"
    VERBATIM)
add_dependencies(instancing compile_shader)
