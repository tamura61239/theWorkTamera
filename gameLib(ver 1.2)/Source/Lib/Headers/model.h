#pragma once
#include <memory>
#include <d3d11.h>
#include "model_resource.h"

//�A�j���[�V�������郂�f���̃N���X
class Model
{
public:
	//�R���X�g���N�^
	Model(std::shared_ptr<ModelResource>& resource);
	~Model() {}

	struct Node
	{
		const char*			name;
		Node*				parent;
		VECTOR3F	scale;
		VECTOR4F	rotate;
		VECTOR3F	translate;
		FLOAT4X4	localTransform;
		FLOAT4X4	worldTransform;
		FLOAT4X4    beforeWorldTransform;
	};

	// �A�j���[�V����
	//�I��
	bool IsPlayAnimation() const { return mCurrentAnimation >= 0; }
	//�J�n
	void PlayAnimation(int animationIndex, bool loop = false, float changeTime=0);
	//�X�V
	void UpdateAnimation(float elapsedTime);

	// �s��v�Z
	//�{�[�����[�J����ԍs��̍X�V
	void CalculateLocalTransform();
	//�{�[���̃��[���h��ԍs��̍X�V
	void CalculateWorldTransform(const DirectX::XMMATRIX& worldTransform);
	//getter
	const std::vector<Node>& GetNodes() const { return mNodes; }
	const ModelResource* GetModelResource() const { return mModelResource.get(); }

private:
	//�A�j���[�V�����u�����h
	void BlendAnimation(float elapsed_time);
	//���f���̃f�[�^
	std::shared_ptr<ModelResource>	mModelResource;
	std::vector<Node>				mNodes;
	std::vector<Node>				mChangeSceneNodes;
	//�A�j���[�V�����X�V�Ɏg���ϐ�
	int								mCurrentAnimation = -1;
	float							mCurrentSeconds = 0.0f;
	bool							mLoopAnimation = false;
	bool							mEndAnimation = false;
	bool                            mChangeAnimation = false;
	float                           mChangeTime = 0.f;

};
