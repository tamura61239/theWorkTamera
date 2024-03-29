#include "sprite.hlsli"   
Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);
/*****************************************************/
//　　　　	UV値に合わせた色を取得する
/*****************************************************/

float4 main(VS_OUT pin) : SV_TARGET
{
	return diffuse_map.Sample(diffuse_map_sampler_state, pin.texcoord) * pin.color;
}