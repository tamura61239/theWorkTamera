#pragma once
#include<string>
#include<memory>
#include<d3d11.h>
#include<vector>
#include <wrl.h>

class ParticleCreateBase
{
public:
	//�R���X�g���N�^
	ParticleCreateBase() = default;
	ParticleCreateBase(ID3D11Device* device) {}
	//�t�@�C�����샍�[�h�֐�
	virtual void Load(std::string filePas) = 0;
	//�G�f�B�^�֐�
	virtual void Editor(void*id) =0;
	//�p�[�e�B�N���̐����֐�
	virtual void CreateParticle(ID3D11DeviceContext* context, float elapsdTime) =0;

	virtual ~ParticleCreateBase() = default;
protected:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mShader;
};