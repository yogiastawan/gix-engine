cmake_minimum_required(VERSION 3.21.0)

project(gix-engine VERSION 0.1.0 LANGUAGES C)

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(JSON_C_BUGREPORT "json-c@googlegroups.com")
set(CPACK_SOURCE_IGNORE_FILES
    ${PROJECT_SOURCE_DIR}/build
    ${PROJECT_SOURCE_DIR}/cmake-build-debug
    ${PROJECT_SOURCE_DIR}/pack
    ${PROJECT_SOURCE_DIR}/.idea
    ${PROJECT_SOURCE_DIR}/.DS_Store
    ${PROJECT_SOURCE_DIR}/.git
    ${PROJECT_SOURCE_DIR}/.vscode)

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
add_subdirectory(${SDL3_DIR})
add_subdirectory(${CGLM_DIR})
add_subdirectory(${JSON_C_DIR})


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

set(BUILD_LIBRARY_TYPE)

if(BUILD_STATIC)
    message("Configure to static library")
    set(BUILD_LIBRARY_TYPE STATIC)
else()
    message("Configure to shared library")

    set(BUILD_LIBRARY_TYPE SHARED)

endif(BUILD_STATIC)

add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PRIVATE src/include)







include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

