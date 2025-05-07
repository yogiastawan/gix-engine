#include "scene.h"

static bool scene_init(GixScene *self) {
    gix_info("Init scene");
    // Init scene here
    return true;
}
static void scene_event(GixScene *self, const SDL_Event *event) {
    // Handle event here
}
static void scene_update(GixScene *self, Uint64 delta_time) {
    // Handle event here
}
static void scene_draw(GixScene *self) {
    // Draw frame here
    SDL_GPUCommandBuffer *cmd_buffer = SDL_AcquireGPUCommandBuffer(gix_app_get_gpu_device(self->app));
    if (!cmd_buffer) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    // TODO! draw here
    SDL_SubmitGPUCommandBuffer(cmd_buffer);
}
static void scene_quit(GixScene *self) {
    // Handle quit here
    gix_info("Quit scene");
}

GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene, scene_init, scene_event, scene_update, scene_draw, scene_quit);

    return scene;
}