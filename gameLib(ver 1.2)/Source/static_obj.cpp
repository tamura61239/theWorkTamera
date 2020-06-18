#include "static_obj.h"

StaticObj::StaticObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType, bool pathOrganize, int organizeType):Obj3D()
{
	mMesh = std::make_unique<StaticMesh>(device, fileName, shaderType, pathOrganize, organizeType);
}

int StaticObj::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition, VECTOR3F* outPosition, VECTOR3F* outNormal, float* outLength)
{
	return 0;
}

