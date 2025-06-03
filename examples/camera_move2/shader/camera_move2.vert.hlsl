struct InputVertex {
    float3 vertex : TEXCOORD0;
    uint face_index : TEXCOORD1;
};

cbuffer VPObjects : register(b0, space1) {
    float4x4 view_projection;
}

StructuredBuffer<float4> color_faces : register(t0, space0);
StructuredBuffer<float4x4> model_matrix : register(t1, space0);
struct Output {
    float4 position : SV_POSITION;
    float4 color : TEXCOORD0;
};

Output main(InputVertex input, uint id : SV_InstanceID) {
    Output output;
    output.color = color_faces[input.face_index];
    float4x4 mvp = mul(view_projection, model_matrix[id]);
    // float4 world_pos = mul(model_matrix[id], float4(input.vertex, 1.0));
    output.position = mul(mvp, float4(input.vertex, 1.0));
    return output;
}