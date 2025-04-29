#include <gix_engine/gix_checker.h>
#include <gix_engine/gix_log.h>
#include <gix_engine/gix_shader.h>

SDL_GPUShader* gix_load_shader(SDL_GPUDevice* device,
                               const char* shader_file,
                               SDL_GPUShaderStage shader_stage,
                               Uint32 sampler_count,
                               Uint32 uniform_buffer_count,
                               Uint32 storage_buffer_count,
                               Uint32 storage_texture_count) {
    SDL_GPUShaderFormat backend_format = SDL_GetGPUShaderFormats(device);
    const char* entry_point = NULL;
#ifdef GIX_VULKAN
    entry_point = "main";
#elif GIX_MSL
    entry_point = "main0";
#elif GX_DXIL
    entry_point = "main";
#else
    backend_format = SDL_GPU_SHADERFORMAT_INVALID;
    entry_point = NULL;
#endif

    gix_if_null_exit(entry_point, gix_log("Do not supported shader format"));
    gix_if_exit(!backend_format, gix_log("Invalid shader format. Choose format by set SHADER_FORMAT to GIX_VULKAN, GIX_MSL, or GIX_DXIL"));

    size_t shader_code_size = 0;
    void* shader_code = SDL_LoadFile(shader_file, &shader_code_size);

    gix_if_null_exit(shader_code, gix_log_error("Can not load shader file"));

    SDL_GPUShaderCreateInfo shader_info = {
        .code = shader_code,
        .code_size = shader_code_size,
        .entrypoint = entry_point,
        .format = backend_format,
        .stage = shader_stage,
        .num_samplers = sampler_count,
        .num_uniform_buffers = uniform_buffer_count,
        .num_storage_buffers = storage_buffer_count,
        .num_storage_textures = storage_texture_count

    };

    SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shader_info);
    gix_if_null_exit(shader, gix_log_error("Cannot create shader"));

    SDL_free(shader_code);

    return shader;
}