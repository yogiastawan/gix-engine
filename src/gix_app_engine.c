
#include <gix_engine/gix_app_engine.h>
#include <gix_engine/gix_checker.h>
#include <gix_engine/gix_log.h>

static void gix_app_sdl_init() {
    bool init_result = SDL_Init(SDL_INIT_VIDEO);
    gix_if_exit(!init_result, gix_log_error("Couldn't init SDL"));
    is_inited = true;
}

GixScene* gix_scene_new(GixApp* app) {
    gix_info("Create new GixScene");

    GixScene* scene = SDL_malloc(sizeof(GixScene));
    scene->app = app;
    scene->graphic_pipeline = NULL;
    scene->numb_graphic_pipeline = 0;
    scene->compute_pipeline = NULL;
    scene->numb_compute_pipeline = 0;

    return scene;
}

void gix_scene_impl(GixScene* scene, SceneInit init_func, SceneUpdate update_func, SceneDraw draw_func, SceneQuit quit_func) {
    gix_if_null_exit(scene, gix_log("Can not destroy of NULL GixScene"));
    scene->scene_init = init_func;
    scene->scene_update = update_func;
    scene->scene_draw = draw_func;
    scene->scene_quit = quit_func;
}

// TODO! implement load scene from file protobuf
GixScene* gix_scene_from_file(GixApp* app) {
    return NULL;
}
uint8_t gix_scene_graphic_pipeline_size(const GixScene* scene) {
    return scene->numb_graphic_pipeline;
}

uint8_t gix_scene_compute_pipeline_size(const GixScene* scene) {
    return scene->numb_compute_pipeline;
}
void gix_scene_destroy(GixScene* scene) {
    gix_info("Destroy GixScene");

    gix_if_null_exit(scene, gix_log("Can not destroy NULl of scene"));
    for (uint8_t i = 0; i < scene->numb_graphic_pipeline; i++) {
        SDL_ReleaseGPUGraphicsPipeline(scene->app->device, scene->graphic_pipeline[i]);
    }

    for (uint8_t i = 0; i < scene->numb_graphic_pipeline; i++) {
        SDL_ReleaseGPUGraphicsPipeline(scene->app->device, scene->compute_pipeline[i]);
    }

    SDL_free(scene->graphic_pipeline);
    SDL_free(scene->compute_pipeline);
}

GixApp* gix_app_new(const char* name) {
    gix_info("Create new GixApp");

    gix_if_null_exit(name, gix_log("Name GixApp shouldn't NULL"));

    if (!is_inited) {
        gix_app_sdl_init();
    }

    GixApp* app = SDL_malloc(sizeof(GixApp));

    // Initialize window
    app->window = SDL_CreateWindow(name, 800, 600, SDL_WINDOW_HIDDEN);
    gix_if_null_exit(app->window, gix_log_error("Couldn't create window"));

    // Initialize GPUDevice
    app->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
                                      true, NULL);
    gix_if_null_exit(app->device, gix_log_error("Couldn't create GPU device"));

    if (!SDL_ClaimWindowForGPUDevice(app->device, app->window)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error on claim window for GPU device. %s", SDL_GetError());
    }

    app->loading_scene = NULL;
    app->current_scene = NULL;

    return app;
}

void gix_app_set_loading_scene(GixApp* app, GixScene* scene) {
    gix_info("Set loading scene to GixApp");

    gix_if_null_exit(app, gix_log("GixApp should not NULL"));
    gix_if_null_exit(scene, gix_log("GixScene should not NULL"));
    app->loading_scene = scene;
}

void gix_app_set_scene(GixApp* app, GixScene* scene) {
    gix_info("Set scene to GixApp");

    gix_if_null_exit(app, gix_log("GixApp should not NULL"));
    gix_if_null_exit(scene, gix_log("GixScene should not NULL"));

    // TODO! Loading scene here
    app->is_onload_scene = true;
    scene->scene_init(scene);
    app->current_scene = scene;
    app->is_onload_scene = false;
}

void gix_app_run(GixApp* app) {
    gix_if_null_exit(app, gix_log("GixApp should not NULL"));

    gix_if_exit(!app->loading_scene && !app->current_scene, gix_log("GixApp scene must not NULL"));

    // show window
    SDL_ShowWindow(app->window);

    bool is_running = true;

    SDL_Event event;

    // main loop
    while (is_running) {
        // event loop
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    is_running = false;
                    break;

                default:
                    break;
            }

            app->current_scene->scene_update(app->current_scene, &event);
        }

        if (app->is_onload_scene && app->loading_scene) {
            app->loading_scene->scene_draw(app->loading_scene);
            continue;
        }

        app->current_scene->scene_draw(app->current_scene);
    }
}

void gix_app_destroy(GixApp* app) {
    gix_info("Destroy GixApp");

    gix_if_null_exit(app, gix_log("Can not destroy of NULL GixApp"));

    if (app->loading_scene) {
        app->loading_scene->scene_quit(app->loading_scene);
        gix_scene_destroy(app->loading_scene);
    }

    if (app->current_scene) {
        app->current_scene->scene_quit(app->current_scene);
        gix_scene_destroy(app->current_scene);
    }

    SDL_ReleaseWindowFromGPUDevice(app->device, app->window);
    SDL_DestroyGPUDevice(app->device);
    SDL_DestroyWindow(app->window);

    SDL_Quit();

    free(app);
}
