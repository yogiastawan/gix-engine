#ifndef __LOG_H__
#define __LOG_H__

#include <SDL3/SDL.h>

#ifdef __cpluplus
extern "C" {
#endif

#ifdef BUILD_DEBUG
#define gix_log_error(msg) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s. %s", msg, SDL_GetError())
#define gix_log(msg...) SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, msg);
#define gix_log_debug(msg...) SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, msg);
#else
#define gix_log_error(msg)
#define gix_log(msg...)
#define gix_log_debug(msg...)
#endif

#ifdef __cpluplus
}
#endif

#endif /* __LOG_H__ */
