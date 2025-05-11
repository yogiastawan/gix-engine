#include "scene.h"

void gix_app_init(GixApp* app) {
    // Initialize the application
    // This function should be implemented to set up the application
    gix_app_set_name(app, "Draw Objects");
    gix_app_set_scene(app, create_scene(app));
}