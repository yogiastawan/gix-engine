# Intro
gix-engine is just simple game template using SDL3 GPU.

# Prerequisites
Libraries that used in gix-engine:
1. [SDL3](https://www.libsdl.org/)
2. [cglm](https://github.com/recp/cglm)
3. [json-c](https://github.com/json-c/json-c)

# How to use
To use gix-engine we need define some option and variable. Below is list of options and variable we can set.

| Name                  | Default Value                | Desc                                                                                                                            |
| --------------------- | ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------- |
| BUILD_EXAMPLES        | OFF                          | Set ON to create examples                                                                                                       |
| BUILD_STATIC          | ON                           | Create static library                                                                                                           |
| BUILD_SHARED          | ON                           | Create shared library                                                                                                           |
| USE_COMPILED_SHADER   | ON                           | Use compiled shader. Set OFF to compile shader when build library                                                               |
| SHADER_FORMAT         | GIX_VULKAN                   | Format shader that specific to platform. Value: GIX_VULKAN (for Linux,Android), GIX_MSL (for Mac, Ios), GIX_DXIL (for Windows). |
| SDL_SHADER_CROSS      |                              | Set path of SDL3 Shadercross. Note: Must be set if BUILD_EXAMPLES=ON or USE_COMPILED_SHADER=ON                                  |
| SDL3_DIR              |                              | Path location of SDL3. Note: Must be set                                                                                        |
| CGLM_DIR              |                              | Path location of cglm. Note: Must be set                                                                                        |
| JSON_C_DIR            |                              | Path location of json-c. Note: Must be set                                                                                      |
| GIX_ENGINE_SHADER_DIR | gix-engine/gix_engine_shader | Path of system shader directory location                                                                                        |

## Build gix-engine
1. Configure gix-engine to folder build
    ```sh
    cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DBUILD_SHARED=OFF -DBUILD_EXAMPLES=ON -DUSE_COMPILED_SHADER=OFF -DSDL3_DIR=libs/SDL3-3.2.10 -DCGLM_DIR=libs/cglm-0.9.6 -DJSON_C_DIR=libs/json-c-0.18 -DSDL_SHADER_CROSS=bin/shadercross -S./ -B./build
    ```
2. Build gix-engine
   ```sh
   cmake --build ./build --config Debug --target all
   ```
3. Copy compiled library in `./build` to your project. Also copy the header file in `src/include/gix_engine` to your project.

## Add to Cmake project
1. On your project cmake add variable definition.
   ```cmake
   set(BUILD_SHARED OFF)
   set(BUILD_EXAMPLES ON)
   set(USE_COMPILED_SHADER OFF)
   set(SDL3_DIR libs/SDL3-3.2.10)
   set(CGLM_DIR libs/cglm-0.9.6)
   set(JSON_C_DIR libs/json-c-0.18)
   set(SDL_SHADER_CROSS bin/shadercross)

   add_subdirectory(path/gix-engine)

   # your project target project
   # add_executable(your_roject srcs.c)

   # link static library
   target_link_libraries(your_roject gix-engine-static)
   # use this to link shared library instead
   # target_link_libraries(your_roject gix-engine)

   ```
2. Configure and build your project