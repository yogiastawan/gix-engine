#include "scene.h"

#include <cglm/cglm.h>

#define NUMB_RECT 5000

// create react with same size for all instance
typedef struct _Rect {
    vec2 pos;
    vec2 size;
    vec4 color;
} RectMono;

RectMono rects[NUMB_RECT];

static void create_rects() {
    for (size_t i = 0; i < NUMB_RECT; i++) {
        rects[i] = (RectMono){
            // .pos = {-0.5, -0.5},
            .pos = {(float)(SDL_rand(800 * 2) - 800) / 800.0f, (float)(SDL_rand(600 * 2) - 600) / 600.0f},
            // .size = {0.5, 0.5},
            .size = {(float)SDL_rand(100) / 100.0f, (float)SDL_rand(100) / 100.0f},
            // .color = {1.0f, 1.0f, 1.0f, 1.0f},
            .color = {(float)SDL_rand(255) / 255.0f, (float)SDL_rand(255) / 255.0f, (float)SDL_rand(255) / 255.0f, 1.0}};
    }
}

typedef struct _MPV {
    mat4 mpv;
} MPV;

MPV mpv;

SDL_GPUBuffer* rects_buffer;

float rotate_angle = 0.0f;
const float rotate_speed = 0.1f;

static void update_mpv(Uint64 delta_time) {
    mat4 model, view, projection;
    glm_mat4_identity(model);
    glm_mat4_identity(view);
    glm_mat4_identity(projection);

    rotate_angle += glm_rad(rotate_speed) * (float)delta_time;

    glm_rotate(model, rotate_angle, (vec3){0.0f, 1.0f, 0.0f});
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.001f, 1000.0f, projection);
    glm_translate(view, (vec3){0.0f, 0.0f, -10.0f});
    glm_mat4_mulN((mat4*[]){&projection, &view, &model}, 3, mpv.mpv);
}

static SDL_AppResult scene_init(GixScene* scene) {
    // Initialize the scene
    // This function should be implemented to set up the scene
    // For example, you might want to create graphics pipelines, load resources, etc.

    gix_info("Init draw objects scene: %lu", sizeof(RectMono));

    create_rects();

    SDL_Window* window = gix_app_get_window(scene->app);
    SDL_GPUDevice* device = gix_app_get_gpu_device(scene->app);

    scene->numb_graphic_pipeline = 1;
    scene->graphic_pipeline = SDL_malloc(sizeof(SDL_GPUGraphicsPipeline*));

    // load shader
    // IMPORTANT: add numb storage buffer to 1;
    SDL_GPUShader* vertex_shader = gix_load_shader(device, "./shader/SPIRV/drawobjects.vert.spv",
                                                   SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, 1, 0);
    SDL_GPUShader* frag_shader = gix_load_shader(device, "./shader/SPIRV/drawobjects.frag.spv",
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 0, 0, 0);

    // create pipelene
    SDL_GPUColorTargetDescription color_target_desc[] = {
        (SDL_GPUColorTargetDescription){
            .format = SDL_GetGPUSwapchainTextureFormat(device, window)},
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelien_info = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = color_target_desc,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = vertex_shader,
        .fragment_shader = frag_shader,
    };

    scene->graphic_pipeline[0] = SDL_CreateGPUGraphicsPipeline(device, &pipelien_info);
    gix_if_null_exit(scene->graphic_pipeline[0], gix_log_error("Couldn't create graphic pipeline"));
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, frag_shader);

    // create buffer
    SDL_GPUBufferCreateInfo buffer_info = {
        .size = sizeof(RectMono) * NUMB_RECT,
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
    };

    rects_buffer = SDL_CreateGPUBuffer(device, &buffer_info);

    // create transfer buffer
    SDL_GPUTransferBufferCreateInfo transfer_info = {
        .size = sizeof(RectMono) * NUMB_RECT,
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    };
    SDL_GPUTransferBuffer* transfer_data_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);

    // map gpu tranfer buffer
    RectMono* transfer_rects = SDL_MapGPUTransferBuffer(device, transfer_data_buffer, true);
    SDL_memcpy(transfer_rects, rects, sizeof(RectMono) * NUMB_RECT);
    // unmap transfer buffer
    SDL_UnmapGPUTransferBuffer(device, transfer_data_buffer);

    SDL_GPUCommandBuffer* upload_cmd = SDL_AcquireGPUCommandBuffer(device);
    if (!upload_cmd) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_cmd);

    SDL_GPUTransferBufferLocation transfer_buffer_location = {
        .transfer_buffer = transfer_data_buffer,
        .offset = 0,
    };
    SDL_GPUBufferRegion rects_buffer_region = {
        .buffer = rects_buffer,
        .offset = 0,
        .size = sizeof(RectMono) * NUMB_RECT,
    };
    // upload
    SDL_UploadToGPUBuffer(copy_pass, &transfer_buffer_location, &rects_buffer_region, true);

    SDL_EndGPUCopyPass(copy_pass);

    // submit command buffer
    if (!SDL_SubmitGPUCommandBuffer(upload_cmd)) {
        gix_log_error("Couldn't submit command buffer");
    }
    SDL_ReleaseGPUTransferBuffer(device, transfer_data_buffer);
    return SDL_APP_CONTINUE;
}
static SDL_AppResult scene_event(GixScene* scene, const SDL_Event* event) {
    // Handle events for the scene
    // This function should be implemented to respond to user input or other events
    return SDL_APP_CONTINUE;
}
static SDL_AppResult scene_update(GixScene* scene, Uint64 delta_time) {
    // Update the scene
    // This function should be implemented to update the state of the scene
    update_mpv(delta_time);
    return SDL_APP_CONTINUE;
}
static SDL_AppResult scene_draw(GixScene* scene) {
    // Draw the scene
    // This function should be implemented to render the scene
    SDL_GPUCommandBuffer* cmd_buffer = SDL_AcquireGPUCommandBuffer(gix_app_get_gpu_device(scene->app));
    if (!cmd_buffer) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    SDL_GPUTexture* swapchain_texture;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, gix_app_get_window(scene->app), &swapchain_texture, NULL, NULL)) {
        SDL_GPUColorTargetInfo color_target_info = {0};
        color_target_info.texture = swapchain_texture;
        color_target_info.clear_color = (SDL_FColor){0.3f, 0.4f, 0.5f, 1.0f};
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;
        color_target_info.cycle = false;

        SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cmd_buffer, &color_target_info, 1, NULL);
        // bind pipeline
        SDL_BindGPUGraphicsPipeline(render_pass, scene->graphic_pipeline[0]);

        SDL_BindGPUVertexStorageBuffers(render_pass, 0, &rects_buffer, 1);

        // push uniform data
        SDL_PushGPUVertexUniformData(cmd_buffer, 0, &mpv, sizeof(MPV));

        // draw
        SDL_DrawGPUPrimitives(render_pass, 6 * NUMB_RECT, 1, 0, 0);
        SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(cmd_buffer);

    return SDL_APP_CONTINUE;
}
static void scene_quit(GixScene* scene) {
    // Clean up the scene
    // This function should be implemented to release resources and perform any necessary cleanup
    SDL_ReleaseGPUBuffer(gix_app_get_gpu_device(scene->app), rects_buffer);
}

GixScene* create_scene(GixApp* app) {
    GixScene* scene = gix_scene_new(app);

    gix_scene_impl(scene, scene_init, scene_event, scene_update, scene_draw, scene_quit);

    return scene;
}