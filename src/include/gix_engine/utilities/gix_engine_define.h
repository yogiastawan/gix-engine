#ifndef __GIX_ENGINE_DEFINE_H__
#define __GIX_ENGINE_DEFINE_H__

#include <gix_engine/utilities/gix_engine_platform.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GIX_DEFINE_TYPES
#define GIX_DEFINE_TYPES

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef uint8_t bool8;
typedef uint32_t bool32;

typedef size_t usize;

#endif

#ifndef GEAPI

#ifdef GIX_ENGINE_PLATFORM_WINDOWS
#define GEAPI __declspec(dllexport)
#else
#define GEAPI
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* __GIX_ENGINE_DEFINE_H__ */