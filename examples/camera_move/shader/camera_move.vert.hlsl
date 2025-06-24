#include "../../../src/shader/shader_include/gix_engine_transform.hlsl"

// uniform viewproj
cbuffer MPV : register(b0, space1)
{
    float4x4 view_proj : packoffset(c0);
};

// array of color face
StructuredBuffer<float4> color_faces : register(t0, space0);

// array of rotate
StructuredBuffer<float> rotate : register(t1, space0);

struct VertexInput
{
    float3 position : TEXCOORD0;
    uint face_id : TEXCOORD1;
};

struct Output
{
    float4 color : TEXCOORD0;
    float4 position : SV_POSITION;
};

Output main(VertexInput input, uint id : SV_InstanceID)
{
    Output output;
    output.color = color_faces[input.face_id];

    float4 world_space = GixEngineTransform::rotate_axis_y(float4(input.position, 1.0f), rotate[id]);

    // clip pos
    output.position = mul(view_proj, world_space);

    return output;
}