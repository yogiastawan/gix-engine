#ifndef __GIX_APP_ENGINE_H__
#define __GIX_APP_ENGINE_H__

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

#define gix_scene_init(scene) scene->scene_init(scene)
#define gix_scene_event(scene, event) scene->scene_event(scene, event)
#define gix_scene_update(scene, delta_time) scene->scene_update(scene, delta_time)
#define gix_scene_draw(scene) scene->scene_draw(scene)
#define gix_scene_quit(scene) scene->scene_quit(scene)

///* GixScene struct is used to create a scene in the GixApp engine.
///* All GixScene field are public
typedef struct _GixScene GixScene;

///* GixApp struct is used to create a GixApp engine.
///* All GixApp field are private. But you can use GixApp public function to access it.
///* GixApp is a singleton class, you can only create one instance of it.
typedef struct _GixApp GixApp;

typedef SDL_AppResult (*SceneInit)(GixScene* self);
typedef SDL_AppResult (*SceneEvent)(GixScene* self, const SDL_Event* event);
typedef SDL_AppResult (*SceneUpdate)(GixScene* self, Uint64 delta_time);
typedef SDL_AppResult (*SceneDraw)(GixScene* self);
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

    // implement function
    SceneInit scene_init;
    SceneEvent scene_event;
    SceneUpdate scene_update;
    SceneDraw scene_draw;
    SceneQuit scene_quit;
};

/*SCENE PUBLIC FUNCTION*/
/**
 * @brief Create new GixScene
 *
 * @param app GixApp pointer
 * @return GixScene*
 */
GixScene* gix_scene_new(GixApp* app);

/**
 * @brief Create new GixScene from file
 *
 * @param app GixApp pointer
 * @param file_path Path to file
 * @return GixScene*
 * @note This function will load scene from file
 */
GixScene* gix_scene_from_file(GixApp* app, const char* file_path);

/**
 * @brief Set scene implementation function
 *
 * @param scene GixScene pointer
 * @param init_func SceneInit function
 * @param event_func SceneEvent function
 * @param update_func SceneUpdate function
 * @param draw_func SceneDraw function
 * @param quit_func SceneQuit function
 */
void gix_scene_impl(GixScene* scene, SceneInit init_func, SceneEvent event_func, SceneUpdate update_func, SceneDraw draw_func, SceneQuit quit_func);

/**
 * @brief Allocate graphic pipeline of GixScene
 *
 * @param scene GixScene pointer
 * @param numb Number of graphic pipeline. Value: 0-255
 */
static inline void gix_scene_alloc_graphic_pipeline(GixScene* scene, Uint8 numb) {
    scene->numb_graphic_pipeline = numb;
    scene->graphic_pipeline = SDL_malloc(sizeof(void*) * numb);
}

/**
 * @brief Allocate compute pipeline of GixScene
 *
 * @param scene GixScene pointer
 * @param numb Number of compute pipeline. Value: 0-255
 */
static inline void gix_scene_alloc_compute_pipeline(GixScene* scene, Uint8 numb) {
    scene->numb_compute_pipeline = numb;
    scene->compute_pipeline = SDL_malloc(sizeof(void*) * numb);
}

/**
 * @brief Destroy GixScene
 *
 * @param scene GixScene pointer
 */
void gix_scene_destroy(GixScene* scene);

struct _GixApp;

/*APP PUBLIC FUNCTION*/
/**
 * @brief Init GixApp here. here we can set scene, window size, position, name, etc. This function is entry of GixApp engine.
 *
 * @param app GixApp pointer, passed from engine.
 */
extern void gix_app_init(GixApp* app);

/**
 * @brief Set GixApp window to fullscreen
 *
 * @param app GixApp pointer
 */
void gix_app_set_window_fullscreen(GixApp* app);

/**
 * @brief Set GixApp window to borderless or not
 *
 * @param app GixApp pointer
 * @param borderless Bool value to set window borderless or not
 */
void gix_app_set_window_borderless(GixApp* app, bool borderless);

/**
 * @brief Set GixApp window name
 *
 * @param app GixApp pointer
 * @param name Name of window
 */
void gix_app_set_name(GixApp* app, const char* name);

/**
 * @brief Set GixApp window size
 *
 * @param app GixApp pointer
 * @param width Integer value of width
 * @param height Integer value of height
 */
void gix_app_set_window_size(GixApp* app, int width, int height);

/**
 * @brief Set GixApp window position
 *
 * @param app GixApp pointer
 * @param x Integer value of x position or use SDL_WINDOWPOS_CENTERED
 * @param y Integer value of y position or use SDL_WINDOWPOS_CENTERED
 */
void gix_app_set_window_position(GixApp* app, int x, int y);

void gix_app_set_window_resizeable(GixApp* app, bool resizeable);

/**
 * @brief Set GixApp loading scene
 *
 * @param app GixApp pointer
 * @param scene GixScene pointer
 */
void gix_app_set_loading_scene(GixApp* app, GixScene* scene);

/**
 * @brief Set GixApp scene
 *
 * @param app GixApp pointer
 * @param scene GixScene pointer
 * @return SDL_AppResult. return SDL_APP_CONTINUE if success, SDL_APP_FAILURE if failed
 */
SDL_AppResult gix_app_set_scene(GixApp* app, GixScene* scene);

/**
 * @brief Get GixApp window
 *
 * @param app GixApp pointer
 * @return SDL_Window* SDL_Window pointer
 */
SDL_Window* gix_app_get_window(GixApp* app);

/**
 * @brief Get GixApp GPU device
 *
 * @param app GixApp pointer
 * @return SDL_GPUDevice* SDL_GPUDevice pointer
 */
SDL_GPUDevice* gix_app_get_gpu_device(GixApp* app);

#ifdef __cplusplus
}
#endif

#endif /* __GIX_APP_ENGINE_H__ */
