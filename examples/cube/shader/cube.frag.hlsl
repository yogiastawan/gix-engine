struct FragInput {
    float4 color : TEXCOORD0;
};

struct FragOut {
    float4 color : SV_TARGET0;
};

FragOut main(FragInput input){
    FragOut output;
    output.color=input.color;
    return output;
}
