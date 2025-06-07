cmake_minimum_required(VERSION 3.10.0)

find_library(MATH m)

set(srcs gix_app_engine.c
    gix_shader.c
    gix_engine_version.c)


add_library(${PROJECT_NAME} ${BUILD_LIBRARY_TYPE} ${srcs})
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 cglm json-c ${MATH})

add_custom_target(compile_shader
    ALL
    COMMAND COMMAND /bin/sh -c 'COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/shader DST=${GIX_ENGINE_SHADER_DIR_OUT} ${SCRIPT_COMPILE_SHADER_FILE}')
add_dependencies(${PROJECT_NAME} compile_shader)


