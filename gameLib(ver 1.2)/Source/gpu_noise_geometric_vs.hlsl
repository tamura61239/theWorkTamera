#include"gpu_noise_geometric.hlsli"
VS_OUT main( float4 position : POSITION, float4 color : COLOR, float3 angle : ANGLE,
	float3 velocity : VELOCITY, float life : LIFE, float maxLife : MAXLIFE)
{
	VS_OUT vout;
	vout.position = position;
	vout.color = color;
	vout.angle = angle;
	return vout;
	//GS_OUT gout;
	//gout.position = mul(position, view);
	//gout.position = mul(gout.position, projection);
	//gout.color = color;
	//return gout;
}