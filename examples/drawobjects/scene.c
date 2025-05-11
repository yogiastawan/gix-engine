#include "scene.h"

bool scene_init(GixScene* scene) {
    // Initialize the scene
    // This function should be implemented to set up the scene
    // For example, you might want to create graphics pipelines, load resources, etc.
    return true;
}
bool scene_event(GixScene* scene, SDL_Event* event) {
    // Handle events for the scene
    // This function should be implemented to respond to user input or other events
    return true;
}
bool scene_update(GixScene* scene) {
    // Update the scene
    // This function should be implemented to update the state of the scene
    return true;
}
bool scene_draw(GixScene* scene) {
    // Draw the scene
    // This function should be implemented to render the scene
    return true;
}
bool scene_quit(GixScene* scene) {
    // Clean up the scene
    // This function should be implemented to release resources and perform any necessary cleanup
    return true;
}

GixScene* create_scene(GixApp* app) {
    GixScene* scene = gix_scene_create(app);

    gix_scene_implement(scene, scene_init, scene_event, scene_update, scene_draw, scene_quit);

    return scene;
}