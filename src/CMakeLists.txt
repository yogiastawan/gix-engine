cmake_minimum_required(VERSION 3.10.0)

find_library(MATH m)

set(srcs gix_app_engine.c
    gix_shader.c
    gix_engine_version.c)


add_library(${PROJECT_NAME} ${BUILD_LIBRARY_TYPE} ${srcs})
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 cglm json-c ${MATH})


