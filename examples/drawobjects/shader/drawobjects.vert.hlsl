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

Output main(uint id : SV_VertexID) {
    Output output;
    uint data_index = id / 6;
    ObjectData data = object_data[data_index];

    uint vertex_index = id % 6;

    // Calculate the vertex position
    uint vert_rect_index = vertex_index_rect[vertex_index];
    float2 pos = data.position + (data.size * size_control[vert_rect_index]);
    output.position = mul(mvp, float4(pos, 0.0f, 1.0f));

    // Set the color
    output.color = data.color;

    return output;
}