#!/bin/sh

# $1 is buld type: Debug or Release

CC=/usr/bin/clang
CXX=/usr/bin/clang++
BUILD_TYPE=Debug

SOURCE=.

BUILD_OUTPUT=./build
BUILD_EXAMPLES=ON
# Change this as your libs path
SDL3=libs/SDL3-3.2.10
cglm=libs/cglm-0.9.6
json_c=libs/json-c-0.18

SHADERCROSS=/home/gihexdev00/Game-Dev/Program/SDL3_shadercross-3.0.0-linux-x64/bin/shadercross 

BUILD_EXAMPLES=ON

if [[ $1 == "Release" ]]; then
  BUILD_TYPE=Release
  BUILD_EXAMPLES=OFF
fi

mkdir -p ./build

cmake -DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=${CC} -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/clang++-18 -DBUILD_EXAMPLES=${BUILD_EXAMPLES} -DUSE_COMPILED_SHADER=OFF -DSDL3_DIR=${SDL3} -DCGLM_DIR=${cglm} -DJSON_C_DIR=${json_c} -DSDL_SHADER_CROSS=${SHADERCROSS} --no-warn-unused-cli -S${SOURCE} -B${BUILD_OUTPUT} 
