cmake_minimum_required(VERSION 3.21.0)

project(gix-engine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

option(BUILD_EXAMPLE "Build examples" ON)
option(BUILD_STATIC "Build static library" ON)

message(STATUS "BUILD_EXAMPLES: ${BUILD_EXAMPLE}")
message(STATUS "BUILD_STATIC: ${BUILD_STATIC}")

if(NOT DEFINED SDL3_DIR)
    set(SDL3_DIR libs/SDL3-3.2.10)
endif()

if(NOT DEFINED CGLM_DIR)
    set(CGLM_DIR libs/cglm-0.9.6)
endif()

if(NOT DEFINED JSON_C_DIR)
    set(JSON_C_DIR libs/json-c-0.18)
endif()

# add libs
add_subdirectory(${SDL3_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${CGLM_DIR} EXCLUDE_FROM_ALL)
add_subdirectory(${JSON_C_DIR} EXCLUDE_FROM_ALL)

#version
add_compile_definitions(GIX_ENGINE_MAJOR_VERSION=${PROJECT_VERSION_MAJOR})
add_compile_definitions(GIX_ENGINE_MINOR_VERSION=${PROJECT_VERSION_MINOR})
add_compile_definitions(GIX_ENGINE_PATCH_VERSION=${PROJECT_VERSION_PATCH})
add_compile_definitions(GIX_ENGINE_VERSION="${PROJECT_VERSION}")
# system shader build output
if(NOT DEFINED ENGINE_SHADER_OUT)
    set(GIX_ENGINE_SHADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/shader)
    add_compile_definitions(GIX_ENGINE_SHADER_DIR="${CMAKE_BINARY_DIR}/gix_engine_shader")
else()
    set(GIX_ENGINE_SHADER_DIR ENGINE_SHADER_OUT)
    add_compile_definitions(GIX_ENGINE_SHADER_DIR="${ENGINE_SHADER_OUT}")
endif()

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

# Backend Shader format choose
if(NOT DEFINED SHADER_FORMAT)
    add_compile_definitions(GIX_VULKAN)
else()
    add_compile_definitions(${SHADER_FORMAT})
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(examples)
endif(BUILD_EXAMPLE)

# library type
if(BUILD_STATIC)
    message("Configure to STATIC library. To create SHARED Library use: -DBUILD_STATIC=OFF")
    set(BUILD_LIBRARY_TYPE STATIC)
else()
    message("Configure to SHARED library")

    set(BUILD_LIBRARY_TYPE SHARED)

endif(BUILD_STATIC)

# shadercross path
if(NOT DEFINED SDL_SHADER_CROSS)
    message("Please set file SDLShadercross file using -DSDL_SHADER_CROSS=\"path\"")
    set(SDL_SHADER_CROSS /home/gihexdev00/Game-Dev/Program/SDL3_shadercross-3.0.0-linux-x64/bin/shadercross)
endif()

add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PRIVATE src/include)

include(CTest)
enable_testing()


install(DIRECTORY
    src DESTINATION .)
install(DIRECTORY
    examples DESTINATION .)

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

