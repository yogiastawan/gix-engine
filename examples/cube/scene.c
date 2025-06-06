#include "scene.h"

#include <cglm/cglm.h>

// vertex=>data per pixel
typedef struct _Vertex {
    vec3 vertice;
} Vertex;

// 24 vertex: 4 per face, urut per face
Vertex cube[] = {
    // front (z = -0.5)
    {{-0.5f, -0.5f, -0.5f}},  // 0
    {{0.5f, -0.5f, -0.5f}},   // 1
    {{0.5f, 0.5f, -0.5f}},    // 2
    {{-0.5f, 0.5f, -0.5f}},   // 3
    // right (x = 0.5)
    {{0.5f, -0.5f, -0.5f}},  // 4
    {{0.5f, -0.5f, 0.5f}},   // 5
    {{0.5f, 0.5f, 0.5f}},    // 6
    {{0.5f, 0.5f, -0.5f}},   // 7
    // back (z = 0.5)
    {{0.5f, -0.5f, 0.5f}},   // 8
    {{-0.5f, -0.5f, 0.5f}},  // 9
    {{-0.5f, 0.5f, 0.5f}},   // 10
    {{0.5f, 0.5f, 0.5f}},    // 11
    // left (x = -0.5)
    {{-0.5f, -0.5f, 0.5f}},   // 12
    {{-0.5f, -0.5f, -0.5f}},  // 13
    {{-0.5f, 0.5f, -0.5f}},   // 14
    {{-0.5f, 0.5f, 0.5f}},    // 15
    // bottom (y = -0.5)
    {{-0.5f, -0.5f, 0.5f}},   // 16
    {{0.5f, -0.5f, 0.5f}},    // 17
    {{0.5f, -0.5f, -0.5f}},   // 18
    {{-0.5f, -0.5f, -0.5f}},  // 19
    // top (y = 0.5)
    {{-0.5f, 0.5f, -0.5f}},  // 20
    {{0.5f, 0.5f, -0.5f}},   // 21
    {{0.5f, 0.5f, 0.5f}},    // 22
    {{-0.5f, 0.5f, 0.5f}},   // 23
};

// 36 index: 2 triangle per face, 6 face
Uint16 index_cube[36] = {
    // front
    // 0, 1, 2, 2, 3, 0,
    0, 2, 1, 0, 3, 2,
    // right
    4, 5, 6, 6, 7, 4,
    // back
    8, 9, 10, 10, 11, 8,
    // left
    12, 13, 14, 14, 15, 12,
    // bottom
    16, 17, 18, 18, 19, 16,
    // top
    20, 21, 22, 22, 23, 20};

// uniform color
typedef struct ColorFace {
    vec4 color[6];
} ColorFace;

// uniform mvp
typedef struct _MPV {
    mat4 mpv;
} MPV;

ColorFace colors = {
    .color = {
        {1.f, 0.f, 0.f, 1.f},  // front
        {0.f, 1.f, 0.f, 1.f},  // right
        {0.f, 0.f, 1.f, 1.f},  // back
        {1.f, 1.f, 0.f, 1.f},  // left
        {0.f, 1.f, 1.f, 1.f},  // bottom
        {1.f, 0.f, 1.f, 1.f},  // top
    },
};

MPV mpv;

float rotate_angle = 0.f;
float rotate_speed = 0.1f;

SDL_GPUBuffer *vertex_buffer;
SDL_GPUBuffer *index_buffer;
SDL_GPUBuffer *color_buffer;
SDL_GPUTexture *depth_texture;

static void update_uniform_data(Uint64 delta_time) {
    mat4 model, view, projection;
    glm_mat4_identity(model);
    glm_mat4_identity(view);
    glm_mat4_identity(projection);

    rotate_angle += glm_rad(rotate_speed) * (float)delta_time;

    glm_rotate(model, rotate_angle, (vec3){0.0f, 1.0f, 0.0f});
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.001f, 1000.0f, projection);
    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f});
    glm_mat4_mulN((mat4 *[]){&projection, &view, &model}, 3, mpv.mpv);
}

static SDL_AppResult
scene_init(GixScene *self) {
    gix_log("Init cube scene");

    SDL_Window *window = gix_app_get_window(self->app);
    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);

    // create depth texture
    SDL_GPUTextureFormat depth_format = gix_app_get_depth_texture_format(self->app);
    SDL_GPUTextureCreateInfo depth_texture_info = {
        .format = depth_format,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .height = 600,  // screen h
        .width = 800,   // screen w
        .layer_count_or_depth = 1,
        .num_levels = 1,
    };
    depth_texture = SDL_CreateGPUTexture(device, &depth_texture_info);

    // create 1 pipeline
    gix_scene_alloc_graphic_pipeline(self, 1);

    // load shader
    SDL_GPUShader *vertex_shader = gix_load_shader(device,
                                                   "./shader/SPIRV/cube.vert.spv",
                                                   "./shader/SPIRV/cube.vert.json",
                                                   SDL_GPU_SHADERSTAGE_VERTEX);

    SDL_GPUShader *fragment_shader = gix_load_shader(device,
                                                     "./shader/SPIRV/cube.frag.spv",
                                                     "./shader/SPIRV/cube.frag.json",
                                                     SDL_GPU_SHADERSTAGE_FRAGMENT);

    // create pipeline
    SDL_GPUColorTargetDescription color_target[] = {
        (SDL_GPUColorTargetDescription){
            .format = SDL_GetGPUSwapchainTextureFormat(device, window),
        },
    };
    SDL_GPUVertexBufferDescription vertex_buffer_desc[] = {
        {
            .slot = 0,
            .pitch = sizeof(Vertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
        },
    };

    SDL_GPUVertexAttribute vertex_attributes[] = {
        {
            .buffer_slot = 0,
            .location = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(Vertex, vertice),
        },
    };
    SDL_GPUVertexInputState input_state = {
        .num_vertex_buffers = 1,
        .vertex_buffer_descriptions = vertex_buffer_desc,
        .num_vertex_attributes = 1,
        .vertex_attributes = vertex_attributes,
    };

    SDL_GPUDepthStencilState depth_stencil = {
        .enable_depth_test = true,
        .enable_depth_write = true,
        .compare_op = SDL_GPU_COMPAREOP_LESS,
    };

    SDL_GPUGraphicsPipelineCreateInfo
        pipeline_info = {
            .target_info = {
                .num_color_targets = 1,
                .color_target_descriptions = color_target,
                .has_depth_stencil_target = true,
                .depth_stencil_format = depth_format,
            },
            .vertex_input_state = input_state,
            .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
            .vertex_shader = vertex_shader,
            .fragment_shader = fragment_shader,
            .depth_stencil_state = depth_stencil,
        };

    self->graphic_pipeline[0] = SDL_CreateGPUGraphicsPipeline(device,
                                                              &pipeline_info);

    gix_if_return(!self->graphic_pipeline[0],
                  gix_log_error("Couldn't create pipeline 0"),
                  SDL_APP_FAILURE);
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, fragment_shader);

    // create vertex buffer
    SDL_GPUBufferCreateInfo create_buffer_info = {
        .size = sizeof(Vertex) * 24,
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
    };
    vertex_buffer = SDL_CreateGPUBuffer(device, &create_buffer_info);
    // create index buffer
    SDL_GPUBufferCreateInfo index_buffer_info = {
        .size = sizeof(Uint16) * 36,
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
    };
    index_buffer = SDL_CreateGPUBuffer(device, &index_buffer_info);

    // create color buffer
    SDL_GPUBufferCreateInfo color_buffer_info = {
        .size = sizeof(vec4) * 6,
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
    };
    color_buffer = SDL_CreateGPUBuffer(device, &color_buffer_info);

    // create transfer buffer for vertex and index
    SDL_GPUTransferBufferCreateInfo transfer_buffer_info = {
        .size = sizeof(Vertex) * 24 + sizeof(Uint16) * 36 + sizeof(vec4) * 6,
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    };
    SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_info);
    gix_if_return(!transfer_buffer, gix_log_error("Couldn't create transfer buffer"), SDL_APP_FAILURE);

    // map transfer data
    Vertex *transfer_data = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    gix_if_return(!transfer_data, gix_log_error("Couldn't map transfer buffer"), SDL_APP_FAILURE);

    // copy Vertex
    SDL_memcpy(transfer_data, cube, sizeof(Vertex) * 24);
    // copy index
    SDL_memcpy(&transfer_data[24], index_cube, sizeof(Uint16) * 36);
    // copy
    SDL_memcpy((Uint8 *)transfer_data + sizeof(Vertex) * 24 + sizeof(Uint16) * 36, colors.color, sizeof(vec4) * 6);
    // unmap
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    SDL_GPUCommandBuffer *uploda_cmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass *cp = SDL_BeginGPUCopyPass(uploda_cmd);

    // upload vertex buffer
    SDL_GPUTransferBufferLocation src = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
    };

    SDL_GPUBufferRegion dst = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = sizeof(Vertex) * 24,
    };

    SDL_UploadToGPUBuffer(cp, &src, &dst, false);

    // upload index buffer
    src.offset = sizeof(Vertex) * 24;
    dst.offset = 0;
    dst.buffer = index_buffer;
    dst.size = sizeof(Uint16) * 36;
    SDL_UploadToGPUBuffer(cp, &src, &dst, false);

    // upload color buffer
    src.offset = sizeof(Vertex) * 24 + sizeof(Uint16) * 36;
    dst.offset = 0;
    dst.buffer = color_buffer;
    dst.size = sizeof(vec4) * 6;
    SDL_UploadToGPUBuffer(cp, &src, &dst, false);

    SDL_EndGPUCopyPass(cp);

    // SDL_GPURenderPass *render_pass =
    //     SDL_BeginGPURenderPass(uploda_cmd, NULL, 0, NULL);
    // SDL_BindGPUGraphicsPipeline(render_pass, self->graphic_pipeline[0]);

    // SDL_EndGPURenderPass(render_pass);
    bool res = SDL_SubmitGPUCommandBuffer(uploda_cmd);

    gix_if_return(!res, gix_log_error("couldn't submit cmd"), SDL_APP_FAILURE);

    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    return SDL_APP_CONTINUE;
}

static SDL_AppResult
scene_handle_event(GixScene *self, const SDL_Event *event) {
    return SDL_APP_CONTINUE;
}

static SDL_AppResult scene_update(GixScene *self, Uint64 delta_time) {
    update_uniform_data(delta_time);

    return SDL_APP_CONTINUE;
}

static SDL_AppResult scene_draw(GixScene *self) {
    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gix_app_get_gpu_device(self->app));
    gix_if_return(!cmd, gix_log_error("couldn't create cmd buffer"), SDL_APP_FAILURE);

    SDL_GPUTexture *swapchain_texture;

    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd, gix_app_get_window(self->app),
                                              &swapchain_texture, NULL, NULL)) {
        SDL_GPUColorTargetInfo target_info = {0};
        target_info.clear_color = (SDL_FColor){0.3f, 0.4f, 0.5f, 1.0f};
        target_info.texture = swapchain_texture;
        target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        target_info.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depth_target_info = {0};
        depth_target_info.clear_depth = 1.0f,
        depth_target_info.texture = depth_texture,
        depth_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        depth_target_info.store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_GPURenderPass *render_pass =
            SDL_BeginGPURenderPass(cmd, &target_info, 1, &depth_target_info);
        SDL_BindGPUGraphicsPipeline(render_pass, self->graphic_pipeline[0]);

        // bindn vertex buffer
        SDL_GPUBufferBinding vertex_buffer_binding[] = {
            {
                .buffer = vertex_buffer,
                .offset = 0,
            },
        };
        SDL_BindGPUVertexBuffers(render_pass, 0, vertex_buffer_binding, 1);

        // bind index buffer
        SDL_GPUBufferBinding index_buffer_binding = {
            .buffer = index_buffer,
            .offset = 0,
        };
        SDL_BindGPUIndexBuffer(render_pass, &index_buffer_binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

        SDL_BindGPUVertexStorageBuffers(render_pass, 0, &color_buffer, 1);
        // push uniform mpv
        SDL_PushGPUVertexUniformData(cmd, 0, &mpv, sizeof(MPV));

        SDL_DrawGPUIndexedPrimitives(render_pass, 36, 1, 0, 0, 0);

        SDL_EndGPURenderPass(render_pass);
    }

    SDL_SubmitGPUCommandBuffer(cmd);

    return SDL_APP_CONTINUE;
}

static void scene_quit(GixScene *self) {
    gix_log("Quit scene");
    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);
    SDL_ReleaseGPUBuffer(device, vertex_buffer);
    SDL_ReleaseGPUBuffer(device, index_buffer);
    SDL_ReleaseGPUBuffer(device, color_buffer);
    SDL_ReleaseGPUTexture(device, depth_texture);
}

GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene,
                   scene_init, scene_handle_event, scene_update, scene_draw, scene_quit);
    return scene;
}
