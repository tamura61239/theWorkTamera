#include "static_mesh.hlsli"
#include"ps_function.hlsli"
#include"lightType.hlsli"

Texture2D diffuse_map : register(t0);
SamplerState diffuse_map_sampler_state : register(s0);

/*****************************************************/
//�@�@�@�@	���C�g�Ȃǂ����f���ꂽ�F���v�Z����
/*****************************************************/

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = diffuse_map.Sample(diffuse_map_sampler_state,pin.texcoord);

	float3 E = normalize(eyePosition.xyz - pin.worldPosition.xyz);

	float3 L = normalize(lightDirection.xyz);
	//����
	float3 A = ambientColor.rgb*HemiSphereLight(pin.worldNormal, skyColor, groundColor);
	//�g�U����
	float3 C = lightColor.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = Diffuse(pin.worldNormal, L, C, Kd);
	//���ʔ���
	float3 Ks = float3(1, 1, 1);
	float3 S = BlinnPhongSpcular(pin.worldNormal, L, C, E, Ks, 20);

	float3 LV;   //���C�g�x�N�g��
	float3 LC;    //���C�g�F
	float influence;  //���C�g�̉e��
	float3 LD = (float3)0;
	float3 LS = (float3)0;
	for (int i = 0;i < POINTMAX;i++)
	{
		//�|�C���g���C�g
		LV = pin.worldPosition.xyz - pointLight[i].position.xyz;
		float d = length(LV);
		float r = pointLight[i].range;
		float s = step(r, d);
		influence = lerp(saturate(1.0f - d / r), 0, s* pointLight[i].type);
		LV = normalize(LV);
		LC = pointLight[i].color.rgb;
		//�g�U���̉��Z
		LD += Diffuse(pin.worldNormal, -LV, LC, Kd) * influence * influence;
		//���ʌ��̉��Z
		LS += BlinnPhongSpcular(pin.worldNormal, -LV, LC, E, Ks, 20) * influence * influence;
		
		//�X�|�b�g���C�g
		LV = pin.worldPosition.xyz - spotLight[i].position.xyz;
		d = length(LV);
		r = spotLight[i].range;
		float3 SFront = normalize(spotLight[i].dir.xyz);
		LV = normalize(LV);
		float angle = dot(LV, SFront);
		float area = spotLight[i].inner_com - spotLight[i].outer_com;
		float influence2 = spotLight[i].inner_com - angle;
		s = step(r, d);
		influence2 = lerp(saturate(1.0 - influence2 / area), 0, s * spotLight[i].type);
		influence = 1.0f;
		LC = spotLight[i].color.rgb;
		//�g�U���̉��Z
		LD += Diffuse(pin.worldNormal, -LV, LC, Kd) * influence * influence* influence2;
		//���ʌ��̉��Z
		LS += BlinnPhongSpcular(pin.worldNormal, -LV, LC, E, Ks, 20) * influence * influence* influence2;
	}
	color *= pin.color * float4(A + D + S + LD + LS, 1.0);
	return color;
}
