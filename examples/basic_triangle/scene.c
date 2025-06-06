#include "scene.h"

// static SDL_GPUViewport viewport = {160, 120, 320, 240, 0.1f, 1.0f};

static SDL_AppResult basic_triangle_init(GixScene *self) {
    gix_log("Init basic triangle");
    // Init scene here

    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);
    SDL_Window *window = gix_app_get_window(self->app);

    // create 1 graphic pipeline
    self->numb_graphic_pipeline = 1;
    self->graphic_pipeline = SDL_malloc(sizeof(SDL_GPUGraphicsPipeline *));

    // load shader
    SDL_GPUShader *vertex_shader = gix_load_shader(device, "./shader/SPIRV/basic_triangle.vert.spv",
                                                   "./shader/SPIRV/basic_triangle.vert.json", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader *frag_shader = gix_load_shader(device, "./shader/SPIRV/triangle_fill.frag.spv",
                                                 "./shader/SPIRV/triangle_fill.frag.json",
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT);

    // create pipeline
    SDL_GPUColorTargetDescription color_target_desc[] = {
        (SDL_GPUColorTargetDescription){
            .format = SDL_GetGPUSwapchainTextureFormat(device, window)}};

    SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = color_target_desc,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = vertex_shader,
        .fragment_shader = frag_shader,
        .rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL,
    };

    self->graphic_pipeline[0] = SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);

    gix_if_null_exit(self->graphic_pipeline[0], gix_log_error("Couldn't create graphic pipeline"));

    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, frag_shader);

    return SDL_APP_CONTINUE;
}

static SDL_AppResult scene_event(GixScene *self, const SDL_Event *event) {
    // Handle event here
    return SDL_APP_CONTINUE;
}

static SDL_AppResult basic_triangle_update(GixScene *self, Uint64 delta_time) {
    // Handle event here
    return SDL_APP_CONTINUE;
}
static SDL_AppResult basic_triangle_draw(GixScene *self) {
    // Draw frame here
    SDL_GPUCommandBuffer *cmd_buffer = SDL_AcquireGPUCommandBuffer(gix_app_get_gpu_device(self->app));
    if (!cmd_buffer) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    SDL_GPUTexture *swapchain_texture;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, gix_app_get_window(self->app), &swapchain_texture, NULL, NULL)) {
        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture = swapchain_texture;
        colorTargetInfo.clear_color = (SDL_FColor){0.3f, 0.4f, 0.5f, 1.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(cmd_buffer, &colorTargetInfo, 1, NULL);
        // bind pipeline
        SDL_BindGPUGraphicsPipeline(render_pass, self->graphic_pipeline[0]);
        // set viewport
        // SDL_SetGPUViewport(render_pass, &viewport);
        // draw primitive
        SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
        SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(cmd_buffer);
    return SDL_APP_CONTINUE;
}
static void basic_triangle_quit(GixScene *self) {
    gix_log("Quit basic triangle");
    // Deinit scene here
    self->numb_compute_pipeline = 0;
}
GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene, basic_triangle_init, scene_event, basic_triangle_update, basic_triangle_draw, basic_triangle_quit);

    return scene;
}