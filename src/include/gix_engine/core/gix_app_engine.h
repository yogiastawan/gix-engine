#ifndef __GIX_APP_ENGINE_H__
#define __GIX_APP_ENGINE_H__

#include <SDL3/SDL.h>
#include <cglm/cglm.h>
#include <gix_arena/gix_arena.h>
#include <gix_engine/core/gix_engine_checker.h>
#include <gix_engine/core/gix_engine_log.h>
#include <gix_engine/utilities/gix_engine_define.h>

#ifdef __cplusplus
extern "C" {
#endif

// Define default memory arena size
#ifndef GIX_ENGINE_ARENA_DEFAULT_SIZE
/// @brief Default size of memory arena for GixScene in bytes
#define GIX_ENGINE_ARENA_DEFAULT_SIZE 1048576  // 1 MB
#endif

#ifndef GIX_ENGINE_SHADER_DIR
#define GIX_ENGINE_SHADER_DIR System Error.GIX_ENGINE_SHADER_DIR must be defined
#endif

#define GIX_ENGINE_NUMB_GRID_3D_LINE_DEFAULT 50
#define GIX_ENGINE_COLOR_GRID_3D_LINE_DEFAULT (u8[4]){200, 200, 200, 255}

#define gix_scene_init(scene) scene->scene_init(scene)
#define gix_scene_event(scene, event) scene->scene_event(scene, event)
#define gix_scene_update(scene, delta_time) \
    scene->scene_update(scene, delta_time)
#define gix_scene_draw(scene) scene->scene_draw(scene)
#define gix_scene_quit(scene) scene->scene_quit(scene)

#ifdef BUILD_DEBUG
/**
 * @brief Setup 3D grid
 *
 * @param scene_ptr GixScene pointer
 * @param line_length_u32 length of each line grid in format `unsigned int`
 *
 * @note Call this on setup. Size of each grid created is 1x1 (1 meter * 1
 * meter)
 */
#define gix_scene_setup_3d_grid(scene_ptr, line_length_u32) \
    (__internal_gix_scene_setup_3d_grid)(scene_ptr, line_length_u32)

/**
 * @brief Draw 3D grid
 *
 * @param scene_ptr GixScene pointer
 * @param cmd_buffer_ptr SDL_GPUCommmandBuffer pointer
 * @param render_pass_ptr SDL_RenderPass pointer
 *
 * @note Call this function inside draw loop
 */
#define gix_scene_draw_3d_grid(scene_ptr, cmd_buffer_ptr, render_pass_ptr, \
                               vp_mat4)                                    \
    (__internal_gix_scene_draw_3d_grid)(scene_ptr, cmd_buffer_ptr,         \
                                        render_pass_ptr, vp_mat4)

/**
 * @brief Set color of line 3D grid
 *
 * @param scene_ptr GixScene pointer
 * @param color_u8_4 Color in array `unsigned int[4]`
 *
 * @note Call this function before call `gix_scene_setup_3d_grid`
 */
#define gix_scene_set_3d_grid_color(scene_ptr, color_u8_4) \
    (__internal_gix_scene_set_3d_grid_color)(scene_ptr, color_u8_4)
#else
#define gix_scene_setup_3d_grid(...)
#define gix_scene_draw_3d_grid(...)
#define gix_scene_set_3d_grid_numb_line(...)
#define gix_scene_set_3d_grid_color(...)
#endif

///* GixScene struct is used to create a scene in the GixApp engine.
///* All GixScene field are public
typedef struct _GixScene GixScene;

///* GixApp struct is used to create a GixApp engine.
///* All GixApp field are private. But you can use GixApp public function to
/// access it.
///* GixApp is a singleton class, you can only create one instance of it.
typedef struct _GixApp GixApp;

typedef struct _GixSceneDebugPrivate GixSceneDebugPrivate;

typedef SDL_AppResult (*SceneInit)(GixScene* self);
typedef SDL_AppResult (*SceneEvent)(GixScene* self, const SDL_Event* event);
typedef SDL_AppResult (*SceneUpdate)(GixScene* self, Uint64 delta_time);
typedef SDL_AppResult (*SceneDraw)(GixScene* self);
typedef void (*SceneQuit)(GixScene* self);

struct _GixScene {
    /// app bind
    GixApp* app;

    /// List of graphic pipline
    SDL_GPUGraphicsPipeline** graphic_pipeline;
    /// Number of graphics pipeline
    u8 numb_graphic_pipeline;

    /// List of compute pipline
    SDL_GPUComputePipeline** compute_pipeline;
    /// Number of compute pipeline
    u8 numb_compute_pipeline;

    // Implement function
    SceneInit scene_init;
    SceneEvent scene_event;
    SceneUpdate scene_update;
    SceneDraw scene_draw;
    SceneQuit scene_quit;

    /// User Data
    void* user_data;

    /// Memory arena
    GixArena* arena;
    usize arena_size;

#ifdef BUILD_DEBUG
    GixSceneDebugPrivate* priv;
#endif
};

/*SCENE PUBLIC FUNCTION*/

/**
 * @brief Create new GixScene
 *
 * @param app GixApp pointer
 * @param numb_graphic_pipeline Number of graphic pipeline. Value: 0-255
 * @param numb_compute_pipeline Number of compute pipeline. Value: 0-255
 * @param arena_size Size of memory arena in bytes.
 *
 * @return GixScene*
 */
GEAPI GixScene* gix_scene_new(GixApp* app, u8 numb_graphic_pipeline,
                              u8 numb_compute_pipeline, usize arena_size);

/**
 * @brief Create new GixScene from file
 *
 * @param app GixApp pointer
 * @param file_path Path to file
 * @return GixScene*
 * @note This function will load scene from file
 */
GEAPI GixScene* gix_scene_from_file(GixApp* app, const char* file_path);

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
GEAPI void gix_scene_impl(GixScene* scene, SceneInit init_func,
                          SceneEvent event_func, SceneUpdate update_func,
                          SceneDraw draw_func, SceneQuit quit_func);

/**
 * @brief Allocate graphic pipeline of GixScene
 *
 * @param scene GixScene pointer
 * @note This function must be called once only on init scene.
 */
static inline void gix_scene_alloc_graphic_pipeline(GixScene* scene) {
    scene->graphic_pipeline = gix_arena_alloc(
        scene->arena, sizeof(void*) * scene->numb_graphic_pipeline);
}

/**
 * @brief Allocate compute pipeline of GixScene
 *
 * @param scene GixScene pointer
 * @note This function must be called once only on init scene.
 */
static inline void gix_scene_alloc_compute_pipeline(GixScene* scene) {
    scene->compute_pipeline = gix_arena_alloc(
        scene->arena, sizeof(void*) * scene->numb_compute_pipeline);
}



#ifdef BUILD_DEBUG
void __internal_gix_scene_setup_3d_grid(GixScene* scene, u32 lenght_side);
#define __internal_gix_scene_setup_3d_grid(...) \
    Error:                                      \
    use function gix_scene_setup_3d_grid instead
void __internal_gix_scene_draw_3d_grid(GixScene* scene,
                                       SDL_GPUCommandBuffer* cmd,
                                       SDL_GPURenderPass* render_pass, mat4 vp);
#define __internal_gix_scene_draw_3d_grid(...) \
    Error:                                     \
    use function gix_scene_draw_3d_grid instead

void __internal_gix_scene_set_3d_grid_color(GixScene* scene, const u8 color[4]);
#define __internal_gix_scene_set_3d_grid_color(...) \
    Error:                                          \
    use function gix_scene_set_3d_grid_color instead
#endif

/**
 * @brief Destroy GixScene
 *
 * @param scene GixScene pointer
 */
GEAPI void gix_scene_destroy(GixScene* scene);

struct _GixApp;

/*APP PUBLIC FUNCTION*/

/**
 * @brief Init GixApp here. here we can set scene, window size, position, name,
 * etc. This function is entry of GixApp engine.
 *
 * @param app GixApp pointer, passed from engine.
 */
extern void gix_app_init(GixApp* app);

/**
 * @brief Set GixApp window to fullscreen
 *
 * @param app GixApp pointer
 */
GEAPI void gix_app_set_window_fullscreen(GixApp* app);

/**
 * @brief Set GixApp window to borderless or not
 *
 * @param app GixApp pointer
 * @param borderless Bool value to set window borderless or not
 */
GEAPI void gix_app_set_window_borderless(GixApp* app, bool borderless);

/**
 * @brief Set GixApp window name
 *
 * @param app GixApp pointer
 * @param name Name of window
 */
GEAPI void gix_app_set_name(GixApp* app, const char* name);

/**
 * @brief Set GixApp window size
 *
 * @param app GixApp pointer
 * @param width Integer value of width
 * @param height Integer value of height
 */
GEAPI void gix_app_set_window_size(GixApp* app, i32 width, i32 height);

/**
 * @brief Get Window size of GixApp
 *
 * @param app GixApp pointer
 * @param width u32 pointer to hold window width value
 * @param height u32 pointer to hold window height value
 */
GEAPI void gix_app_get_window_size(GixApp* app, u32* width, u32* height);

/**
 * @brief Set GixApp window position
 *
 * @param app GixApp pointer
 * @param x Integer value of x position or use SDL_WINDOWPOS_CENTERED
 * @param y Integer value of y position or use SDL_WINDOWPOS_CENTERED
 */
GEAPI void gix_app_set_window_position(GixApp* app, i32 x, i32 y);

GEAPI void gix_app_set_window_resizeable(GixApp* app, bool resizeable);

/**
 * @brief Set GixApp loading scene
 *
 * @param app GixApp pointer
 * @param scene GixScene pointer
 */
GEAPI void gix_app_set_loading_scene(GixApp* app, GixScene* scene);

/**
 * @brief Set GixApp scene
 *
 * @param app GixApp pointer
 * @param scene GixScene pointer
 * @return SDL_AppResult. return SDL_APP_CONTINUE if success, SDL_APP_FAILURE if
 * failed
 */
GEAPI SDL_AppResult gix_app_set_scene(GixApp* app, GixScene* scene);

/**
 * @brief Get GixApp window
 *
 * @param app GixApp pointer
 * @return SDL_Window* SDL_Window pointer
 */
GEAPI SDL_Window* gix_app_get_window(GixApp* app);

/**
 * @brief Get GixApp GPU device
 *
 * @param app GixApp pointer
 * @return SDL_GPUDevice* SDL_GPUDevice pointer
 */
GEAPI SDL_GPUDevice* gix_app_get_gpu_device(GixApp* app);

GEAPI SDL_GPUTextureFormat gix_app_get_depth_texture_format(GixApp* app);

#ifdef __cplusplus
}
#endif

#endif /* __GIX_APP_ENGINE_H__ */
