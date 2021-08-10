#include"particle_render.hlsli"
#include"color_conversion.hlsli"

float4 main(GS_OUT pin) : SV_TARGET
{
    float4 color = pin.color;
    if (color.a <= 0.f)
    {
        discard;
    }

    return color;
}