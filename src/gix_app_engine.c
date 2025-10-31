// #ifdef GIX_APP_MAIN
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
// #endif
#include <gix_engine/core/gix_app_engine.h>
#include <gix_engine/core/gix_checker.h>
#include <gix_engine/core/gix_log.h>
#include <gix_engine/core/gix_shader.h>

#undef __internal_gix_scene_setup_3d_grid
#undef __internal_gix_scene_draw_3d_grid
#undef __internal_gix_scene_set_3d_grid_color

struct _GixApp {
    SDL_Window* window;
    SDL_GPUDevice* device;
    GixScene* loading_scene;
    GixScene* current_scene;

    u64 delta_time;
    u64 last_tick;

    u32 window_width;
    u32 window_height;

    bool is_onload_scene;
};

#define GIX_ENGINE_NUM_GRID_3D_VERTEX 2  // 2 vertex to draw start and end
#define GIX_ENGINE_NUM_GRID_3D_DATA 2    // for x and z

#ifdef BUILD_DEBUG

struct _GixSceneDebugPrivate {
    SDL_GPUBuffer* vertex_grid_3d_buffer;
    SDL_GPUBuffer* line_grid_3d_buffer;
    SDL_GPUGraphicsPipeline* grid_3d_pipeLine;
    u32 grid_3d_numb_line;
    u8 grid_3d_color[4];
    bool is_grid_3d_inited;
};
typedef struct _VertexGrid3DLine {
    u8 color[4];
} VertexGrid3DLine;

typedef struct _Line3DData {
    float start_end[2];
} Line3DData;

typedef struct Grid3DUniform {
    mat4 vp;
    u32 numb_instance;
} Grid3DUniform;

#endif

static GixApp* gix_app_new(const char* name);
static void gix_app_destroy(GixApp* app);

static char* concate_str(const char* dir, const char* file_name) {
    size_t dir_len = SDL_strlen(dir);
    size_t name_len = SDL_strlen(file_name);
    char* out = SDL_malloc(sizeof(char) * (dir_len + name_len + 1));
    size_t max_len = dir_len + name_len + 1;
    SDL_strlcpy(out, dir, max_len);
    SDL_strlcat(out, file_name, max_len);
    return out;
}

static void gix_app_sdl_init() {
    bool init_result = SDL_Init(SDL_INIT_VIDEO);
    gix_if_exit(!init_result, gix_log_error("Couldn't init SDL"));

#ifdef BUILD_DEBUG
    SDL_SetLogPriorities(SDL_LOG_PRIORITY_DEBUG);
#endif
}
// #ifdef GIX_APP_MAIN

// SDL CYCLE
SDL_AppResult SDL_AppInit(void** app_state, int argc, char* argv[]) {
    if (argc > 1) {
        gix_log("%s", argv[1]);
    }

    GixApp* app = gix_app_new("GixApp");
    app_state[0] = app;
    gix_app_init(app);
    SDL_ShowWindow(app->window);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* app_state, SDL_Event* event) {
    GixApp* app = (GixApp*)app_state;
    gix_if_null_exit(app, gix_log_debug("GixApp should not NULL"));

    switch (event->type) {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS;
            break;
        }

        default: {
            if (app->is_onload_scene && app->loading_scene) {
                return gix_scene_event(app->loading_scene, event);
            }
            return gix_scene_event(app->current_scene, event);
            break;
        }
    }
}

SDL_AppResult SDL_AppIterate(void* app_state) {
    GixApp* app = (GixApp*)app_state;
    gix_if_null_exit(app, gix_log_debug("GixApp should not NULL"));

    u64 current_tick = SDL_GetTicks();
    app->delta_time = (current_tick - app->last_tick);
    app->last_tick = current_tick;

    if (app->loading_scene) {
        SDL_AppResult update_result =
            gix_scene_update(app->loading_scene, app->delta_time);
        SDL_AppResult draw_result = gix_scene_draw(app->loading_scene);
        // return SDL_APP_CONTINUE=0 when continue
        // return SDL_APP_SUCCESS=1 when success and quit
        // return SDL_APP_FAILURE=2 when failed
        return update_result | draw_result;
    }
    SDL_AppResult update_result =
        gix_scene_update(app->current_scene, app->delta_time);
    SDL_AppResult draw_result = gix_scene_draw(app->current_scene);
    // return SDL_APP_CONTINUE=0 when continue
    // return SDL_APP_SUCCESS=1 when success
    // return SDL_APP_FAILURE=2 when failed
    return update_result | draw_result;
}
void SDL_AppQuit(void* app_state, SDL_AppResult result) {
    if (result == SDL_APP_FAILURE) {
        gix_log("App quit with error");
    }

    GixApp* app = (GixApp*)app_state;
    gix_if_null_exit(app, gix_log_debug("GixApp should not NULL"));

    gix_app_destroy(app);
}

// END SDL CYCLE

// #endif

// private function
GixScene* gix_scene_new(GixApp* app) {
    gix_log("Create new GixScene");

    GixScene* scene = SDL_malloc(sizeof(GixScene));
    scene->app = app;
    scene->graphic_pipeline = NULL;
    scene->numb_graphic_pipeline = 0;
    scene->compute_pipeline = NULL;
    scene->numb_compute_pipeline = 0;
    scene->user_data = NULL;

#ifdef BUILD_DEBUG
    scene->priv = SDL_malloc(sizeof(GixSceneDebugPrivate));
    scene->priv->vertex_grid_3d_buffer = NULL;
    scene->priv->line_grid_3d_buffer = NULL;
    scene->priv->grid_3d_numb_line = GIX_ENGINE_NUMB_GRID_3D_LINE_DEFAULT;
    SDL_memcpy(scene->priv->grid_3d_color,
               GIX_ENGINE_COLOR_GRID_3D_LINE_DEFAULT, sizeof(u8) * 4);
    scene->priv->is_grid_3d_inited = false;
#endif

    return scene;
}

// TODO! implement load scene from file protobuf
GixScene* gix_scene_from_file(GixApp* app, const char* file_path) {
    gix_log("Create new GixScene");

    GixScene* scene = SDL_malloc(sizeof(GixScene));
    scene->app = app;
    return scene;
}

void gix_scene_impl(GixScene* scene, SceneInit init_func, SceneEvent event_func,
                    SceneUpdate update_func, SceneDraw draw_func,
                    SceneQuit quit_func) {
    gix_if_null_exit(scene, gix_log_debug("GixScene must not be NULL"));
    scene->scene_init = init_func;
    scene->scene_event = event_func;
    scene->scene_update = update_func;
    scene->scene_draw = draw_func;
    scene->scene_quit = quit_func;
}

#ifdef BUILD_DEBUG
void __internal_gix_scene_setup_3d_grid(GixScene* scene, u32 length_side) {
    if (scene->priv->is_grid_3d_inited) {
        return;
    }
    u32 length =
        length_side +
        (length_side % 2);  // if length_side is odd then add 1 meter more

    scene->priv->grid_3d_numb_line = length + 1;

    SDL_GPUDevice* device = gix_app_get_gpu_device(scene->app);
    SDL_Window* window = gix_app_get_window(scene->app);

    // create vertex bufer
    SDL_GPUBufferCreateInfo vertex_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(VertexGrid3DLine) * GIX_ENGINE_NUM_GRID_3D_VERTEX,
    };
    scene->priv->vertex_grid_3d_buffer =
        SDL_CreateGPUBuffer(device, &vertex_buffer_info);
    // create storage buffer
    SDL_GPUBufferCreateInfo line_data_buffer_info = {
        .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
        .size = sizeof(Line3DData) * GIX_ENGINE_NUM_GRID_3D_DATA,
    };
    scene->priv->line_grid_3d_buffer =
        SDL_CreateGPUBuffer(device, &line_data_buffer_info);

    SDL_GPUTextureFormat depth_format =
        gix_app_get_depth_texture_format(scene->app);

    // setup pipeline
    scene->priv->grid_3d_pipeLine =
        SDL_malloc(sizeof(SDL_GPUGraphicsPipeline*));
    // load shader
#ifdef GIX_VULKAN
    const char* vert_file_name = "/SPIRV/3d_grid.vert.spv";
    const char* vert_file_info = "/SPIRV/3d_grid.vert.json";
    const char* frag_file_name = "/SPIRV/3d_grid.frag.spv";
    const char* frag_file_info = "/SPIRV/3d_grid.frag.json";
#elif GIX_MSL
    const char* vert_file_name = "/MSL/3d_grid.vert.msl";
    const char* vert_file_info = "/MSL/3d_grid.vert.json";
    const char* frag_file_name = "/MSL/3d_grid.frag.msl";
    const char* frag_file_info = "/MSL/3d_grid.frag.json";
#elif GX_DXIL
    const char* vert_file_name = "/DXIL/3d_grid.vert.dxil";
    const char* vert_file_info = "/DXIL/3d_grid.vert.json";
    const char* frag_file_name = "/DXIL/3d_grid.frag.dxil";
    const char* frag_file_info = "/DXIL/3d_grid.frag.json";
#else
    const char* vert_file_name = NULL;
    const char* vert_file_info = NULL;
    const char* frag_file_name = NULL;
    const char* frag_file_info = NULL;
#endif
    char* ver_file = concate_str(GIX_ENGINE_SHADER_DIR, vert_file_name);
    char* vert_json = concate_str(GIX_ENGINE_SHADER_DIR, vert_file_info);
    SDL_GPUShader* vertex_shader = gix_load_shader(device, ver_file, vert_json,
                                                   SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_free(ver_file);
    SDL_free(vert_json);

    char* farg_file = concate_str(GIX_ENGINE_SHADER_DIR, frag_file_name);
    char* frag_json = concate_str(GIX_ENGINE_SHADER_DIR, frag_file_info);
    SDL_GPUShader* frag_shader = gix_load_shader(device, farg_file, frag_json,
                                                 SDL_GPU_SHADERSTAGE_FRAGMENT);

    SDL_free(farg_file);
    SDL_free(frag_json);

    // create color target description
    SDL_GPUColorTargetDescription color_target_desc[1] = {
        (SDL_GPUColorTargetDescription){
            .format = SDL_GetGPUSwapchainTextureFormat(device, window),
        },
    };

    // create vertex buffer desc
    SDL_GPUVertexBufferDescription vert_buffer_desc[1] = {
        {
            .slot = 0,
            .pitch = sizeof(VertexGrid3DLine),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0,
        },
    };
    // vertex attr desc
    SDL_GPUVertexAttribute vert_attr[1] = {
        {
            .buffer_slot = 0,
            .location = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
            .offset = offsetof(VertexGrid3DLine, color),
        },
    };
    // vert input state
    SDL_GPUVertexInputState vert_input_state = {
        .num_vertex_buffers = 1,
        .vertex_buffer_descriptions = vert_buffer_desc,
        .num_vertex_attributes = 1,
        .vertex_attributes = vert_attr,
    };

    // enable depth test
    SDL_GPUDepthStencilState depth_stencil = {
        .enable_depth_test = true,
        .enable_depth_write = true,
        .compare_op = SDL_GPU_COMPAREOP_LESS,
    };

    // create grphic pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
        .target_info =
            {
                .num_color_targets = 1,
                .color_target_descriptions = color_target_desc,
                .has_depth_stencil_target = true,
                .depth_stencil_format = depth_format,
            },
        .vertex_input_state = vert_input_state,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_LINELIST,
        .vertex_shader = vertex_shader,
        .fragment_shader = frag_shader,
        .depth_stencil_state = depth_stencil,
    };
    scene->priv->grid_3d_pipeLine =
        SDL_CreateGPUGraphicsPipeline(device, &pipeline_info);
    SDL_ReleaseGPUShader(device, vertex_shader);
    SDL_ReleaseGPUShader(device, frag_shader);

    // create transfer buffer
    SDL_GPUTransferBufferCreateInfo transfer_buffer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = (sizeof(VertexGrid3DLine) * GIX_ENGINE_NUM_GRID_3D_VERTEX) +
                (sizeof(Line3DData) * GIX_ENGINE_NUM_GRID_3D_DATA),
    };

    SDL_GPUTransferBuffer* transfer_buffer =
        SDL_CreateGPUTransferBuffer(device, &transfer_buffer_info);
    // map transfer buffer
    void* transfer_address =
        SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    VertexGrid3DLine data[GIX_ENGINE_NUM_GRID_3D_VERTEX] = {
        {
            .color = {scene->priv->grid_3d_color[0],
                      scene->priv->grid_3d_color[1],
                      scene->priv->grid_3d_color[2],
                      scene->priv->grid_3d_color[3]},
        },
        {
            .color = {scene->priv->grid_3d_color[0],
                      scene->priv->grid_3d_color[1],
                      scene->priv->grid_3d_color[2],
                      scene->priv->grid_3d_color[3]},
        },
    };
    // copy vertex
    SDL_memcpy(transfer_address, &data,
               sizeof(VertexGrid3DLine) * GIX_ENGINE_NUM_GRID_3D_VERTEX);
    // copy storage buffer
    float start = (0.f - (float)length / 2.f);
    float end = ((float)length / 2.f);
    Line3DData line_data[GIX_ENGINE_NUM_GRID_3D_DATA] = {
        {.start_end = {start, end}},
        {.start_end = {end, start}},
    };
    SDL_memcpy((u8*)transfer_address +
                   (sizeof(VertexGrid3DLine) * GIX_ENGINE_NUM_GRID_3D_VERTEX),
               line_data, sizeof(Line3DData) * GIX_ENGINE_NUM_GRID_3D_DATA);
    // unmap
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    // upload buffer
    //  Upload buffer data
    SDL_GPUCommandBuffer* upload_cmd = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(upload_cmd);
    // upload vertex
    SDL_GPUTransferBufferLocation src = {
        .transfer_buffer = transfer_buffer,
        .offset = 0,
    };
    SDL_GPUBufferRegion dst = {
        .buffer = scene->priv->vertex_grid_3d_buffer,
        .offset = 0,
        .size = sizeof(VertexGrid3DLine) * GIX_ENGINE_NUM_GRID_3D_VERTEX,
    };
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
    // upload line data
    src.offset = sizeof(VertexGrid3DLine) * GIX_ENGINE_NUM_GRID_3D_VERTEX;
    dst.buffer = scene->priv->line_grid_3d_buffer;
    dst.offset = 0;
    dst.size = sizeof(Line3DData) * GIX_ENGINE_NUM_GRID_3D_DATA;
    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
    SDL_EndGPUCopyPass(copy_pass);

    bool res = SDL_SubmitGPUCommandBuffer(upload_cmd);
    gix_if_exit(!res, gix_log_error("Couldn't submit upload cmd in grid 3d"));
    // release transfer buffer
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    scene->priv->is_grid_3d_inited = true;
}

void __internal_gix_scene_draw_3d_grid(GixScene* scene,
                                       SDL_GPUCommandBuffer* cmd,
                                       SDL_GPURenderPass* render_pass,
                                       mat4 vp) {
    SDL_BindGPUGraphicsPipeline(render_pass, scene->priv->grid_3d_pipeLine);
    // bind vertex buffer
    SDL_GPUBufferBinding vertex_buffer_binding[1] = {
        {
            .buffer = scene->priv->vertex_grid_3d_buffer,
            .offset = 0,
        },
    };
    SDL_BindGPUVertexBuffers(render_pass, 0, vertex_buffer_binding, 1);

    // bind storage buffer
    SDL_GPUBuffer* vertex_storage_buffer[1] = {
        scene->priv->line_grid_3d_buffer,
    };
    SDL_BindGPUVertexStorageBuffers(render_pass, 0, vertex_storage_buffer, 1);

    // push uniform
    Grid3DUniform uniform = {0};
    glm_mat4_copy(vp, uniform.vp);
    uniform.numb_instance = scene->priv->grid_3d_numb_line * 2;
    SDL_PushGPUVertexUniformData(cmd, 0, &uniform, sizeof(Grid3DUniform));

    // draw
    SDL_DrawGPUPrimitives(render_pass, GIX_ENGINE_NUM_GRID_3D_VERTEX,
                          uniform.numb_instance, 0, 0);
}

void __internal_gix_scene_set_3d_grid_color(GixScene* scene,
                                            const u8 color[4]) {
    SDL_memcpy(scene->priv->grid_3d_color, color, sizeof(u8) * 4);
}
#endif

void gix_scene_destroy(GixScene* scene) {
    gix_log("Destroy GixScene");
    gix_if_null_exit(scene, gix_log_debug("Can not destroy NULl of scene"));
    SDL_GPUDevice* device = gix_app_get_gpu_device(scene->app);
    // free user data
    SDL_free(scene->user_data);
    // free graphic pipelines
    for (u8 i = 0; i < scene->numb_graphic_pipeline; i++) {
        SDL_ReleaseGPUGraphicsPipeline(scene->app->device,
                                       scene->graphic_pipeline[i]);
    }
    // free compute pipeline
    for (u8 i = 0; i < scene->numb_compute_pipeline; i++) {
        SDL_ReleaseGPUGraphicsPipeline(scene->app->device,
                                       scene->compute_pipeline[i]);
    }
    // free list graphic pipeline
    SDL_free(scene->graphic_pipeline);
    // free list compute pipeline
    SDL_free(scene->compute_pipeline);

    // free debug
#ifdef BUILD_DEBUG
    if (scene->priv->vertex_grid_3d_buffer) {
        SDL_ReleaseGPUBuffer(device, scene->priv->vertex_grid_3d_buffer);
    }

    if (scene->priv->line_grid_3d_buffer) {
        SDL_ReleaseGPUBuffer(device, scene->priv->line_grid_3d_buffer);
    }
    if (scene->priv->grid_3d_pipeLine) {
        SDL_ReleaseGPUGraphicsPipeline(device, scene->priv->grid_3d_pipeLine);
    }

    SDL_free(scene->priv);
#endif

    // free scene
    SDL_free(scene);
}

GixApp* gix_app_new(const char* name) {
    gix_log("Create new GixApp");

    gix_if_null_exit(name, gix_log_debug("Name GixApp shouldn't NULL"));

    gix_app_sdl_init();

    GixApp* app = SDL_malloc(sizeof(GixApp));

    // Initialize window
    app->window = SDL_CreateWindow(name, 800, 600, SDL_WINDOW_HIDDEN);
    gix_if_null_exit(app->window, gix_log_error("Couldn't create window"));

    // Initialize GPUDevice
    app->device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                          SDL_GPU_SHADERFORMAT_DXIL |
                                          SDL_GPU_SHADERFORMAT_MSL,
                                      true, NULL);
    gix_if_null_exit(app->device, gix_log_error("Couldn't create GPU device"));

    if (!SDL_ClaimWindowForGPUDevice(app->device, app->window)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Error on claim window for GPU device. %s",
                     SDL_GetError());
    }

    app->loading_scene = NULL;
    app->current_scene = NULL;
    app->is_onload_scene = false;
    app->delta_time = 0;
    app->last_tick = 0;
    SDL_GetWindowSize(app->window, (int*)&app->window_width,
                      (int*)&app->window_height);

    return app;
}

void gix_app_set_window_fullscreen(GixApp* app) {
    SDL_SetWindowFullscreen(app->window, true);
    SDL_GetWindowSize(app->window, (int*)&app->window_width,
                      (int*)&app->window_height);
}

void gix_app_set_window_borderless(GixApp* app, bool borderless) {
    SDL_SetWindowBordered(app->window, borderless);
}

void gix_app_set_name(GixApp* app, const char* name) {
    SDL_SetWindowTitle(app->window, name);
}

void gix_app_get_window_size(GixApp* app, u32* width, u32* height) {
    *width = app->window_width;
    *height = app->window_height;
}

void gix_app_set_window_size(GixApp* app, i32 width, i32 height) {
    SDL_SetWindowSize(app->window, width, height);
}

void gix_app_set_window_position(GixApp* app, i32 x, i32 y) {
    SDL_SetWindowPosition(app->window, x, y);
}

void gix_app_set_window_resizeable(GixApp* app, bool resizeable) {
    SDL_SetWindowResizable(app->window, resizeable);
    gix_log("Set window resizeable");
}

void gix_app_set_loading_scene(GixApp* app, GixScene* scene) {
    gix_log("Set loading scene to GixApp");

    gix_if_null_exit(app, gix_log_debug("GixApp should not NULL"));
    gix_if_null_exit(scene, gix_log_debug("GixScene should not NULL"));
    app->loading_scene = scene;
}

SDL_AppResult gix_app_set_scene(GixApp* app, GixScene* scene) {
    gix_log("Set scene to GixApp");

    gix_if_null_exit(app, gix_log_debug("GixApp should not NULL"));
    gix_if_null_exit(scene, gix_log_debug("GixScene should not NULL"));

    // TODO! Loading scene here
    // set is_onload_scene to true
    app->is_onload_scene = true;
    // init scene
    SDL_AppResult result = gix_scene_init(scene);
    // set scene to current scene
    app->current_scene = scene;
    // set is_onload_scene to false
    app->is_onload_scene = false;
    return result;
}

SDL_Window* gix_app_get_window(GixApp* app) { return app->window; }

SDL_GPUDevice* gix_app_get_gpu_device(GixApp* app) { return app->device; }

SDL_GPUTextureFormat gix_app_get_depth_texture_format(GixApp* app) {
    SDL_GPUTextureFormat format;
    if (SDL_GPUTextureSupportsFormat(
            app->device, SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            SDL_GPU_TEXTURETYPE_2D,
            SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    } else if (SDL_GPUTextureSupportsFormat(
                   app->device, SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
                   SDL_GPU_TEXTURETYPE_2D,
                   SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
    } else if (SDL_GPUTextureSupportsFormat(
                   app->device, SDL_GPU_TEXTUREFORMAT_D24_UNORM,
                   SDL_GPU_TEXTURETYPE_2D,
                   SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D24_UNORM;
    } else if (SDL_GPUTextureSupportsFormat(
                   app->device, SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
                   SDL_GPU_TEXTURETYPE_2D,
                   SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET)) {
        format = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
    } else {
        format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
    }

    gix_log("Get depth texture format support: %u", format);
    return format;
}

// private function
void gix_app_destroy(GixApp* app) {
    gix_log("Destroy GixApp");

    gix_if_null_exit(app, gix_log_debug("Can not destroy of NULL GixApp"));

    if (app->loading_scene) {
        app->loading_scene->scene_quit(app->loading_scene);
        gix_scene_destroy(app->loading_scene);
    }

    if (app->current_scene) {
        app->current_scene->scene_quit(app->current_scene);
        gix_scene_destroy(app->current_scene);
    }

    SDL_ReleaseWindowFromGPUDevice(app->device, app->window);
    SDL_DestroyGPUDevice(app->device);
    SDL_DestroyWindow(app->window);

    SDL_Quit();

    SDL_free(app);
}
