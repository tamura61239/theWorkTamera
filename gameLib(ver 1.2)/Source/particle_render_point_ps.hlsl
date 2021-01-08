#include"particle_render.hlsli"

float4 main(VS_OUT pin) : SV_TARGET
{
	if(pin.position.w<=0)
    {
        discard;
    }
	return pin.color;
}