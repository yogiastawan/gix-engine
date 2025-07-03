#include "scene.h"

#include <cglm/cglm.h>

#include "cm_scene.h"
#include "gix_engine/gix_app_engine.h"

static SDL_AppResult init(GixScene *scene);
static SDL_AppResult event(GixScene *scene, const SDL_Event *event);
static SDL_AppResult update(GixScene *scene, Uint64 dt);
static SDL_AppResult draw(GixScene *scene);
static void quit(GixScene *scene);

GixScene *create_scene(GixApp *app) {
    GixScene *scene = gix_scene_new(app);

    gix_scene_impl(scene, init, event, update, draw, quit);
    return scene;
}

SDL_AppResult init(GixScene *scene) {
    SDL_GPUDevice *device = gix_app_get_gpu_device(scene->app);
    SDL_Window *window = gix_app_get_window(scene->app);

    CMScene *cm = malloc(sizeof(CMScene));
    Uint32 w, h;
    Uint32 fmt = gix_app_get_depth_texture_format(scene->app);
    gix_app_get_window_size(scene->app, &w, &h);
    cm_scene_init(cm, device, fmt, w, h);

    // setup pipeline
    gix_scene_alloc_graphic_pipeline(scene, 1);
    // load shader
    SDL_GPUShader *vertex_shader = gix_load_shader(
        device, "./shader/SPIRV/camera_move.vert.spv",
        "./shader/SPIRV/camera_move.vert.json", SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader *frag_shader = gix_load_shader(
        device, "./shader/SPIRV/camera_move.frag.spv",
        "./shader/SPIRV/camera_move.frag.json", SDL_GPU_SHADERSTAGE_FRAGMENT);

    // create color buffer descr
    const Uint8 num_color_targets = 1;
    SDL_GPUColorTargetDescription color_target_desc[] = {
        (SDL_GPUColorTargetDescription){
            .format = SDL_GetGPUSwapchainTextureFormat(device, window),
        },
    };

    // create vertex buffer descr
    const Uint8 num_vertex_buffers = 1;
    SDL_GPUVertexBufferDescription vertex_buffer_descriptions[] = {
        // CubeVertex
        (SDL_GPUVertexBufferDescription){
            .slot = 0,
            .pitch = sizeof(CubeVertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
        },
    };
    // create vertex buffer attr
    const Uint8 num_vertex_attributes = 2;
    SDL_GPUVertexAttribute vertex_attributes[] = {
        // position
        (SDL_GPUVertexAttribute){
            .buffer_slot = 0,
            .location = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = offsetof(CubeVertex, vertex),
        },
        // face id
        (SDL_GPUVertexAttribute){
            .buffer_slot = 0,
            .location = 1,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UINT,
            .offset = offsetof(CubeVertex, face_id),
        },
    };
    // create vertex input state
    SDL_GPUVertexInputState vertex_input_state = {
        .num_vertex_buffers = num_vertex_buffers,
        .vertex_buffer_descriptions = vertex_buffer_descriptions,
        .num_vertex_attributes = num_vertex_attributes,
        .vertex_attributes = vertex_attributes,
    };
    // enable depth test stencil state
    SDL_GPUDepthStencilState depth_stencil_state = {
        .enable_depth_test = true,
        .enable_depth_write = true,
        .compare_op = SDL_GPU_COMPAREOP_LESS,
    };
    // create graphic piplene
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
        .target_info =
            {
                .num_color_targets = num_color_targets,
                .color_target_descriptions = color_target_desc,
                .has_depth_stencil_target = true,
                .depth_stencil_format = fmt,
            },
        .vertex_input_state = vertex_input_state,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = vertex_shader,
        .fragment_shader = frag_shader,
        .depth_stencil_state = depth_stencil_state,
    };
    scene->graphic_pipeline[0] =
        SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);
    // release shader
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, frag_shader);

    // create transfer buffer
    SDL_GPUTransferBufferCreateInfo tf_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = (sizeof(CubeVertex) * cm->numb_cube_vertex) +  // vertex
                (sizeof(Uint16) * cm->numb_indice_vertex) +    // indices
                (sizeof(vec4) * cm->numb_face_color) +         // face color
                sizeof(vec4) * cm->numb_cube,                  // position_cube
    };
    SDL_GPUTransferBuffer *transfer_buffer =
        SDL_CreateGPUTransferBuffer(device, &tf_info);
    // map transfer buffer
    CubeVertex *transfer_address =
        SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    // copy data
    SDL_memcpy(transfer_address, cm->cube_vertex,
               sizeof(CubeVertex) * cm->numb_cube_vertex);
    SDL_memcpy(
        (Uint8 *)transfer_address + (sizeof(CubeVertex) * cm->numb_cube_vertex),
        cm->indice_vertex, sizeof(Uint16) * cm->numb_indice_vertex);
    SDL_memcpy((Uint8 *)transfer_address +
                   (sizeof(CubeVertex) * cm->numb_cube_vertex) +
                   (sizeof(Uint16) * cm->numb_indice_vertex),
               cm->face_color, sizeof(vec4) * cm->numb_face_color);
    SDL_memcpy((Uint8 *)transfer_address +
                   (sizeof(CubeVertex) * cm->numb_cube_vertex) +
                   (sizeof(Uint16) * cm->numb_indice_vertex) +
                   sizeof(vec4) * cm->numb_face_color,
               cm->position_cube, sizeof(vec4) * cm->numb_cube);
    // unmap transfer buffer
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    // create cmd buffer for upload
    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
    // begin copy pass
    SDL_GPUCopyPass *cp = SDL_BeginGPUCopyPass(cmd);
    // upload vertex
    SDL_GPUTransferBufferLocation src = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
    };
    SDL_GPUBufferRegion dst = {
        .buffer = cm->vertex_buffer,
        .offset = 0,
        .size = sizeof(CubeVertex) * cm->numb_cube_vertex,
    };
    SDL_UploadToGPUBuffer(cp, &src, &dst, false);
    // upload indices
    src.offset = sizeof(CubeVertex) * cm->numb_cube_vertex;
    dst.offset = 0;
    dst.buffer = cm->index_buffer;
    dst.size = sizeof(Uint16) * cm->numb_indice_vertex;
    SDL_UploadToGPUBuffer(cp, &src, &dst, false);
    // upload face color
    src.offset = (sizeof(CubeVertex) * cm->numb_cube_vertex) +
                 (sizeof(Uint16) * cm->numb_indice_vertex);
    dst.offset = 0;
    dst.buffer = cm->color_buffer;
    dst.size = sizeof(vec4) * cm->numb_face_color;
    SDL_UploadToGPUBuffer(cp, &src, &dst, false);
    // upload position_cube
    src.offset = (sizeof(CubeVertex) * cm->numb_cube_vertex) +
                 (sizeof(Uint16) * cm->numb_indice_vertex) +
                 sizeof(vec4) * cm->numb_face_color;
    dst.offset = 0;
    dst.buffer = cm->position_cube_buffer;
    dst.size = sizeof(vec4) * cm->numb_cube;
    SDL_UploadToGPUBuffer(cp, &src, &dst, false);

    // end copy pass
    SDL_EndGPUCopyPass(cp);
    // submit cmd
    bool res = SDL_SubmitGPUCommandBuffer(cmd);
    gix_if_exit(!res, gix_log("Couldn't submit cmd"));
    // release transfer buffer
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);
    // free vertex, indices, face color after upload to GPU
    SDL_free(cm->cube_vertex);
    SDL_free(cm->indice_vertex);
    SDL_free(cm->face_color);
    SDL_free(cm->position_cube);
    return SDL_APP_CONTINUE;
}
SDL_AppResult event(GixScene *scene, const SDL_Event *event) {
    return SDL_APP_CONTINUE;
}
SDL_AppResult update(GixScene *scene, Uint64 dt) { return SDL_APP_CONTINUE; }

SDL_AppResult draw(GixScene *scene) {
    SDL_Window *window = gix_app_get_window(scene->app);
    SDL_GPUDevice *device = gix_app_get_gpu_device(scene->app);
    CMScene *cm = scene->user_data;

    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);

    SDL_GPUTexture *swapchain_texture;
    if (SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &swapchain_texture,
                                              NULL, NULL)) {
        // map rotate angel transfer buffer
        float *rotate_transfer_address =
            SDL_MapGPUTransferBuffer(device, cm->rotate_transfer_buffer, false);
        SDL_memcpy(rotate_transfer_address, cm->rotate_angle,
                   sizeof(float) * cm->numb_cube);
        SDL_UnmapGPUTransferBuffer(device, cm->rotate_transfer_buffer);
        // upload to GPU
    }

    return SDL_APP_CONTINUE;
}

void quit(GixScene *scene) {}
