struct InputVertex {
    float3 vertex : TEXCOORD0;
    uint face_index : TEXCOORD1;
    uint cube_id : TEXCOORD2;
};

cbuffer MvpObjects : register(b0, space1) {
    float4x4 mvp[2];  // 0->for static cube, 1->for rotated cube
};

struct CubeColorFace {
    float3 color;
};

StructuredBuffer<CubeColorFace> colors : register(t0, space0);

struct Output {
    float4 position : SV_POSITION;
    float3 color : TEXCOORD0;
};

Output main(InputVertex input) {
    Output output;
    output.color = colors[input.face_index].color;
    output.position = mul(mvp[input.cube_id], float4(input.vertex, 1.f));
    return output;
}