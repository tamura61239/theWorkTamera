#include"model_normal.hlsli"
#include"ps_function.hlsli"
#include"lightType.hlsli"

Texture2D diffuse_map : register(t0);
Texture2D normal_map : register(t1);
Texture2D bump_map : register(t2);
SamplerState diffuse_map_sampler_state : register(s0);
SamplerState normal_map_sampler_state : register(s1);

//�������C�g�p�����[�^ 
static const float3 skyColor = { 0.4,0.4,0.4 };
static const float3 groundColor = { 0.4,0.4,0.4 };

static const float viewOffset = 0.01f;

float4 main(VS_OUT pin) : SV_TARGET
{
	float2 texcoord = pin.texcoord;

	float3 E = normalize(eyePosition.xyz - pin.worldPosition.xyz);

	float3 L = normalize(-lightDirection.xyz);

	//�ڋ�Ԏ�
	float3 vx = normalize(pin.vT);
	float3 vy = normalize(pin.vB);
	float3 vz = normalize(pin.worldNormal);

	//���_�ڋ�ԕϊ�
	float3x3 inv_mat = { vx,vy,vz };
	float3 inv_E = normalize(mul(inv_mat, E));

	//�����v�Z
	float H = bump_map.Sample(normal_map_sampler_state, texcoord).r;
	H = H * 2.0f - 1.0f;
	texcoord.x -= inv_E.x * H * viewOffset;
	texcoord.y -= inv_E.y * H * viewOffset;
	//�e�N�X�`���F�擾
	float4 color = diffuse_map.Sample(diffuse_map_sampler_state, texcoord);
	float3 N = normal_map.Sample(normal_map_sampler_state, texcoord).xyz;
	N = N * 2.0f - 1.0f;

	//�@�����[���h�ϊ�
	float3x3 mat = { vx,vy,vz };
	N = normalize(mul(N, mat));

	//����
	float3 A = ambientColor.rgb * HemiSphereLight(N, skyColor, groundColor);

	//�g�U����
	float3 C = lightColor.rgb;
	float3 Kd = float3(1, 1, 1);
	float3 D = Diffuse(N, L, C, Kd);
	//���ʔ���
	float3 Ks = float3(1, 1, 1);
	float3 S = BlinnPhongSpcular(N, L, C, E, Ks, 20);

	float3 LV;   //���C�g�x�N�g��
	float3 LC;    //���C�g�F
	float influence;  //���C�g�̉e��
	float3 LD = (float3)0;
	float3 LS = (float3)0;
	for (int i = 0;i < POINTMAX;i++)
	{
		//�|�C���g���C�g
		LV = pin.worldPosition - pointLight[i].position.xyz;
		float d = length(LV);
		float r = pointLight[i].range;
		float s = step(r, d);
		influence = lerp(saturate(1.0f - d / r), 0, s * pointLight[i].type);
		LV = normalize(LV);
		LC = pointLight[i].color.rgb;
		//�g�U���̉��Z
		LD += Diffuse(N, -LV, LC, Kd) * influence * influence;
		//���ʌ��̉��Z
		LS += BlinnPhongSpcular(N, -LV, LC, E, Ks, 20) * influence * influence;

		//�X�|�b�g���C�g
		LV = pin.worldPosition - spotLight[i].position.xyz;
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
		LD += Diffuse(N, -LV, LC, Kd) * influence * influence * influence2;
		//���ʌ��̉��Z
		LS += BlinnPhongSpcular(N, -LV, LC, E, Ks, 20) * influence * influence * influence2;
	}
	color *= pin.color * float4(A + D + S + LD + LS, 1.0);
	return color;
}