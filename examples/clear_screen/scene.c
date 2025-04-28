
#include "scene.h"

static bool init(GixScene *self) {
    self->compute_pipeline = NULL;

    return true;
}

static bool update(GixScene *self, const SDL_Event *event) {
    self->compute_pipeline = NULL;
    switch (event->gbutton.button) {
        case SDL_GAMEPAD_BUTTON_BACK:
            /* code */
            break;

        default:
            break;
    }

    return true;
}
static void draw(GixScene *self) {
    SDL_GPUCommandBuffer *cmd_buffer = SDL_AcquireGPUCommandBuffer(self->app->device);
    if (!cmd_buffer) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    SDL_GPUTexture *swapchain_texture;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, self->app->window, &swapchain_texture, NULL, NULL)) {
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
static void quit(GixScene *self) {
    self->numb_compute_pipeline = 0;
}
GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene, init, update, draw, quit);

    return scene;
}
