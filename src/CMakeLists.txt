cmake_minimum_required(VERSION 3.10.0)

find_library(MATH m)

set(srcs gix_app_engine.c
    gix_shader.c
    gix_engine_version.c)

add_custom_target(compile_shader
    ALL
    COMMAND COMMAND /bin/sh -c 'COMPILER=${SDL_SHADER_CROSS} SRC=${CMAKE_CURRENT_SOURCE_DIR}/shader DST=${GIX_ENGINE_SHADER_DIR_OUT} ${SCRIPT_COMPILE_SHADER_FILE}')

if(BUILD_STATIC)
    add_library(${PROJECT_NAME}-static STATIC ${srcs})
    target_link_libraries(${PROJECT_NAME}-static SDL3::SDL3 cglm json-c ${MATH})
    set_target_properties(${PROJECT_NAME}-static
        PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
        OUTPUT_NAME ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME}-static compile_shader)
endif()
if(BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${srcs})
    target_link_libraries(${PROJECT_NAME} SDL3::SDL3 cglm json-c ${MATH})
    set_target_properties(${PROJECT_NAME}
        PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}
        OUTPUT_NAME ${PROJECT_NAME})
    add_dependencies(${PROJECT_NAME} compile_shader)
endif()
