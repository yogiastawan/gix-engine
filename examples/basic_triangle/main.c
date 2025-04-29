#include <gix_engine/gix_engine.h>

#include "scene.h"

int main() {
    GixApp *app = gix_app_new("Basic Triangle");

    gix_app_set_scene(app, create_scene(app));

    gix_app_run(app);

    gix_app_destroy(app);
    return 0;
}
