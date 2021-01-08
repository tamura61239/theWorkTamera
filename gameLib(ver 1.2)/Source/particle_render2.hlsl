#include"particle_render.hlsli"

VS_OUT main(float4 position : POSITION, float angle : ANGLE, uint color : COLOR, float3 velocity : VELOCITY, float scale : SCALE)
{
    VS_OUT vout = (VS_OUT) 0;
    vout.worldPosition = position;
    vout.position = mul(float4(position.xyz, 1), view);
    vout.position = mul(vout.position, projection);
    vout.scale = scale;
    vout.color.r = ((color >> 24) & 0x000000FF) / 255.0f;
    vout.color.g = ((color >> 16) & 0x000000FF) / 255.0f;
    vout.color.b = ((color >> 8) & 0x000000FF) / 255.0f;
    vout.color.a = ((color >> 0) & 0x000000FF) / 255.0f;
    return vout;
}