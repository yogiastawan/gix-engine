struct InputFrag {
    float4 color : TEXCOORD0;
};

struct OutPixel {
    float4 color : SV_TARGET0;
};

OutPixel main(InputFrag input) {
    OutPixel output;
    output.color = input.color;
    return output;
}