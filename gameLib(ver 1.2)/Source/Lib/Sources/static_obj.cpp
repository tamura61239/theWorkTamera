#include "static_obj.h"

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
/****************************���b�V���𐶐�*************************/

StaticObj::StaticObj(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType, bool pathOrganize, int organizeType):Obj3D()
{
	mMesh = std::make_shared<StaticMesh>(device, fileName, shaderType, pathOrganize, organizeType);
}
/*************************���b�V�����擾****************************/

StaticObj::StaticObj(std::shared_ptr<StaticMesh> mesh)
{
	mMesh = mesh;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@���C�s�b�N�֐�
/*****************************************************/

int StaticObj::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition, VECTOR3F* outPosition, VECTOR3F* outNormal, float* outLength)
{
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&GetWorld());
	DirectX::XMMATRIX invers_W;
	invers_W = DirectX::XMMatrixInverse(NULL, W);
	//�I�u�W�F�N�g��Ԃł̃��C�ɕϊ�
	DirectX::XMVECTOR startposition = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR endposition = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR loacal_start = DirectX::XMVector3TransformCoord(startposition, invers_W);
	DirectX::XMVECTOR loacal_end = DirectX::XMVector3TransformCoord(endposition, invers_W);
	//���C�s�b�N
	float outDistance;
	VECTOR3F start, end;
	DirectX::XMStoreFloat3(&start, loacal_start);
	DirectX::XMStoreFloat3(&end, loacal_end);
	int set = mMesh->RayPick(start, end, outPosition, outNormal, &outDistance);
	if (set != -1)
	{
		//�I�u�W�F�N�g��Ԃ��烏�[���h��Ԃ֕ϊ�
		DirectX::XMVECTOR out_p = DirectX::XMLoadFloat3(outPosition);
		DirectX::XMVECTOR out_n = DirectX::XMLoadFloat3(outNormal);

		DirectX::XMVECTOR world_position = DirectX::XMVector3TransformCoord(out_p, W);
		DirectX::XMVECTOR world_normal = DirectX::XMVector3TransformNormal(out_n, W);

		DirectX::XMStoreFloat3(outPosition, world_position);
		DirectX::XMStoreFloat3(outNormal, world_normal);

		*outLength = outDistance; // ���������Y��Ă���`

		return set;
	}
	return set;
}

