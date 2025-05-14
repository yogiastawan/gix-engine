cbuffer MPV : register(b0, space1) {
    float4x4 mpv : packoffset(c0);
};

cbuffer ColorFace : register(b1, space1) {
    float4 color[6];
};

struct VertexInput {
    float3 position : TEXCOORD0;
};

struct Output {
    float4 color : TEXCOORD0;
    float4 position : SV_POSITION;
};

Output main(VertexInput input, uint id : SV_VERTEXID) {
    Output output;

    // set color
    // get face index
    uint index = id / 4;
    output.color = color[index];

    // set position
    output.position = mul(mpv, float4(input.position, 1.0f));
    return output;
}