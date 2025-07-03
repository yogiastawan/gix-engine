#ifndef __CM_SCENE_H__
#define __CM_SCENE_H__

#include <cglm/cglm.h>
#include <gix_engine/gix_engine.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUMB_CUBE 1000

typedef struct _CubeVertex {
    vec3 vertex;
    Uint32 face_id;
} CubeVertex;

typedef struct _ViewProj {
    mat4 view;
    mat4 proj;
} ViewProj;

typedef struct _Camera {
    vec3 position;
    vec3 target;
} Camera;

typedef struct _CMScene {
    SDL_GPUBuffer *vertex_buffer;
    SDL_GPUBuffer *index_buffer;
    SDL_GPUBuffer *color_buffer;
    SDL_GPUBuffer *rotate_angle_buffer;

    SDL_GPUBuffer *position_cube_buffer;

    SDL_GPUTexture *depth_texture;

    SDL_GPUTransferBuffer *rotate_transfer_buffer;

    CubeVertex *cube_vertex;
    Uint16 numb_cube_vertex;

    Uint16 *indice_vertex;
    Uint16 numb_indice_vertex;
    vec4 *face_color;
    Uint16 numb_face_color;

    Camera camera;
    ViewProj vp;
    mat4 view_proj;

    // position each cube
    vec4 *position_cube;
    // rotate_angle each cube
    float *rotate_angle;
    Uint32 numb_cube;
    float rotate_speed;
    Uint32 width;
    Uint32 height;

} CMScene;

void cm_scene_init(CMScene *cm, SDL_GPUDevice *device,
                   SDL_GPUTextureFormat format, Uint32 w, Uint32 h);
void cm_scene_destroy(CMScene *cm, SDL_GPUDevice *device);

#ifdef __cplusplus
}
#endif

#endif /* __CM_SCENE_H__ */
