cbuffer MvpUniform : register(b0, space1) {
    float4x4 mvp : packoffset(c0);
};

struct ObjectData {
    float2 position;
    float2 size;
    float4 color;
};

struct Output {
    float4 position : SV_POSITION;
    float4 color : TEXCOORD0;
};

StructuredBuffer<ObjectData> object_data : register(t0, space0);

static const uint vertex_index_rect[6] = {0, 1, 2, 3, 2, 1};
static const float2 size_control[4] = {
    {0.0f, 0.0f},
    {1.0f, 0.0f},
    {0.0f, 1.0f},
    {1.0f, 1.0f}};

static const float pi = 3.14159265358979323846f;

Output main(uint id : SV_VertexID) {
    Output output;
    uint data_index = id / 6;
    ObjectData data = object_data[data_index];

    uint vertex_index = id % 6;

    // rotate
    float c = cos(45.f * pi / 180.f);
    float s = sin(45.f * pi / 180.f);
    // Create a rotation matrix of axis Z
    float3x3 rotation_matrix_around_z = {c, -s, 0.f,
                                         s, c, 0.f,
                                         0.f, 0.f, 1.f};
    // create a rotation matrix of axis Y
    float3x3 rotation_matrix_around_y = {c, 0.f, s,
                                         0.f, 1.f, 0.f,
                                         -s, 0.f, c};
    // create a rotation matrix of axis X
    float3x3 rotation_matrix_around_x = {1.f, 0.f, 0.f,
                                         0.f, c, -s,
                                         0.f, s, c};

    // Calculate the vertex position
    uint vert_rect_index = vertex_index_rect[vertex_index];
    float2 pos = data.position + (data.size * size_control[vert_rect_index]);
    float3 pos3 = {pos, 0.0f};
    if (data_index % 100 == 0) {
        pos3 = mul(rotation_matrix_around_x, pos3);
    }
    if (data_index % 150 == 0) {
        pos3 = mul(rotation_matrix_around_y, pos3);
    }
    if (data_index % 50 == 0) {
        pos3 = mul(rotation_matrix_around_z, pos3);
    }
    output.position = mul(mvp, float4(pos3, 1.0f));

    // Set the color
    output.color = data.color;

    return output;
}