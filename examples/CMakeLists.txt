project(examples)
include_directories(../src/include)

if(NOT DEFINED SDL_SHADER_CROSS)
    set(SDL_SHADER_CROSS /home/gihexdev00/Game-Dev/Program/SDL3_shadercross-3.0.0-linux-x64/bin/shadercross)
endif()

# example for gix-engine
add_executable(clear_screen clear_screen/main.c clear_screen/scene.c)
target_link_libraries(clear_screen gix-engine)

# example for gix-engine with shader
add_executable(basic_triangle basic_triangle/main.c
    basic_triangle/scene.c
)
target_link_libraries(basic_triangle gix-engine)
add_custom_target(compile_shader_basic_triangle
    ALL
    COMMAND COMMAND /bin/sh -c 'COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/basic_triangle/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${CMAKE_CURRENT_SOURCE_DIR}/compile-shader.sh')
add_dependencies(basic_triangle compile_shader_basic_triangle)

# example for gix-engine with vertex buffer
add_executable(vertex_buffer vertex_buffer/main.c
    vertex_buffer/scene.c
)
target_link_libraries(vertex_buffer gix-engine)
add_custom_target(compile_shader_vertex_buffer
    ALL
    COMMAND COMMAND /bin/sh -c 'COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/vertex_buffer/shader DST=${CMAKE_CURRENT_BINARY_DIR}/shader ${CMAKE_CURRENT_SOURCE_DIR}/compile-shader.sh')
add_dependencies(vertex_buffer compile_shader_vertex_buffer)
