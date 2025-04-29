cmake_minimum_required(VERSION 3.10.0)

set(srcs gix_app_engine.c
    gix_shader.c)


add_library(${PROJECT_NAME} ${BUILD_TYPE} ${srcs})
target_link_libraries(${PROJECT_NAME} SDL3::SDL3 cglm)


