#include "scene.h"

#include <cglm/cglm.h>

enum CubeID {
    STATIC_CUBE = 0,
    ROTATED_CUBE,
    NUM_CUBE_ID,
};

typedef struct _MVP {
    mat4 model;
    mat4 view;
    mat4 projection;
} MVP;

typedef struct _CubeVertex {
    vec3 vertex;       // data vertex;
    Uint8 face;        // face index per cube
    Uint16 object_id;  // id per cube
} CubeVertex;

typedef struct _MyScene {
    SDL_GPUBuffer *vertex_buffer;
    SDL_GPUBuffer *index_buffer;
    SDL_GPUBuffer *color_buffer;
    SDL_GPUTexture *depth_texture;

    CubeVertex *cube_vertex;
    Uint16 numb_cube;

    vec4 *face_colors;
    Uint8 numb_face_color;

    MVP mvp[NUM_CUBE_ID];

} MyScene;

static void init_my_scene(MyScene *scene);
static void destroy_my_scene(MyScene *scene);

static SDL_AppResult init(GixScene *self);
static SDL_AppResult event(GixScene *self, const SDL_Event *event);

static SDL_AppResult update(GixScene *self, Uint64 delta_time);

static SDL_AppResult draw(GixScene *self);

static void quit(GixScene *self);

GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene, init, event, update, draw, quit);
    return scene;
}

SDL_AppResult init(GixScene *self) {
    gix_log("Init GixScene for Camera Move 2");

    SDL_Window *window = gix_app_get_window(self->app);
    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);

    // create my scene
    MyScene *scene = SDL_malloc(sizeof(MyScene));
    init_my_scene(scene);
    self->user_data = scene;

    // setup pipe line
    gix_scene_alloc_graphic_pipeline(self, 1);

    // free vertex & face color after uploaded to GPU
    SDL_free(scene->cube_vertex);
    SDL_free(scene->face_colors);
    return SDL_APP_CONTINUE;
}

SDL_AppResult event(GixScene *self, const SDL_Event *event) {}

SDL_AppResult update(GixScene *self, Uint64 delta_time) {}

SDL_AppResult draw(GixScene *self) {
}

void quit(GixScene *self) {
    MyScene *scene = self->user_data;
    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);

    SDL_ReleaseGPUBuffer(device, scene->vertex_buffer);
    SDL_ReleaseGPUBuffer(device, scene->index_buffer);
    SDL_ReleaseGPUBuffer(device, scene->color_buffer);
    SDL_ReleaseGPUTexture(device, scene->depth_texture);

    destroy_my_scene(scene);
}

static void init_my_scene(MyScene *scene) {
}
static void destroy_my_scene(MyScene *scene) {
    free(scene);
}