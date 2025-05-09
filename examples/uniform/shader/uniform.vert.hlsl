cbuffer UBO : register(b0, space1) {
    float4x4 mvp : packoffset(c0);
};
struct Input {
    float3 position : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct Output {
    float4 color : TEXCOORD0;
    float4 position : SV_Position;
};
Output main(Input input) {
    Output output;
    output.color = input.color;
    float4 position = float4(input.position, 1.0f);
    output.position = mul(mvp, position);
    return output;
}