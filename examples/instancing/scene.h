#ifndef __SCENE_H__
#define __SCENE_H__

#define GIX_SCENE_DEFAULT_NUMB_GRAPHIC_PIPELINE 1
#define GIX_SCENE_DEFAULT_NUMB_COMPUTE_PIPELINE 0

#include <gix_engine/gix_engine.h>

#ifdef __cplusplus
extern "C" {
#endif

GixScene* create_scene(GixApp* app);

#ifdef __cplusplus
}
#endif

#endif /* __SCENE_H__ */
