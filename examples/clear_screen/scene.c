
#include "scene.h"

static bool clear_scene_init(GixScene *self) {
    gix_info("Init clear scene");
    // Init scene here
    self->compute_pipeline = NULL;

    return true;
}

static void clear_scene_event(GixScene *self, const SDL_Event *event) {
    // Handle event here
    self->compute_pipeline = NULL;
    switch (event->gbutton.button) {
        case SDL_GAMEPAD_BUTTON_BACK:
            /* code */
            break;

        default:
            break;
    }
}

static void scene_update(GixScene *self, Uint64 delta_time) {
    // Update scene here
    self->compute_pipeline = NULL;
}
static void clear_scene_draw(GixScene *self) {
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

        SDL_GPURenderPass *renderPass = SDL_BeginGPURenderPass(cmd_buffer, &colorTargetInfo, 1, NULL);
        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(cmd_buffer);
}
static void clear_scene_quit(GixScene *self) {
    gix_info("Quit clear scene");
    // Deinit scene here
    self->numb_compute_pipeline = 0;
}
GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene, clear_scene_init, clear_scene_event, scene_update, clear_scene_draw, clear_scene_quit);

    return scene;
}
