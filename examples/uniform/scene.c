#include "scene.h"

#include <cglm/cglm.h>
#include <stdio.h>

// buffer
SDL_GPUBuffer* vertex_buffer;

typedef struct {
    vec3 position;
    Uint8 color[4];
} MyVertex;

MyVertex vertices_color[] = {
    {{0.0f, 0.5f, 0.0f}, {255, 0, 0, 255}},
    {{-0.5f, -0.5f, 0.0f}, {0, 255, 0, 255}},
    {{0.5f, -0.5f, 0.0f}, {0, 0, 255, 255}},
};

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
    glm_translate(view, (vec3){0.0f, 0.0f, -2.0f});
    glm_mat4_mulN((mat4*[]){&projection, &view, &model}, 3, uniform_data.mvp);
}

static SDL_AppResult scene_init(GixScene* self) {
    gix_log("Init uniform scene: %p", self);

    SDL_GPUDevice* device = gix_app_get_gpu_device(self->app);
    SDL_Window* window = gix_app_get_window(self->app);
    // Init scene here
    self->graphic_pipeline = SDL_malloc(sizeof(SDL_GPUGraphicsPipeline*));
    self->numb_graphic_pipeline = 1;

    // load shader
    SDL_GPUShader* vertex_shader = gix_load_shader(device, "./shader/SPIRV/uniform.vert.spv",
                                                   SDL_GPU_SHADERSTAGE_VERTEX, 0, 1, 0, 0);
    SDL_GPUShader* frag_shader = gix_load_shader(device, "./shader/SPIRV/uniform.frag.spv",
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT, 0, 0, 0, 0);

    gix_log("vertex_shader: %p, frag_shader: %p", vertex_shader, frag_shader);
    gix_log("graphic_pipeline ptr: %p", self->graphic_pipeline);
    //  pipeline color target description
    SDL_GPUColorTargetDescription color_target_desc[] = {
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
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, frag_shader);

    // create vertex buffer info
    SDL_GPUBufferCreateInfo buffer_info = {
        .size = sizeof(MyVertex) * 3,
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
    };
    // create vertex buffer
    vertex_buffer = SDL_CreateGPUBuffer(device, &buffer_info);
    gix_if_null_exit(vertex_buffer, gix_log_error("Couldn't create vertex buffer"));
    // create transfer buffer create info
    SDL_GPUTransferBufferCreateInfo transfer_buffer_info = {
        .size = sizeof(MyVertex) * 3,
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
    };
    // create transfer buffer
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_info);
    // map gpu tranfer buffer
    MyVertex* transfer_data = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    gix_if_null_exit(transfer_data, gix_log_error("Couldn't map transfer buffer"));
    // copy data to transfer buffer
    SDL_memcpy(transfer_data, vertices_color, sizeof(MyVertex) * 3);
    // unmap transfer buffer
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    // upload transfer data to vertex buffer

    //  create command buffer
    SDL_GPUCommandBuffer* upload_cmd_buffer = SDL_AcquireGPUCommandBuffer(device);
    if (!upload_cmd_buffer) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    // create copy pass command
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_cmd_buffer);
    // create transfer buffer location source
    SDL_GPUTransferBufferLocation transfer_buffer_location = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
    };
    // create vertex buffer region destionation
    SDL_GPUBufferRegion vertex_buffer_region = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = sizeof(MyVertex) * 3,
    };
    // upload
    SDL_UploadToGPUBuffer(copy_pass, &transfer_buffer_location, &vertex_buffer_region, false);
    // end copy pass
    SDL_EndGPUCopyPass(copy_pass);
    // submit command buffer
    if (!SDL_SubmitGPUCommandBuffer(upload_cmd_buffer)) {
        gix_log_error("Couldn't submit command buffer");
    }

    // release transfer buffer
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
    return SDL_APP_CONTINUE;
}

static SDL_AppResult scene_event(GixScene* self, const SDL_Event* event) {
    // Handle event here
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_UP) {
                rotate_speed += 0.1f;
            } else if (event->key.key == SDLK_DOWN) {
                rotate_speed -= 0.1f;
            } else if (event->key.key == SDLK_ESCAPE) {
                return SDL_APP_SUCCESS;
            }
            break;

        default:
            break;
    }

    return SDL_APP_CONTINUE;
}
static SDL_AppResult scene_update(GixScene* self, Uint64 delta_time) {
    update_uniform_data(delta_time);
    return SDL_APP_CONTINUE;
}

static SDL_AppResult scene_draw(GixScene* self) {
    // Draw frame here
    SDL_GPUCommandBuffer* cmd_buffer = SDL_AcquireGPUCommandBuffer(gix_app_get_gpu_device(self->app));
    if (!cmd_buffer) {
        gix_log_error("Couldn't aquire GPU command buffer");
    }
    SDL_GPUTexture* swapchain_texture;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, gix_app_get_window(self->app), &swapchain_texture, NULL, NULL)) {
        SDL_GPUColorTargetInfo colorTargetInfo = {0};
        colorTargetInfo.texture = swapchain_texture;
        colorTargetInfo.clear_color = (SDL_FColor){0.3f, 0.4f, 0.5f, 1.0f};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmd_buffer, &colorTargetInfo, 1, NULL);
        // bind pipeline
        SDL_BindGPUGraphicsPipeline(renderPass, self->graphic_pipeline[0]);
        // bind vertex buffer
        SDL_GPUBufferBinding buffer_binding[1] = {{
            .buffer = vertex_buffer,
            .offset = 0,
        }};
        SDL_BindGPUVertexBuffers(renderPass, 0, buffer_binding, 1);

        // push uniform data
        SDL_PushGPUVertexUniformData(cmd_buffer, 0, &uniform_data, sizeof(uniform));

        // draw
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(cmd_buffer);

    return SDL_APP_CONTINUE;
}
static void vertex_buffer_scene_quit(GixScene* self) {
    gix_log("Quit uniform scene");
    // Deinit scene here
    SDL_ReleaseGPUBuffer(gix_app_get_gpu_device(self->app), vertex_buffer);
    self->numb_compute_pipeline = 0;
}
GixScene* create_scene(GixApp* app) {
    GixScene* scene = gix_scene_new(app);
    gix_scene_impl(scene, scene_init, scene_event, scene_update, scene_draw, vertex_buffer_scene_quit);

    return scene;
}