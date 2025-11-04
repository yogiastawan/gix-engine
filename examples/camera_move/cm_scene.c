#include "cm_scene.h"

#include <SDL3/SDL.h>
#include <gix_engine/gix_engine.h>

static void init_data(CMScene* cm) {
    // set cube_vertex;
    cm->numb_cube_vertex = 24;

    CubeVertex cube_vertex[] = {
        // front (z = 0.5)
        {{-0.5f, -0.5f, 0.5f}, 0},  // 0
        {{0.5f, -0.5f, 0.5f}, 0},   // 1
        {{0.5f, 0.5f, 0.5f}, 0},    // 2
        {{-0.5f, 0.5f, 0.5f}, 0},   // 3
        // right (x = 0.5)
        {{0.5f, -0.5f, -0.5f}, 1},  // 4
        {{0.5f, -0.5f, 0.5f}, 1},   // 5
        {{0.5f, 0.5f, 0.5f}, 1},    // 6
        {{0.5f, 0.5f, -0.5f}, 1},   // 7
        // back (z = -0.5)
        {{0.5f, -0.5f, -0.5f}, 2},   // 8
        {{-0.5f, -0.5f, -0.5f}, 2},  // 9
        {{-0.5f, 0.5f, -0.5f}, 2},   // 10
        {{0.5f, 0.5f, -0.5f}, 2},    // 11
        // left (x = -0.5)
        {{-0.5f, -0.5f, 0.5f}, 3},   // 12
        {{-0.5f, -0.5f, -0.5f}, 3},  // 13
        {{-0.5f, 0.5f, -0.5f}, 3},   // 14
        {{-0.5f, 0.5f, 0.5f}, 3},    // 15
        // bottom (y = -0.5)
        {{-0.5f, -0.5f, 0.5f}, 4},   // 16
        {{0.5f, -0.5f, 0.5f}, 4},    // 17
        {{0.5f, -0.5f, -0.5f}, 4},   // 18
        {{-0.5f, -0.5f, -0.5f}, 4},  // 19
        // top (y = 0.5)
        {{-0.5f, 0.5f, -0.5f}, 5},  // 20
        {{0.5f, 0.5f, -0.5f}, 5},   // 21
        {{0.5f, 0.5f, 0.5f}, 5},    // 22
        {{-0.5f, 0.5f, 0.5f}, 5},   // 23
    };

    cm->cube_vertex = SDL_malloc(sizeof(CubeVertex) * cm->numb_cube_vertex);
    SDL_memcpy(cm->cube_vertex, cube_vertex,
               sizeof(CubeVertex) * cm->numb_cube_vertex);

    // numb_indice_vertex
    cm->numb_indice_vertex = 36;
    Uint16 indice_vertex_cube[] = {// front (z = -0.5)
                                   0, 1, 2, 0, 2, 3,
                                   // right (x = 0.5)
                                   4, 5, 6, 4, 6, 7,
                                   // back (z = 0.5)
                                   8, 11, 10, 8, 10, 9,
                                   // left (x = -0.5)
                                   12, 13, 14, 12, 14, 15,
                                   // bottom (y = -0.5)
                                   16, 17, 18, 16, 18, 19,
                                   // top (y = 0.5)
                                   20, 23, 22, 20, 22, 21};
    cm->indice_vertex = SDL_malloc(sizeof(Uint16) * cm->numb_indice_vertex);
    SDL_memcpy(cm->indice_vertex, indice_vertex_cube,
               sizeof(Uint16) * cm->numb_indice_vertex);

    // face_color
    cm->numb_face_color = 6;
    vec4 face_color[] = {
        {1.f, 0.f, 0.f, 1.f},  // front
        {0.f, 1.f, 0.f, 1.f},  // right
        {0.f, 0.f, 1.f, 1.f},  // back
        {1.f, 1.f, 0.f, 1.f},  // left
        {0.f, 1.f, 1.f, 1.f},  // bottom
        {1.f, 0.f, 1.f, 1.f},  // top

    };
    cm->face_color = SDL_malloc(sizeof(vec4) * cm->numb_face_color);
    SDL_memcpy(cm->face_color, face_color, sizeof(vec4) * cm->numb_face_color);

    // set rotate angle
    // set rotate speed
    cm->rotate_speed = 0.1f;

    // mvp
    glm_vec3_copy((vec3){0.f, 3.f, 25.f}, cm->camera.position);
    glm_vec3_copy((vec3){0.f, 0.f, 0.f}, cm->camera.target);

    mat4 init = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_copy(init, cm->vp.view);
    glm_mat4_copy(init, cm->vp.proj);

    glm_perspective(glm_rad(45.f), (float)cm->width / (float)cm->height, 0.001f,
                    1000.f, cm->vp.proj);

    // set position cube & rotate angle each cube
    cm->rotate_angle = SDL_malloc(sizeof(float) * cm->numb_cube);
    cm->position_cube = SDL_malloc(sizeof(vec4) * cm->numb_cube);
    for (Uint32 i = 0; i < cm->numb_cube; ++i) {
        cm->rotate_angle[i] = 0;

        float a = (float)SDL_rand(cm->width * 2);
        float x = (float)(a - cm->width) / 80.f;
        float b = (float)SDL_rand(cm->height);

        float y = (float)(b) / 60.f;
        // gix_log("x: %f=>%f", a, x);
        // gix_log("y: %f=>%f", b, y);
        vec4 pos = {
            x,
            0.5f + y,  // 0.5 to translate to 0 position
            (float)SDL_rand(600) / 60.f,
            1.0f,
        };
        glm_vec4_copy(pos, cm->position_cube[i]);
    }
}

void cm_scene_init(CMScene* cm, SDL_GPUDevice* device,
                   SDL_GPUTextureFormat format, Uint32 w, Uint32 h) {
    cm->numb_cube = NUMB_CUBE;
    // create vertex_buffer
    SDL_GPUBufferCreateInfo vertex_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(CubeVertex) * cm->numb_cube_vertex,
    };
    cm->vertex_buffer = SDL_CreateGPUBuffer(device, &vertex_buffer_info);

    // create index_buffer
    SDL_GPUBufferCreateInfo index_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
        .size = sizeof(Uint16) * cm->numb_indice_vertex,
    };
    cm->index_buffer = SDL_CreateGPUBuffer(device, &index_buffer_info);
    // create color_buffer
    SDL_GPUBufferCreateInfo color_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = sizeof(vec4) * cm->numb_face_color,
    };
    cm->color_buffer = SDL_CreateGPUBuffer(device, &color_buffer_info);
    // create rotate_angle_buffer
    SDL_GPUBufferCreateInfo rotate_angle_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = sizeof(float) * cm->numb_cube,
    };
    cm->rotate_angle_buffer =
        SDL_CreateGPUBuffer(device, &rotate_angle_buffer_info);

    // create position_cube_buffer
    SDL_GPUBufferCreateInfo position_cube_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = sizeof(vec4) * cm->numb_cube,
    };
    cm->position_cube_buffer =
        SDL_CreateGPUBuffer(device, &position_cube_buffer_info);

    // create depth_texture
    SDL_GPUTextureCreateInfo depth_texture_info = {
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .format = format,
        .height = h,
        .width = w,
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };
    cm->depth_texture = SDL_CreateGPUTexture(device, &depth_texture_info);

    // create transfer buffer for rotate_angle_buffer
    SDL_GPUTransferBufferCreateInfo rotate_angle_tf_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(float) * NUMB_CUBE,
    };
    cm->rotate_transfer_buffer =
        SDL_CreateGPUTransferBuffer(device, &rotate_angle_tf_info);
}
void cm_scene_destroy(CMScene* cm, SDL_GPUDevice* device) {
    SDL_ReleaseGPUBuffer(device, cm->vertex_buffer);
    SDL_ReleaseGPUBuffer(device, cm->color_buffer);
    SDL_ReleaseGPUBuffer(device, cm->index_buffer);
    SDL_ReleaseGPUBuffer(device, cm->rotate_angle_buffer);
    SDL_ReleaseGPUTexture(device, cm->depth_texture);
    SDL_ReleaseGPUBuffer(device, cm->position_cube_buffer);
    SDL_ReleaseGPUTransferBuffer(device, cm->rotate_transfer_buffer);

    SDL_free(cm->cube_vertex);
    SDL_free(cm->indice_vertex);
    SDL_free(cm->face_color);
    SDL_free(cm->position_cube);
    SDL_free(cm->rotate_angle);

    SDL_free(cm);
}
