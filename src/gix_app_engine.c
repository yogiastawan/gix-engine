// #ifdef GIX_APP_MAIN
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
// #endif
#include <gix_engine/gix_app_engine.h>
#include <gix_engine/gix_checker.h>
#include <gix_engine/gix_log.h>

struct _GixApp {
    SDL_Window* window;
    SDL_GPUDevice* device;
    GixScene* loading_scene;
    GixScene* current_scene;

    bool is_onload_scene;
    Uint64 delta_time;
    Uint64 last_tick;
};

static GixApp* gix_app_new(const char* name);
static void gix_app_destroy(GixApp* app);

static void gix_app_sdl_init() {
    bool init_result = SDL_Init(SDL_INIT_VIDEO);
    gix_if_exit(!init_result, gix_log_error("Couldn't init SDL"));
}
// #ifdef GIX_APP_MAIN

// SDL CYCLE
SDL_AppResult SDL_AppInit(void** app_state, int argc, char* argv[]) {
    if (argc > 1) {
        gix_info("%s", argv[1]);
    }

    GixApp* app = gix_app_new("GixApp");
    app_state[0] = app;
    gix_app_init(app);
    SDL_ShowWindow(app->window);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* app_state, SDL_Event* event) {
    GixApp* app = (GixApp*)app_state;
    gix_if_null_exit(app, gix_log("GixApp should not NULL"));

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
            break;

        default:
            if (app->is_onload_scene && app->loading_scene) {
                return gix_scene_event(app->loading_scene, event);
            }
            return gix_scene_event(app->current_scene, event);
            break;
    }
}

SDL_AppResult SDL_AppIterate(void* app_state) {
    GixApp* app = (GixApp*)app_state;
    gix_if_null_exit(app, gix_log("GixApp should not NULL"));

    Uint64 current_tick = SDL_GetTicks();
    app->delta_time = (current_tick - app->last_tick);
    app->last_tick = current_tick;

    if (app->loading_scene) {
        SDL_AppResult update_result = gix_scene_update(app->loading_scene, app->delta_time);
        SDL_AppResult draw_result = gix_scene_draw(app->loading_scene);
        // return SDL_APP_CONTINUE=0 when continue
        // return SDL_APP_SUCCESS=1 when success and quit
        // return SDL_APP_FAILURE=2 when failed
        return update_result | draw_result;
    }
    SDL_AppResult update_result = gix_scene_update(app->current_scene, app->delta_time);
    SDL_AppResult draw_result = gix_scene_draw(app->current_scene);
    // return SDL_APP_CONTINUE=0 when continue
    // return SDL_APP_SUCCESS=1 when success
    // return SDL_APP_FAILURE=2 when failed
    return update_result | draw_result;
}
void SDL_AppQuit(void* app_state, SDL_AppResult result) {
    if (result == SDL_APP_FAILURE) {
        gix_log("App quit with error");
    }

    GixApp* app = (GixApp*)app_state;
    gix_if_null_exit(app, gix_log("GixApp should not NULL"));

    gix_app_destroy(app);
}

// END SDL CYCLE

// #endif

// private function
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

// TODO! implement load scene from file protobuf
GixScene* gix_scene_from_file(GixApp* app, const char* file_path) {
    gix_info("Create new GixScene");

    GixScene* scene = SDL_malloc(sizeof(GixScene));
    scene->app = app;
    return scene;
}

void gix_scene_impl(GixScene* scene, SceneInit init_func, SceneEvent event_func, SceneUpdate update_func, SceneDraw draw_func, SceneQuit quit_func) {
    gix_if_null_exit(scene, gix_log("Can impl of NULL GixScene"));
    scene->scene_init = init_func;
    scene->scene_event = event_func;
    scene->scene_update = update_func;
    scene->scene_draw = draw_func;
    scene->scene_quit = quit_func;
}

void gix_scene_destroy(GixScene* scene) {
    gix_info("Destroy GixScene");

    gix_if_null_exit(scene, gix_log("Can not destroy NULl of scene"));
    for (Uint8 i = 0; i < scene->numb_graphic_pipeline; i++) {
        SDL_ReleaseGPUGraphicsPipeline(scene->app->device, scene->graphic_pipeline[i]);
    }

    for (Uint8 i = 0; i < scene->numb_compute_pipeline; i++) {
        SDL_ReleaseGPUGraphicsPipeline(scene->app->device, scene->compute_pipeline[i]);
    }

    SDL_free(scene->graphic_pipeline);
    SDL_free(scene->compute_pipeline);
}

GixApp* gix_app_new(const char* name) {
    gix_info("Create new GixApp");

    gix_if_null_exit(name, gix_log("Name GixApp shouldn't NULL"));

    gix_app_sdl_init();

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
    app->is_onload_scene = false;
    app->delta_time = 0;
    app->last_tick = 0;

    return app;
}

void gix_app_set_window_fullscreen(GixApp* app) {
    SDL_SetWindowFullscreen(app->window, true);
}

void gix_app_set_window_borderless(GixApp* app, bool borderless) {
    SDL_SetWindowBordered(app->window, borderless);
}

void gix_app_set_name(GixApp* app, const char* name) {
    SDL_SetWindowTitle(app->window, name);
}

void gix_app_set_window_size(GixApp* app, int width, int height) {
    SDL_SetWindowSize(app->window, width, height);
}

void gix_app_set_window_position(GixApp* app, int x, int y) {
    SDL_SetWindowPosition(app->window, x, y);
}

void gix_app_set_window_resizeable(GixApp* app, bool resizeable) {
    SDL_SetWindowResizable(app->window, resizeable);
    gix_log_error("Set window resizeable");
}

void gix_app_set_loading_scene(GixApp* app, GixScene* scene) {
    gix_info("Set loading scene to GixApp");

    gix_if_null_exit(app, gix_log("GixApp should not NULL"));
    gix_if_null_exit(scene, gix_log("GixScene should not NULL"));
    app->loading_scene = scene;
}

SDL_AppResult gix_app_set_scene(GixApp* app, GixScene* scene) {
    gix_info("Set scene to GixApp");

    gix_if_null_exit(app, gix_log("GixApp should not NULL"));
    gix_if_null_exit(scene, gix_log("GixScene should not NULL"));

    // TODO! Loading scene here
    // set is_onload_scene to true
    app->is_onload_scene = true;
    // init scene
    SDL_AppResult result = gix_scene_init(scene);
    // set scene to current scene
    app->current_scene = scene;
    // set is_onload_scene to false
    app->is_onload_scene = false;
    return result;
}

SDL_Window* gix_app_get_window(GixApp* app) {
    return app->window;
}

SDL_GPUDevice* gix_app_get_gpu_device(GixApp* app) {
    return app->device;
}

SDL_GPUTextureFormat gix_app_get_depth_texture_format(GixApp* app) {
    SDL_GPUTextureFormat format;
    if (SDL_GPUTextureSupportsFormat(app->device,
                                     SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
                                     SDL_GPU_TEXTURETYPE_2D,
                                     SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    } else if (SDL_GPUTextureSupportsFormat(app->device,
                                            SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
                                            SDL_GPU_TEXTURETYPE_2D,
                                            SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
    } else if (SDL_GPUTextureSupportsFormat(app->device,
                                            SDL_GPU_TEXTUREFORMAT_D24_UNORM,
                                            SDL_GPU_TEXTURETYPE_2D,
                                            SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D24_UNORM;
    } else if (SDL_GPUTextureSupportsFormat(app->device,
                                            SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
                                            SDL_GPU_TEXTURETYPE_2D,
                                            SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
    } else {
        format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
    }

    gix_info("Get depth texture format support: %u", format);
    return format;
}

// private function
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
