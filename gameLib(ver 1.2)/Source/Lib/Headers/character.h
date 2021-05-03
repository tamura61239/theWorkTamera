#pragma once
#include<memory>
#include"obj3d.h"
#include"model_resource.h"
#include"model.h"
//�A�j���[�V��������I�u�W�F�N�g�̃N���X
class Character :public Obj3D
{
public:
	//�R���X�g���N�^
	Character(std::shared_ptr<ModelResource>resouce);
	//���[���h�s���{�[���s��̌v�Z������
	void CalculateBoonTransform(float elapsd_time);
	//getter
	Model* GetModel() { return mModel.get(); }
protected:
	//�A�j���[�V�������郂�f��
	std::unique_ptr<Model>mModel;
};