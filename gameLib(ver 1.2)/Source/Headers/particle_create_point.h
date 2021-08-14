#pragma once
#include"particle_create_base.h"
#include"constant_buffer.h"
#include"vector.h"
#include<string>

class ParticleCreatePoint :public ParticleCreateBase
{
public:
	//�R���X�g���N�^
	ParticleCreatePoint(ID3D11Device* device);
	//�t�@�C���f�[�^�̃��[�h
	void Load(std::string filePas);
	//�G�f�B�^�֐�
	void Editor(void* id);
	//�p�[�e�B�N�����֐�
	void CreateParticle(ID3D11DeviceContext* context, float elapsdTime);
	//getter
	//std::string GetFilePas() { return mFilePas; }
	~ParticleCreatePoint()
	{

	}
private:
	//GPU���ɑ��M����f�[�^
	struct Point
	{
		VECTOR3F position;
		bool directionSpecifyFlag;
		VECTOR3F releaseDirection;
		float angleToFine;
	};
	struct Create
	{
		//Point point[15];
		int createCount;
		VECTOR3F dummy;
	};
	//�萔�o�b�t�@
	std::unique_ptr<ConstantBuffer<VECTOR4F>>mCreateBuffer;
	//�G�f�B�^�ő��삷��f�[�^
	struct EditData
	{
		Point point[15];
		int createCount;
		int pointCount;
	};
	EditData mEditData;
	std::string mFilePas;
	float mTotalCreateCount;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mShader;

};