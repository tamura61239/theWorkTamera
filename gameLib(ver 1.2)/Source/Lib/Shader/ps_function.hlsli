//----------------------------------------
//  �g�U���ˊ֐�
//----------------------------------------
//N:�@��(���K���ς�)
//L:���˃x�N�g��(���K���ς�)
//C:���ˌ�(�F�E����)
//K:���˗�(0�`1.0)
float3 Diffuse(float3 N, float3 L, float3 C, float3 K)
{
	float D = dot(N, L);
	D = max(0, D);//���̒l��0�ɂ���
	return K * C * D;
}
//--------------------------------------
//���ʔ��ˊ֐�
//--------------------------------------
//N:�@��(���K���ς�)
//L:���˃x�N�g��(���K���ς�)
//C:���ˌ�(�F�E����)
//E:���_�����x�N�g��(���K���ς�)
//K:���ˌ�(�F�E����)
//Power:�n�C���C�g�̋���(�P���x)

float3 BlinnPhongSpcular(float3 N, float3 L, float3 C, float3 E, float3 K, float Power)
{
	//�n�[�t�x�N�g��
	float3 H = normalize(L + E);

	float3 S = dot(H, N);
	S = max(0, S);
	S = pow(S, Power);
	S = S * K * C;
	return S;
}

//-------------------------------------
//  �������C�g
//-------------------------------------
//N          :�@��
//SkyColor   :��(��)�F
//GroundColor:�n��(��)�F
float3 HemiSphereLight(float3 N, float3 SkyColor, float3 GroundColor)
{
	float skyblend = (N.y + 1.0) * 0.5;
	float groundblend = 1.0 - skyblend;
	return SkyColor * skyblend + GroundColor * groundblend;
}

