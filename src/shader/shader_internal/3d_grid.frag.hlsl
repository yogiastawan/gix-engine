struct FragInput {
    float4 color : TEXCOORD0;
};

struct FragOutput {
    float4 color : SV_TARGET0;
};

FragOutput main(FragInput input) {
    FragOutput output;
    output.color = input.color;
    return output;
}