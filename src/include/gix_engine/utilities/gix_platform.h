/**
 * Used to detect platform and set platform-specific macros.
 * Example to define load file function in desktop and mobile platform (Android
 * assets).
 */

#ifndef __GIX_PLATFORM_H__
#define __GIX_PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define GIX_PLATFORM_WINDOWS
#ifdef _WIN64
#define GIX_PLATFORM_WINDOWS_64
#else
#define GIX_PLATFORM_WINDOWS_32
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#define GIX_PLATFORM_IOS
#else
#define GIX_PLATFORM_MACOS
#endif
#elif __ANDROID__
#define GIX_PLATFORM_ANDROID
#elif __linux__
#define GIX_PLATFORM_LINUX
#elif __unix__
#define GIX_PLATFORM_UNIX
#else
#define GIX_PLATFORM_UNKNOWN
#endif

#ifdef __cplusplus
}
#endif

#endif /* __GIX_PLATFORM_H__ */