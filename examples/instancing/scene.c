#include "scene.h"

#include <cglm/cglm.h>

#define NUMB_CUBE 1000

typedef struct _Camera {
    vec3 position;
    vec3 target;
} Camera;

typedef struct _VP {
    mat4 view;
    mat4 projection;
} VP;

typedef struct _CubeVertex {
    vec3 vertex;  // data vertex;
    Uint32 face;  // face index per cube
} CubeVertex;

typedef struct _MyScene {
    SDL_GPUBuffer *vertex_buffer;
    SDL_GPUBuffer *index_buffer;
    SDL_GPUBuffer *color_buffer;
    SDL_GPUBuffer *model_matrix_buffer;

    SDL_GPUTexture *depth_texture;

    SDL_GPUTransferBuffer *model_matrix_transfer_buffer;

    CubeVertex *cube_vertex;
    Uint16 numb_cube_vertex;

    Uint16 *indice_vertex;
    Uint16 numb_indice;

    vec4 *face_colors;
    Uint8 numb_face_color;

    VP vp;
    mat4 view_proj;
    mat4 model_matrix[NUMB_CUBE];

    Camera camera;

    // rotate
    bool is_rotate[NUMB_CUBE];
    float rotate_angle;
    float rotate_speed;
    Uint32 w;
    Uint32 h;

} MyScene;

static void init_my_scene(MyScene *scene);

static SDL_AppResult init(GixScene *self);
static SDL_AppResult event(GixScene *self,
                           const SDL_Event *event);
static SDL_AppResult update(GixScene *self,
                            Uint64 delta_time);
static SDL_AppResult draw(GixScene *self);
static void quit(GixScene *self);

static void update_model_matrix(MyScene *scene, Uint64 delta_time);

GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene, init, event, update, draw, quit);
    return scene;
}

SDL_AppResult init(GixScene *self) {
    gix_log("Init GixScene for Instancing");

    SDL_Window *window = gix_app_get_window(self->app);
    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);

    // create my scene
    MyScene *scene = SDL_malloc(sizeof(MyScene));
    self->user_data = scene;
    gix_app_get_window_size(self->app, &scene->w, &scene->h);
    gix_log("get size %ux%u", scene->w, scene->h);
    init_my_scene(scene);

    // get window size

    // create vertex buffer
    SDL_GPUBufferCreateInfo vertex_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(CubeVertex) * scene->numb_cube_vertex,  // each cube contains 24 vertex
    };
    scene->vertex_buffer = SDL_CreateGPUBuffer(device,
                                               &vertex_buffer_info);
    // create index buffer
    SDL_GPUBufferCreateInfo index_bufrer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
        .size = sizeof(Uint16) * scene->numb_indice,  // each cube contains 36 indices
    };
    scene->index_buffer = SDL_CreateGPUBuffer(device,
                                              &index_bufrer_info);
    // create color buffer
    SDL_GPUBufferCreateInfo color_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = sizeof(vec4) * scene->numb_face_color,
    };
    scene->color_buffer = SDL_CreateGPUBuffer(device,
                                              &color_buffer_info);
    // create model_matrix buffer
    SDL_GPUBufferCreateInfo model_mat_create_info = {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = sizeof(mat4) * NUMB_CUBE,
    };
    scene->model_matrix_buffer = SDL_CreateGPUBuffer(device,
                                                     &model_mat_create_info);

    // create depth texture
    SDL_GPUTextureFormat depth_format =
        gix_app_get_depth_texture_format(self->app);
    SDL_GPUTextureCreateInfo depth_texture_info = {
        .format = depth_format,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .height = scene->h,  // screen h
        .width = scene->w,   // screen w
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };
    scene->depth_texture =
        SDL_CreateGPUTexture(device, &depth_texture_info);

    // setup pipe line
    gix_scene_alloc_graphic_pipeline(self, 1);
    // load shader
    SDL_GPUShader *vertex_shader = gix_load_shader(
        device, "./shader/SPIRV/instancing.vert.spv",
        SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, 2, 0);
    SDL_GPUShader *frag_shader = gix_load_shader(
        device, "./shader/SPIRV/instancing.frag.spv",
        SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 0, 0, 0);
    // create color target description
    SDL_GPUColorTargetDescription color_target_desc[1] = {
        (SDL_GPUColorTargetDescription){
            .format = SDL_GetGPUSwapchainTextureFormat(device, window),
        },
    };
    // create vertex buffer description
    SDL_GPUVertexBufferDescription vertex_buffer_desc[1] = {
        {
            .slot = 0,
            .pitch = sizeof(CubeVertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
        },
    };
    // create vertex attributes desc
    SDL_GPUVertexAttribute vertex_attributes[2] = {
        {
            .buffer_slot = 0,
            .location = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(CubeVertex, vertex),
        },
        {
            .buffer_slot = 0,
            .location = 1,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UINT,
            .offset = offsetof(CubeVertex, face),
        },
    };
    // create vertex input state
    SDL_GPUVertexInputState vertex_input_state = {
        .num_vertex_buffers = 1,
        .vertex_buffer_descriptions = vertex_buffer_desc,
        .num_vertex_attributes = 2,
        .vertex_attributes = vertex_attributes,
    };
    // enable depth test
    SDL_GPUDepthStencilState depth_stencil = {
        .enable_depth_test = true,
        .enable_depth_write = true,
        .compare_op = SDL_GPU_COMPAREOP_LESS,
    };

    // create grphic pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = color_target_desc,
            .has_depth_stencil_target = true,
            .depth_stencil_format = depth_format,
        },
        .vertex_input_state = vertex_input_state,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = vertex_shader,
        .fragment_shader = frag_shader,
        .depth_stencil_state = depth_stencil,
    };

    self->graphic_pipeline[0] =
        SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);
    // release shader
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, frag_shader);

    // create model matrix transfer buffer
    SDL_GPUTransferBufferCreateInfo mm_transfer_buffer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(mat4) * NUMB_CUBE,
    };

    scene->model_matrix_transfer_buffer =
        SDL_CreateGPUTransferBuffer(device, &mm_transfer_buffer_info);

    // create transfer buffer
    SDL_GPUTransferBufferCreateInfo transfer_buffer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = (sizeof(CubeVertex) * scene->numb_cube_vertex) +
                (sizeof(Uint16) * scene->numb_indice) +
                (sizeof(vec4) * scene->numb_face_color),
    };
    SDL_GPUTransferBuffer *transfer_buffer =
        SDL_CreateGPUTransferBuffer(device, &transfer_buffer_info);
    // map transfer buffer
    CubeVertex *transfer_address =
        SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    // copy cube vertex data
    SDL_memcpy(transfer_address, scene->cube_vertex,
               sizeof(CubeVertex) * scene->numb_cube_vertex);
    // copy indice
    SDL_memcpy((Uint8 *)transfer_address +
                   (sizeof(CubeVertex) * scene->numb_cube_vertex),
               scene->indice_vertex,
               sizeof(Uint16) * scene->numb_indice);
    // copy face color
    SDL_memcpy((Uint8 *)transfer_address +
                   (sizeof(CubeVertex) * scene->numb_cube_vertex) +
                   (sizeof(Uint16) * scene->numb_indice),
               scene->face_colors,
               sizeof(vec4) * scene->numb_face_color);
    // unmap
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    // Upload buffer data
    SDL_GPUCommandBuffer *upload_cmd =
        SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(upload_cmd);
    // upload vertex
    SDL_GPUTransferBufferLocation src = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
    };
    SDL_GPUBufferRegion dst = {
        .buffer = scene->vertex_buffer,
        .offset = 0,
        .size = sizeof(CubeVertex) * scene->numb_cube_vertex,
    };
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
    // upload index
    src.offset = sizeof(CubeVertex) * scene->numb_cube_vertex;
    dst.buffer = scene->index_buffer;
    dst.offset = 0;
    dst.size = sizeof(Uint16) * scene->numb_indice;
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
    // upload color
    src.offset = (sizeof(CubeVertex) * scene->numb_cube_vertex) +
                 (sizeof(Uint16) * scene->numb_indice);
    dst.offset = 0;
    dst.buffer = scene->color_buffer;
    dst.size = sizeof(vec4) * scene->numb_face_color;
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
    // end copy pass
    SDL_EndGPUCopyPass(copy_pass);

    bool res = SDL_SubmitGPUCommandBuffer(upload_cmd);
    gix_if_return(!res,
                  gix_log_error("Couldn't submit upload cmd"),
                  SDL_APP_FAILURE);
    // release transfer buffer
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    // free vertex & face color after uploaded to GPU
    SDL_free(scene->cube_vertex);
    scene->cube_vertex = NULL;
    SDL_free(scene->indice_vertex);
    scene->indice_vertex = NULL;
    SDL_free(scene->face_colors);
    scene->face_colors = NULL;
    return SDL_APP_CONTINUE;
}

SDL_AppResult event(GixScene *self, const SDL_Event *event) {
    return SDL_APP_CONTINUE;
}

SDL_AppResult update(GixScene *self, Uint64 delta_time) {
    MyScene *scene = self->user_data;
    update_model_matrix(scene, delta_time);
    return SDL_APP_CONTINUE;
}

SDL_AppResult draw(GixScene *self) {
    SDL_Window *window = gix_app_get_window(self->app);
    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);
    MyScene *scene = self->user_data;

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);

    SDL_GPUTexture *swapchain_texture;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd,
                                              window,
                                              &swapchain_texture,
                                              NULL, NULL)) {
        // map model matrix transfer buffer
        mat4 *transfer_address =
            SDL_MapGPUTransferBuffer(device,
                                     scene->model_matrix_transfer_buffer,
                                     true);

        SDL_memcpy(transfer_address,
                   scene->model_matrix,
                   sizeof(mat4) * NUMB_CUBE);
        SDL_UnmapGPUTransferBuffer(device,
                                   scene->model_matrix_transfer_buffer);

        // upload to gpu
        SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);
        SDL_UploadToGPUBuffer(
            copy_pass,
            &(SDL_GPUTransferBufferLocation){
                .transfer_buffer = scene->model_matrix_transfer_buffer,
                .offset = 0,
            },
            &(SDL_GPUBufferRegion){
                .buffer = scene->model_matrix_buffer,
                .offset = 0,
                .size = sizeof(mat4) * NUMB_CUBE,
            },
            true);
        SDL_EndGPUCopyPass(copy_pass);

        // create render pass
        SDL_GPUColorTargetInfo color_target_info = {0};
        color_target_info.clear_color =
            (SDL_FColor){0.3f, 0.4f, 0.5f, 1.f};
        color_target_info.texture = swapchain_texture;
        color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        color_target_info.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depth_target_info = {0};
        depth_target_info.clear_depth = 1.0f;
        depth_target_info.texture = scene->depth_texture;
        depth_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        depth_target_info.store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_GPURenderPass *render_pass =
            SDL_BeginGPURenderPass(cmd, &color_target_info, 1,
                                   &depth_target_info);
        SDL_BindGPUGraphicsPipeline(render_pass,
                                    self->graphic_pipeline[0]);
        // bind vertex buffer
        SDL_GPUBufferBinding vertex_buffer_binding[1] = {
            {
                .buffer = scene->vertex_buffer,
                .offset = 0,
            },
        };

        SDL_BindGPUVertexBuffers(render_pass, 0,
                                 vertex_buffer_binding, 1);
        // bind index buffer
        SDL_GPUBufferBinding index_buffer_binding = {
            .buffer = scene->index_buffer,
            .offset = 0,
        };
        SDL_BindGPUIndexBuffer(render_pass,
                               &index_buffer_binding,
                               SDL_GPU_INDEXELEMENTSIZE_16BIT);
        // bind vertex storage buffer
        SDL_GPUBuffer *vertex_storage_buffer[2] = {
            scene->color_buffer,
            scene->model_matrix_buffer,
        };
        SDL_BindGPUVertexStorageBuffers(render_pass,
                                        0,
                                        vertex_storage_buffer, 2);
        // push uniform
        SDL_PushGPUVertexUniformData(cmd, 0, &(scene->view_proj),
                                     sizeof(mat4));

        // draw
        SDL_DrawGPUIndexedPrimitives(render_pass,
                                     scene->numb_indice,
                                     NUMB_CUBE, 0, 0, 0);

        SDL_EndGPURenderPass(render_pass);
    }
    SDL_SubmitGPUCommandBuffer(cmd);
    return SDL_APP_CONTINUE;
}

void quit(GixScene *self) {
    MyScene *scene = self->user_data;
    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);

    SDL_ReleaseGPUBuffer(device, scene->vertex_buffer);
    SDL_ReleaseGPUBuffer(device, scene->index_buffer);
    SDL_ReleaseGPUBuffer(device, scene->color_buffer);
    SDL_ReleaseGPUBuffer(device, scene->model_matrix_buffer);
    SDL_ReleaseGPUTexture(device, scene->depth_texture);
    SDL_ReleaseGPUTransferBuffer(device,
                                 scene->model_matrix_transfer_buffer);
}

static void init_my_scene(MyScene *scene) {
    // set numb_cube_vertex;
    scene->numb_cube_vertex = 24;
    // set *cube_vertex;
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
    scene->cube_vertex =
        SDL_malloc(sizeof(CubeVertex) * scene->numb_cube_vertex);
    SDL_memcpy(scene->cube_vertex,
               cube_vertex,
               sizeof(CubeVertex) * scene->numb_cube_vertex);

    // set numb_indice;
    scene->numb_indice = 36;
    // set *indice_vertex;
    Uint16 index_vertex_cube[36] = {
        // front (z = -0.5)
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
    scene->indice_vertex = SDL_malloc(sizeof(Uint16) *
                                      scene->numb_indice);
    SDL_memcpy(scene->indice_vertex,
               index_vertex_cube,
               sizeof(Uint16) * scene->numb_indice);

    // set numb_face_color;
    scene->numb_face_color = 6;
    // set *face_colors;
    vec4 color[] = {
        {1.f, 0.f, 0.f, 1.f},  // front
        {0.f, 1.f, 0.f, 1.f},  // right
        {0.f, 0.f, 1.f, 1.f},  // back
        {1.f, 1.f, 0.f, 1.f},  // left
        {0.f, 1.f, 1.f, 1.f},  // bottom
        {1.f, 0.f, 1.f, 1.f},  // top
    };
    scene->face_colors =
        SDL_malloc(sizeof(vec4) * scene->numb_face_color);
    SDL_memcpy(scene->face_colors, color,
               sizeof(vec4) * scene->numb_face_color);

    // set camera
    glm_vec3_copy((vec3){0.f, 0.f, 25.f},
                  scene->camera.position);
    glm_vec3_copy((vec3){0.f, 0.f, 0.f},
                  scene->camera.target);
    // set vp;
    mat4 init = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_copy(init, scene->vp.view);
    glm_mat4_copy(init, scene->vp.projection);
    glm_perspective(glm_rad(45.0f),
                    (float)scene->w / (float)scene->h,
                    0.001f, 1000.0f, scene->vp.projection);

    // set rotate angle
    scene->rotate_angle = 0.0f;
    // set rotate speed
    scene->rotate_speed = 0.1f;

    // set model_matrix[NUMB_CUBE];
    for (Uint16 i = 0; i < NUMB_CUBE; i++) {
        glm_mat4_copy(init, scene->model_matrix[i]);
        float a = (float)SDL_rand(scene->w * 2);
        float x = (float)(a - scene->w) /
                  80.f;
        float b = (float)SDL_rand(scene->h);

        float y = (float)(b) /
                  60.f;
        gix_log("x: %f=>%f", a, x);
        gix_log("y: %f=>%f", b, y);
        vec3 pos = {
            x,
            y,
            (float)(SDL_rand(800)) / 80.0f,
        };
        glm_translate(scene->model_matrix[i], pos);
        if (i % 5) {
            scene->is_rotate[i] = true;
        }
    }
}

static void update_model_matrix(MyScene *scene,
                                Uint64 delta_time) {
    glm_lookat(scene->camera.position,
               scene->camera.target,
               (vec3){0.f, 1.f, 0.f},
               scene->vp.view);
    // glm_translate(scene->vp.view, (vec3){0.0f, 0.0f, -5.0f});

    glm_mat4_mulN((mat4 *[]){&(scene->vp.projection),
                             &(scene->vp.view)},
                  2, scene->view_proj);

    scene->rotate_angle = glm_rad(scene->rotate_speed) *
                          (float)delta_time;

    for (Uint16 i = 0; i < NUMB_CUBE; i++) {
        if (scene->is_rotate[i]) {
            glm_rotate(scene->model_matrix[i],
                       scene->rotate_angle,
                       (vec3){0.0f, 1.0f, 0.0f});
        }
    }
}