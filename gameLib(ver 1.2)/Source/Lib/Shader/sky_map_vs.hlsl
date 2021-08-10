#include"sky_map.hlsli"
/*****************************************************/
//�@�@�@�@	���_���W���X�N���[����Ԍn�Ɍv�Z����
/*****************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL)
{
	VS_OUT vout;
	vout.position = mul(position, mul(world, mul(view, projection)));
	vout.texcoord = normalize(position.xyz);
	return vout;
}