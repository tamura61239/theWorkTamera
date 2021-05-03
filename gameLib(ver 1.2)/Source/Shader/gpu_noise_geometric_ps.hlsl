#include"gpu_noise_geometric.hlsli"

float4 main(GS_OUT pin) : SV_TARGET
{
	return pin.color;
}