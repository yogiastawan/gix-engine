#ifndef __GIX_APP_ENGINE_H__
#define __GIX_APP_ENGINE_H__

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern C {
#endif

    static bool is_inited = false;

    typedef struct _GixScene GixScene;
    typedef struct _GixApp GixApp;

    typedef bool (*SceneInit)(GixScene* self);
    typedef bool (*SceneUpdate)(GixScene* self, const SDL_Event* event);
    typedef void (*SceneDraw)(GixScene* self);
    typedef void (*SceneQuit)(GixScene* self);

    struct _GixScene {
        // app bind
        GixApp* app;

        // list of graphic pipline
        SDL_GPUGraphicsPipeline** graphic_pipeline;
        uint8_t numb_graphic_pipeline;

        // list of compute pipline
        SDL_GPUGraphicsPipeline** compute_pipeline;
        uint8_t numb_compute_pipeline;

        // impl
        SceneInit scene_init;
        SceneUpdate scene_update;
        SceneDraw scene_draw;
        SceneQuit scene_quit;
    };

    GixScene* gix_scene_new(GixApp * app);
    GixScene* gix_scene_from_file(GixApp * app);
    void gix_scene_impl(GixScene * scene, SceneInit init_func, SceneUpdate update_func, SceneDraw draw_func, SceneQuit quit_func);
    uint8_t gix_scene_graphic_pipeline_size(const GixScene* scene);
    uint8_t gix_scene_compute_pipeline_size(const GixScene* scene);
    void gix_scene_destroy(GixScene * scene);

    struct _GixApp {
        SDL_Window* window;
        SDL_GPUDevice* device;
        GixScene* loading_scene;
        GixScene* current_scene;

        bool is_onload_scene;
    };

    GixApp* gix_app_new(const char* name);
    void gix_app_set_loading_scene(GixApp * app, GixScene * scene);
    void gix_app_set_scene(GixApp * app, GixScene * scene);
    void gix_app_run(GixApp * app);
    void gix_app_destroy(GixApp * app);

#ifdef __cplusplus
}
#endif

#endif /* __GIX_APP_ENGINE_H__ */
