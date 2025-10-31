#ifndef __GIX_ENGINE_UTIL_H__
#define __GIX_ENGINE_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create array with type
 *
 * @param t type
 * @param arr Array value
 */
#define gix_array(t, arr...) (t) arr

#ifdef __cplusplus
}
#endif

#endif /* __GIX_ENGINE_UTIL_H__ */
