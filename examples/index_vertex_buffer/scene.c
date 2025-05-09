#include "scene.h"

#include <cglm/cglm.h>

typedef struct {
    vec3 position;
    Uint8 color[4];
} MyVertex;

// create rect using MyVertex with index
// create vertices
MyVertex vertices_color[] = {
    {{-0.5f, 0.5f, 0.0f}, {255, 0, 0, 255}},
    {{0.5f, 0.5f, 0.0f}, {0, 255, 0, 255}},
    {{-0.5f, -0.5f, 0.0f}, {0, 0, 255, 255}},
    {{0.5f, -0.5f, 0.0f}, {255, 255, 0, 255}},
};

// Indices define two triangles that form a rectangle:
// Triangle 1: vertices 0, 1, 2
// Triangle 2: vertices 2, 1, 3
Uint16 indices[] = {
    0, 1, 2,  // First triangle
    2, 1, 3   // Second triangle
};

SDL_GPUBuffer *vertex_buffer;
SDL_GPUBuffer *index_buffer;

float rotate_angle = 0.0;
float rotate_speed = .10f;

typedef struct _uniform {
    mat4 mvp;
} uniform;

uniform uniform_data;

static void update_uniform_data(Uint64 delta_time) {
    mat4 model, view, projection;
    glm_mat4_identity(model);
    glm_mat4_identity(view);
    glm_mat4_identity(projection);

    rotate_angle += glm_rad(rotate_speed) * (float)delta_time;

    glm_rotate(model, rotate_angle, (vec3){0.0f, 1.0f, 0.0f});
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.001f, 1000.0f, projection);
    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f});
    glm_mat4_mulN((mat4 *[]){&projection, &view, &model}, 3, uniform_data.mvp);
}

static bool scene_init(GixScene *self) {
    gix_info("Init scene");
    // Init scene here

    SDL_GPUDevice *device = gix_app_get_gpu_device(self->app);
    SDL_Window *window = gix_app_get_window(self->app);
    // create 1 graphic pipeline
    self->numb_graphic_pipeline = 1;
    self->graphic_pipeline = SDL_malloc(sizeof(SDL_GPUGraphicsPipeline *));
    // load shader
    SDL_GPUShader *vertex_shader = gix_load_shader(device, "./shader/SPIRV/index_vertex.vert.spv",
                                                   SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, 0, 0);
    SDL_GPUShader *frag_shader = gix_load_shader(device, "./shader/SPIRV/index_vertex.frag.spv",
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 0, 0, 0);

    // 1. Create graphic pipeline
    // create color target description
    SDL_GPUColorTargetDescription color_target_desc[1] = {
        (SDL_GPUColorTargetDescription){
            .format = SDL_GetGPUSwapchainTextureFormat(device, window)}};
    // create vertex buffer description
    SDL_GPUVertexBufferDescription vertex_buffer_descs[1] = {
        {
            .slot = 0,
            .pitch = sizeof(MyVertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
        }};
    // create vertex attribute description
    SDL_GPUVertexAttribute vertex_attributes[2] = {
        {
            .buffer_slot = 0,
            .location = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(MyVertex, position),
        },
        {
            .buffer_slot = 0,
            .location = 1,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
            .offset = offsetof(MyVertex, color),
        }};
    // create verte input state
    SDL_GPUVertexInputState vertex_input_state = {
        .num_vertex_buffers = 1,
        .vertex_buffer_descriptions = vertex_buffer_descs,
        .num_vertex_attributes = 2,
        .vertex_attributes = vertex_attributes,
    };
    // create graphic pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = color_target_desc,
        },
        .vertex_input_state = vertex_input_state,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = vertex_shader,
        .fragment_shader = frag_shader,
    };
    self->graphic_pipeline[0] = SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);
    gix_if_null_exit(self->graphic_pipeline[0], gix_log_error("Couldn't create graphic pipeline"));
    // release shader
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, frag_shader);

    // 2. Create vertex buffer
    // create vertex buffer info
    SDL_GPUBufferCreateInfo buffer_info = {
        .size = sizeof(MyVertex) * 4,
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
    };
    // create vertex buffer
    vertex_buffer = SDL_CreateGPUBuffer(device, &buffer_info);
    gix_if_null_exit(vertex_buffer, gix_log_error("Couldn't create vertex buffer"));

    // 3. Create index buffer
    // create index buffer info
    SDL_GPUBufferCreateInfo index_buffer_info = {
        .size = sizeof(Uint16) * 6,
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
    };
    // create index buffer
    index_buffer = SDL_CreateGPUBuffer(device, &index_buffer_info);
    gix_if_null_exit(index_buffer, gix_log_error("Couldn't create index buffer"));

    // 4. Create transfer buffer
    // create transfer buffer create info
    SDL_GPUTransferBufferCreateInfo transfer_buffer_info = {
        .size = (sizeof(MyVertex) * 4) +
                (sizeof(Uint16) * 6),
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    };
    // create transfer buffer
    SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_info);
    gix_if_null_exit(transfer_buffer, gix_log_error("Couldn't create transfer buffer"));
    // map transfer buffer
    MyVertex *transfer_data = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    gix_if_null_exit(transfer_data, gix_log_error("Couldn't map transfer buffer"));
    // copy data to transfer buffer
    SDL_memcpy(transfer_data, vertices_color, sizeof(MyVertex) * 4);

    // Uint16 *transfer_index_data = (Uint16 *)(&transfer_data[4]);
    Uint16 *transfer_index_data = (Uint16 *)((Uint8*)transfer_data + sizeof(MyVertex) * 4);
    SDL_memcpy(transfer_index_data, indices, sizeof(Uint16) * 6);
    // unmap transfer buffer
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    // 5. Create command buffer
    SDL_GPUCommandBuffer *upload_cmd_buffer = SDL_AcquireGPUCommandBuffer(device);
    // 6. Create copy pass
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(upload_cmd_buffer);
    // 7. Upload buffer to GPU
    // upload vertex buffer
    SDL_GPUTransferBufferLocation src = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
    };

    SDL_GPUBufferRegion dst = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = sizeof(MyVertex) * 4,
    };
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
    // upload index buffer
    src.transfer_buffer = transfer_buffer;
    src.offset = sizeof(MyVertex) * 4;

    dst.buffer = index_buffer;
    dst.offset = 0;
    dst.size = sizeof(Uint16) * 6;
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
    // 8. End copy pass
    SDL_EndGPUCopyPass(copy_pass);
    // 9. submit command buffer
    if (!SDL_SubmitGPUCommandBuffer(upload_cmd_buffer)) {
        gix_log_error("Couldn't submit command buffer");
    }
    // 10. Release transfer buffer
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    return true;
}
static void scene_event(GixScene *self, const SDL_Event *event) {
    // Handle event here
}
static void scene_update(GixScene *self, Uint64 delta_time) {
    // Handle event here
    update_uniform_data(delta_time);
}
static void scene_draw(GixScene *self) {
    // Draw frame here
    SDL_GPUCommandBuffer *cmd_buffer = SDL_AcquireGPUCommandBuffer(gix_app_get_gpu_device(self->app));
    if (!cmd_buffer) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    // TODO! draw here
    SDL_GPUTexture *swapchain_texture;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, gix_app_get_window(self->app), &swapchain_texture, NULL, NULL)) {
        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture = swapchain_texture;
        colorTargetInfo.clear_color = (SDL_FColor){0.3f, 0.4f, 0.5f, 1.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(cmd_buffer, &colorTargetInfo, 1, NULL);
        // bind pipeline
        SDL_BindGPUGraphicsPipeline(render_pass, self->graphic_pipeline[0]);

        // bind vertex buffer
        SDL_GPUBufferBinding vertex_buffer_bindings[1] = {{
            .buffer = vertex_buffer,
            .offset = 0,
        }};
        SDL_BindGPUVertexBuffers(render_pass, 0, vertex_buffer_bindings, 1);

        // bind index buffer
        SDL_GPUBufferBinding index_buffer_binding = {
            .buffer = index_buffer,
            .offset = 0,
        };
        SDL_BindGPUIndexBuffer(render_pass, &index_buffer_binding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

        // pus uniform data
        SDL_PushGPUVertexUniformData(cmd_buffer, 0, &uniform_data, sizeof(uniform));
        
        // draw
        // SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);
        SDL_DrawGPUIndexedPrimitives(render_pass, 6, 1, 0, 0, 0);
        // end render pass
        SDL_EndGPURenderPass(render_pass);
    }
    SDL_SubmitGPUCommandBuffer(cmd_buffer);
}
static void scene_quit(GixScene *self) {
    // Handle quit here
    gix_info("Quit scene");
    SDL_ReleaseGPUBuffer(gix_app_get_gpu_device(self->app), vertex_buffer);
    SDL_ReleaseGPUBuffer(gix_app_get_gpu_device(self->app), index_buffer);
}

GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);
    gix_scene_impl(scene, scene_init, scene_event, scene_update, scene_draw, scene_quit);

    return scene;
}