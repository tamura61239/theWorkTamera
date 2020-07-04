#include"gpu_particle_test_render.hlsli"
VS_OUT main(float4 position : POSITION, float4 color : COLOR, float3 velocity : VELOCITY,float count:COUNT)
{
	VS_OUT vout;
	vout.position = position;
	vout.position = mul(vout.position, view);
	vout.position = mul(vout.position, projection);
	vout.color = color;
	return vout;
}