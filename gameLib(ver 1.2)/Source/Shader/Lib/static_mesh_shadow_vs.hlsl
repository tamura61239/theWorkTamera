#include"static_mesh.hlsli"
/*****************************************************/
//�@�@�@�@	���_���W���X�N���[����Ԍn�Ɍv�Z����
/*****************************************************/

VS_OUT main(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD)
{
	//float4 pos = mul(position,world);
	float4 worldPosition = mul(position,world);
	float4 viewPosition = mul(worldPosition, view);
    VS_OUT vout = (VS_OUT) 0;
    vout.position = mul(viewPosition, projection);
    vout.texcoord = texcoord;
    vout.color = materialColor;
    return vout;
}