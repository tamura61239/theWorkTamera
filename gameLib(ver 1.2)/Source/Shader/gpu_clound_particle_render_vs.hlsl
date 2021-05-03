#include"gpu_cloud_particle_render.hlsli"
VS_OUT main(float4 position : POSITION, float3 velocity : VELOCITY, float4 color : COLOR)
{
	VS_OUT vout;
	vout.position = mul(position, view);
	vout.color = color;
	return vout;
}