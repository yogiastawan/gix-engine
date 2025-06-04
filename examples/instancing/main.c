#include <gix_engine/gix_engine.h>

#include "scene.h"

void gix_app_init(GixApp *app) {
    gix_app_set_scene(app, create_scene(app));
}