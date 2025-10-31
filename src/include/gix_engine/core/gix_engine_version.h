#ifndef __GIX_ENGINE_VERSION_H__
#define __GIX_ENGINE_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GIX_ENGINE_MAJOR_VERSION
#define GIX_ENGINE_MAJOR_VERSION \
    Error:                       \
    Major version not defined
#endif
#ifndef GIX_ENGINE_MINOR_VERSION
#define GIX_ENGINE_MAJOR_VERSION \
    Error:                       \
    Minor version not defined
#endif

#ifndef GIX_ENGINE_PATCH_VERSION
#define GIX_ENGINE_MAJOR_VERSION \
    Error:                       \
    Patch version not defined
#endif

#ifndef GIX_ENGINE_VERSION
#define GIX_ENGINE_VERSION \
    Error:                 \
    Version not defined
#endif

#define GIX_ENGINE_VERSION_NUM                                            \
    ((GIX_ENGINE_MAJOR_VERSION << 16) | (GIX_ENGINE_MINOR_VERSION << 8) | \
     GIX_ENGINE_PATCH_VERSION)

/**
 * @brief Get version of Gix Engine in string.
 *
 * @return const char*
 */
const char* gix_engine_version(void);

/**
 * @brief Get version of Gix Engine in number.
 *
 * @return int
 */
int gix_engine_version_num(void);

#ifdef __cplusplus
}
#endif

#endif /* __GIX_ENGINE_VERSION_H__ */
