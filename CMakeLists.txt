cmake_minimum_required(VERSION 3.21.0)

project(gix-engine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_STATIC "Build static library" ON)
option(BUILD_SHARED "Build static library" ON)
option(USE_COMPILED_SHADER "Use system compiled shader" ON)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_EXAMPLES: ${BUILD_EXAMPLES}")
message(STATUS "BUILD_STATIC: ${BUILD_STATIC}")
message(STATUS "BUILD_SHARED: ${BUILD_SHARED}")
message(STATUS "USE_COMPILED_SHADER: ${USE_COMPILED_SHADER}")
if(NOT USE_COMPILED_SHADER)
    if(NOT DEFINED GIX_ENGINE_SHADER_DIR_OUT)
        message(STATUS "GIX_ENGINE_SHADER_DIR_OUT: Default")
    else()
        message(STATUS "GIX_ENGINE_SHADER_DIR_OUT: ${GIX_ENGINE_SHADER_DIR_OUT}")
    endif()

else()
    if(NOT DEFINED GIX_ENGINE_SHADER_COMPILED_DIR)
        message(STATUS "GIX_ENGINE_SHADER_COMPILED_DIR: Default")
    else()
        message(STATUS "GIX_ENGINE_SHADER_COMPILED_DIR: ${GIX_ENGINE_SHADER_COMPILED_DIR}")
    endif()
endif()

if(NOT DEFINED SHADER_FORMAT)
    message(STATUS "SHADER_FORMAT: Default")
else()
    message(STATUS "SHADER_FORMAT: ${SHADER_FORMAT}")
endif()
message(STATUS "SDL_SHADER_CROSS: ${SDL_SHADER_CROSS}")
message(STATUS "SDL3_DIR: ${SDL3_DIR}")
message(STATUS "CGLM_DIR: ${CGLM_DIR}")
message(STATUS "JSON_C_DIR: ${JSON_C_DIR}")

if(NOT BUILD_STATIC AND NOT BUILD_SHARED)
    message(FATAL_ERROR "BUILD_STATIC and BUILD_SHARED set to off. Please set to ON one or both variables")
endif()

if(NOT DEFINED SDL3_DIR)
    message(FATAL_ERROR "SDL3_DIR not defined. Set this with location of libs SDL3.")
endif()

if(NOT DEFINED CGLM_DIR)
    message(FATAL_ERROR "CGLM_DIR not defined. Set this with location of libs cglm.")

endif()

if(NOT DEFINED JSON_C_DIR)
    message(FATAL_ERROR "JSON_C_DIR not defined. Set this with location of libs json-c.")

endif()

set(_saved_CMAKE_MESSAGE_LOG_LEVEL ${CMAKE_MESSAGE_LOG_LEVEL})
set(CMAKE_MESSAGE_LOG_LEVEL NOTICE)
add_subdirectory(${SDL3_DIR} EXCLUDE_FROM_ALL)
set(CGLM_STATIC ON)
set(CGLM_SHARED OFF)
add_subdirectory(${CGLM_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${JSON_C_DIR} EXCLUDE_FROM_ALL)
set(CMAKE_MESSAGE_LOG_LEVEL ${_saved_CMAKE_MESSAGE_LOG_LEVEL})


#version
add_compile_definitions(GIX_ENGINE_MAJOR_VERSION=${PROJECT_VERSION_MAJOR})
add_compile_definitions(GIX_ENGINE_MINOR_VERSION=${PROJECT_VERSION_MINOR})
add_compile_definitions(GIX_ENGINE_PATCH_VERSION=${PROJECT_VERSION_PATCH})
add_compile_definitions(GIX_ENGINE_VERSION="${PROJECT_VERSION}")


if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/Wall)
    add_compile_options(/Wextra)
    # add_compile_options(/Werror)
else()
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    # add_compile_options(-Werror)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(BUILD_DEBUG)
endif()

# system shader build output
if(NOT USE_COMPILED_SHADER)
    message(STATUS "Using precompiled shader file.")
    if(NOT DEFINED GIX_ENGINE_SHADER_DIR_OUT)
        set(GIX_ENGINE_SHADER_DIR_OUT ${CMAKE_CURRENT_BINARY_DIR}/gix_engine_shader)
        message(STATUS "GIX_ENGINE_SHADER_DIR_OUT not defined. Default: system shader file will be compiled to ${GIX_ENGINE_SHADER_DIR_OUT}")
    endif()
    add_compile_definitions(GIX_ENGINE_SHADER_DIR="${GIX_ENGINE_SHADER_DIR_OUT}")
else()
    message(STATUS "Using compiled shader file.")
    if(NOT DEFINED GIX_ENGINE_SHADER_COMPILED_DIR)
        set(GIX_ENGINE_SHADER_COMPILED_DIR ${PROJECT_SOURCE_DIR}/gix_engine_shader)
        message(STATUS "GIX_ENGINE_SHADER_DIR not defined. Using default GIX_ENGINE_SHADER_DIR in ${GIX_ENGINE_SHADER_COMPILED_DIR}")
    endif()
    add_compile_definitions(GIX_ENGINE_SHADER_DIR="${GIX_ENGINE_SHADER_COMPILED_DIR}")
endif()

# Backend Shader format choose
if(NOT DEFINED SHADER_FORMAT)
    message(STATUS "SHADER_FORMAT not defined. Default: GIX_VULKAN. Option: GIX_VULKAN, GIX_MSL, GIX_DXIL.")
    add_compile_definitions(GIX_VULKAN)
else()
    add_compile_definitions(${SHADER_FORMAT})
endif()

# shadercross path
if(NOT USE_COMPILED_SHADER)
    if(NOT DEFINED SDL_SHADER_CROSS)
        message(FATAL_ERROR "SDL_SHADER_CROSS is not defined. Please set path of SDLShadercross.")
    endif()

    set(SCRIPT_COMPILE_SHADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/compile-shader.sh)
endif()

set(CMD_COMPILE_SHADER)

# library type
add_subdirectory(src)
if(BUILD_STATIC)
    target_include_directories(${PROJECT_NAME}-static PUBLIC ${PROJECT_SOURCE_DIR}/src/include)
endif()
if(BUILD_SHARED)
    target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/include)
endif()

if(BUILD_EXAMPLES)
    if(NOT DEFINED SDL_SHADER_CROSS)
        message(FATAL_ERROR "SDL_SHADER_CROSS is not defined. Please set path of SDLShadercross.")
    endif()
    set(SCRIPT_COMPILE_SHADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/compile-shader.sh)

    add_subdirectory(examples)
endif(BUILD_EXAMPLES)

add_custom_target(compile_shader
    ALL
    COMMAND sh -c "set -e; ${CMD_COMPILE_SHADER}"
    VERBATIM)

include(CTest)
enable_testing()

# PACKAGING
install(FILES
    ${PROJECT_SOURCE_DIR}/compile-shader.sh
    ${PROJECT_SOURCE_DIR}/CMakeLists.txt
    DESTINATION .
)

install(DIRECTORY
    ${PROJECT_SOURCE_DIR}/src DESTINATION .)
install(DIRECTORY
    ${PROJECT_SOURCE_DIR}/examples DESTINATION .)
install(DIRECTORY
    ${PROJECT_BINARY_DIR}/gix_engine_shader DESTINATION shader)

# install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}-static
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
# )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP") # Or "NSIS", "DEB", etc.
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Game template using SDL3 GPU for GIX")
# set(CPACK_PACKAGE_NAME "${PROJECT_NAME}-${PACKAGE_FILE_NAME_PROC}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-v${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
# set(CPACK_ARCHIVE_FILE_NAME "${PROJECT_NAME}-v${PROJECT_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_SOURCE_DIR}/build
    ${PROJECT_SOURCE_DIR}/cmake-build-debug
    ${PROJECT_SOURCE_DIR}/pack
    ${PROJECT_SOURCE_DIR}/libs
    ${PROJECT_SOURCE_DIR}/.idea
    ${PROJECT_SOURCE_DIR}/.DS_Store
    ${PROJECT_SOURCE_DIR}/.git
    ${PROJECT_SOURCE_DIR}/.gitignore
    ${PROJECT_SOURCE_DIR}/..clang-format
    ${PROJECT_SOURCE_DIR}/.TODO.md
    ${PROJECT_SOURCE_DIR}/.vscode)
include(CPack)

