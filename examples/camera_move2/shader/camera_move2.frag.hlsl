struct InputFrag {
    float3 color : TEXCOORD0;
};

struct OutPixel {
    float3 color : SV_TARGET0;
};

OutPixel main(InputFrag input) {
    OutPixel output;
    output.color = input.color;
    return output;
}