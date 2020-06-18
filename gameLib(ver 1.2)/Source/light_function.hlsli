//#include"lightType.hlsli"
//#include "static_mesh.hlsli"
//
//#include"ps_function.hlsli"
//struct Light
//{
//	float3 PD;
//	float3 PS;
//};
///****************************************/
////�|�C���g���C�g�v�Z�֐�
////pointLight:�|�C���g���C�g
////worldPosition:���[���h���W
////normal:�@��
////Kd:���˗�(0�`1.0)
////Ks:���ˌ�(�F�E����)
////E:���_�����x�N�g��(���K���ς�)
///****************************************/
//Light ChackPL(POINT_LIGHT pointLight,float3 worldPosition,float3 normal
//	,float3 Kd,float3 Ks,float3 E)
//{
//	return lerp((Light)0, IsPLt(pointLight, worldPosition, normal, Kd, Ks, E), pointLight.type);
//}
//
//Light IsPLt(POINT_LIGHT pointLight, float3 worldPosition, float3 normal
//	, float3 Kd, float3 Ks, float3 E)
//{
//	float3 PL;   //�|�C���g���C�g�x�N�g��
//	float3 PC;    //�|�C���g���C�g�F
//	PL = worldPosition - pointLight.position.xyz;
//	float d = length(PL);
//	float r = pointLight.range;
//	return lerp(CalculationPL(pointLight, normal, Kd, Ks, E, d, r), (Light)0, step(r, d));
//}
//
//Light CalculationPL(POINT_LIGHT pointLight, float3 normal
//	, float3 Kd, float3 Ks, float3 E,float d,float r)
//{
//	float influence;  //�|�C���g���C�g�̉e��
//	influence = saturate(1.0f - d / r);
//	PL = normalize(PL);
//	PC = PointLight[i].color.rgb;
//	Light light;
//	//�g�U���̉��Z
//	light.PD = Diffuse(N, PL, PC, Kd) * influence * influence;
//	//���ʌ��̉��Z
//	light.PS = BlinnPhongSpcular(N, PL, PC, E, Ks, 20) * influence * influence;
//	return light;
//}