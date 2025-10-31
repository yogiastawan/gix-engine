#ifndef __GIX_LOG_H__
#define __GIX_LOG_H__

#include <SDL3/SDL.h>

#ifdef __cpluplus
extern "C" {
#endif

#ifdef BUILD_DEBUG
/**
 * @brief Show log level error
 *
 * @param msg Format message
 */
#define gix_log_error(msg...)                        \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg); \
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError())

/**
 * @brief Show log level info
 *
 * @param msg Format message
 */
#define gix_log(msg...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, msg)

/**
 * @brief Show log level debug
 *
 * @param msg Format message
 */
#define gix_log_debug(msg...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, msg)
#else
#define gix_log_error(msg...)
#define gix_log(msg...)
#define gix_log_debug(msg...)
#endif

#ifdef __cpluplus
}
#endif

#endif /* __GIX_LOG_H__ */
