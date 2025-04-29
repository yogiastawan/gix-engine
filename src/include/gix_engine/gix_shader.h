#ifndef __GIX_SHADER_H__
#define __GIX_SHADER_H__

#include <SDL3/SDL.h>
#include <gix_engine/gix_app_engine.h>

#ifdef __cpluplus
extern C {
#endif

    SDL_GPUShader* gix_load_shader(GixApp * app,
                                   const char* shader_file,
                                   SDL_GPUShaderStage shader_stage,
                                   Uint32 sampler_count,
                                   Uint32 uniform_buffer_count,
                                   Uint32 storage_buffer_count,
                                   Uint32 storage_texture_count);

#ifdef __cpluplus
}
#endif

#endif /* __GIX_SHADER_H__ */
