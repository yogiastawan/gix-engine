#ifndef __GIX_APP_ENGINE_H__
#define __GIX_APP_ENGINE_H__

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern C {
#endif

#define gix_scene_init(scene) scene->scene_init(scene)
#define gix_scene_event(scene, event) scene->scene_event(scene, event)
#define gix_scene_update(scene, delta_time) scene->scene_update(scene, delta_time)
#define gix_scene_draw(scene) scene->scene_draw(scene)
#define gix_scene_quit(scene) scene->scene_quit(scene)

    typedef struct _GixScene GixScene;
    typedef struct _GixApp GixApp;

    typedef bool (*SceneInit)(GixScene* self);
    typedef void (*SceneEvent)(GixScene* self, const SDL_Event* event);
    typedef void (*SceneUpdate)(GixScene* self, Uint64 delta_time);
    typedef void (*SceneDraw)(GixScene* self);
    typedef void (*SceneQuit)(GixScene* self);

    struct _GixScene {
        // app bind
        GixApp* app;

        // list of graphic pipline
        SDL_GPUGraphicsPipeline** graphic_pipeline;
        Uint8 numb_graphic_pipeline;

        // list of compute pipline
        SDL_GPUGraphicsPipeline** compute_pipeline;
        Uint8 numb_compute_pipeline;

        // impl
        SceneInit scene_init;
        SceneEvent scene_event;
        SceneUpdate scene_update;
        SceneDraw scene_draw;
        SceneQuit scene_quit;
    };

    GixScene* gix_scene_new(GixApp * app);
    GixScene* gix_scene_from_file(GixApp * app);
    void gix_scene_impl(GixScene * scene, SceneInit init_func, SceneEvent event_func, SceneUpdate update_func, SceneDraw draw_func, SceneQuit quit_func);
    Uint8 gix_scene_graphic_pipeline_size(const GixScene* scene);
    Uint8 gix_scene_compute_pipeline_size(const GixScene* scene);
    void gix_scene_destroy(GixScene * scene);

    struct _GixApp;

    extern void gix_app_init(GixApp * app);
    void gix_app_set_window_fullscreen(GixApp * app);
    void gix_app_set_window_borderless(GixApp * app);
    void gix_app_set_loading_scene(GixApp * app, GixScene * scene);
    SDL_AppResult gix_app_set_scene(GixApp * app, GixScene * scene);
    SDL_Window* gix_app_get_window(GixApp * app);
    SDL_GPUDevice* gix_app_get_gpu_device(GixApp * app);

#ifdef __cplusplus
}
#endif

#endif /* __GIX_APP_ENGINE_H__ */
