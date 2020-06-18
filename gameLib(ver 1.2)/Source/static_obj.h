#pragma once
#include"static_mesh.h"
#include"obj3d.h"

class StaticObj:public Obj3D
{
public:
	StaticObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType = SHADER_TYPE::USEALLY, bool pathOrganize = false, int organizeType = 0);
	int RayPick(
		const VECTOR3F& startPosition,//���C���΂��J�n���W
		const VECTOR3F& endPosition,//���C���΂��I�����W
		VECTOR3F* outPosition,//���C�������������W
		VECTOR3F* outNormal,//���C�����������ʂ̖@��
		float* outLength//���C�����������ʂ܂ł̋���
	);
	StaticMesh* GetMesh() { return mMesh.get(); }
private:
	std::unique_ptr<StaticMesh>mMesh;
};