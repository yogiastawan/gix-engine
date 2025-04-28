cmake_minimum_required(VERSION 3.10.0)

project(gix-engine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

option(BUILD_EXAMPLE "Build examples" ON)
option(BUILD_STATIC "Build static library" ON)

message(STATUS "BUILD_EXAMPLES: ${BUILD_EXAMPLE}")
message(STATUS "BUILD_STATIC: ${BUILD_STATIC}")

# add libs
add_subdirectory(libs/SDL3-3.2.10)
add_subdirectory(libs/cglm-0.9.6)


if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/Wall)
    add_compile_options(/Wextra)
    add_compile_options(/Werror)
else()
    add_compile_options(-Wall)
    add_compile_options(-Wextra)
    add_compile_options(-Werror)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(BUILD_DEBUG)
endif()

add_subdirectory(src)
target_include_directories(${PROJECT_NAME} PRIVATE src/include)

if(BUILD_EXAMPLE)
    add_subdirectory(examples)
endif(BUILD_EXAMPLE)

set(BUILD_TYPE)

if(BUILD_STATIC)
    set(BUILD_TYPE, STATIC)
else()
    set(BUILD_TYPE, SHARED)

endif(BUILD_STATIC)



include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

