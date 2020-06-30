#include"gpu_cloud_particle_render.hlsli"

Texture2D diffuseMap : register(t0);
SamplerState diffuseMapSamplerState : register(s0);

float4 main(GS_OUT pin) : SV_TARGET
{
	return diffuseMap.Sample(diffuseMapSamplerState,pin.texcoord)*pin.color;
}