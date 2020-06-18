//#include"ps_function.hlsli"
//----------------------
//�|�C���g���C�g�\����
//---------------------
struct POINTLIGHT
{
	float dumy1;
	float range;
	float type;
	float dumy2;
	float4 position;
	float4 color;
};
#define POINTMAX 32
//----------------------
//�X�|�b�g���C�g�\����
//----------------------
struct SPOTLIGHT
{
	float index;
	float range;//���̓͂��͈�
	float type;//�L����������
	float inner_com;
	float outer_com;
	float dumy0;
	float dumy1;
	float dumy2;
	float4 position;
	float4 color;
	float4 dir;
};
#define SPOTMAX 32
cbuffer CbLight:register(b3)
{
	POINTLIGHT pointLight[POINTMAX];
	SPOTLIGHT spotLight[SPOTMAX];
}

struct Light
{
	float3 PD;
	float3 PS;
};
/****************************************/
//�|�C���g���C�g�v�Z�֐�
//pointLight:�|�C���g���C�g
//worldPosition:���[���h���W
//normal:�@��
//Kd:���˗�(0�`1.0)
//Ks:���ˌ�(�F�E����)
//E:���_�����x�N�g��(���K���ς�)
/****************************************/
//Light CalculationPL(POINTLIGHT pointLight, float3 normal
//	, float3 Kd, float3 Ks, float3 E,float3 PL, float d, float r)
//{
//	float influence;  //�|�C���g���C�g�̉e��
//	float3 PC;    //�|�C���g���C�g�F
//	influence = saturate(1.0f - d / r);
//	PL = normalize(PL);
//	PC = pointLight.color.rgb;
//	Light light;
//	//�g�U���̉��Z
//	//light.PD = Diffuse(N, PL, PC, Kd) * influence * influence;
//	////���ʌ��̉��Z
//	//light.PS = BlinnPhongSpcular(N, PL, PC, E, Ks, 20) * influence * influence;
//	return light;
//}
//Light IsPLt(POINTLIGHT pointLight, float3 worldPosition, float3 normal
//	, float3 Kd, float3 Ks, float3 E)
//{
//	float3 PL;   //�|�C���g���C�g�x�N�g��
//	PL = worldPosition - pointLight.position.xyz;
//	float d = length(PL);
//	float r = pointLight.range;
//	return lerp(CalculationPL(pointLight, normal, Kd, Ks, E,PL, d, r), (Light)0, step(r, d));
//}
//
//
//Light ChackPL(POINTLIGHT pointLight,float3 worldPosition,float3 normal
//	,float3 Kd,float3 Ks,float3 E)
//{
//	return lerp((Light)0, IsPLt(pointLight, worldPosition, normal, Kd, Ks, E), pointLight.type);
//}

