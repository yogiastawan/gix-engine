#ifndef __GIX_ENGINE_SHADER_H__
#define __GIX_ENGINE_SHADER_H__

#include <SDL3/SDL.h>
#include <gix_engine/core/gix_app_engine.h>

#ifdef __cpluplus
extern "C" {
#endif

GEAPI SDL_GPUShader* gix_load_shader(SDL_GPUDevice* device,
                                     const char* shader_file,
                                     const char* info_file,
                                     SDL_GPUShaderStage shader_stage);

#ifdef __cpluplus
}
#endif

#endif /* __GIX_ENGINE_SHADER_H__ */
