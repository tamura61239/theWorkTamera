#pragma once
#include"static_mesh.h"
#include"obj3d.h"

static const float gameObjScale = 2;
//���f���̃I�u�W�F�N�g�f�[�^
class StaticObj:public Obj3D
{
public:
	//�R���X�g���N�^
	StaticObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType = SHADER_TYPE::USEALLY, bool pathOrganize = false, int organizeType = 0);
	StaticObj(std::shared_ptr<StaticMesh>mesh);
	//���C�s�b�N
	int RayPick(
		const VECTOR3F& startPosition,//���C���΂��J�n���W
		const VECTOR3F& endPosition,//���C���΂��I�����W
		VECTOR3F* outPosition,//���C�������������W
		VECTOR3F* outNormal,//���C�����������ʂ̖@��
		float* outLength//���C�����������ʂ܂ł̋���
	);
	//getter
	StaticMesh* GetMesh() { return mMesh.get(); }
	
private:
	//���f���f�[�^
	std::shared_ptr<StaticMesh>mMesh;
};