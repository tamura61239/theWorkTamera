#include"particle_render.hlsli"
#include"color_conversion.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = pin.color;
    if (color .a<= 0)
    {
        discard;
    }
    return color;
}