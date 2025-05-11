cbuffer MvpUniform : register(b0, space1) {
    float4x4 mvp;
};

struct Objects {
    float3 vertex : TEXCOORD0;
    float3 color : TEXCOORD1;
    float2 position : TEXCOORD2;
};

struct Output {
    float4 position : SV_POSITION;
    float3 color : TEXCOORD0;
};

Output main(Objects input) {
    Output output;
    output.position = mul(mvp, float4(input.vertex, 1.0));
    output.color = input.color;
    return output;
}