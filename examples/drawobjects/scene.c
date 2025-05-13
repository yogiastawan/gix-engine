#include "scene.h"

static SDL_AppResult scene_init(GixScene* scene) {
    // Initialize the scene
    // This function should be implemented to set up the scene
    // For example, you might want to create graphics pipelines, load resources, etc.
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
    return SDL_APP_CONTINUE;
}
static SDL_AppResult scene_draw(GixScene* scene) {
    // Draw the scene
    // This function should be implemented to render the scene
    return SDL_APP_CONTINUE;
}
static void scene_quit(GixScene* scene) {
    // Clean up the scene
    // This function should be implemented to release resources and perform any necessary cleanup
}

GixScene* create_scene(GixApp* app) {
    GixScene* scene = gix_scene_new(app);

    gix_scene_impl(scene, scene_init, scene_event, scene_update, scene_draw, scene_quit);

    return scene;
}