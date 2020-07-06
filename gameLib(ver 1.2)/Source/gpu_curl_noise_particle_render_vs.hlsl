#include"gpu_particle_test_render.hlsli"
VS_OUT main(float4 position : POSITION, float3 velocity : VELOCITY, float4 color : COLOR,float life:LIFE)
{
	VS_OUT vout;
	vout.position = position;
	vout.position = mul(vout.position, view);
	vout.position = mul(vout.position, projection);
	vout.color = color;
	return vout;
}