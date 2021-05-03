#pragma once
#include"geometric_primitive.h"
#include"obj3d.h"
#include<memory>
#include"drow_shader.h"
#include"constant_buffer.h"
#include"render_state.h"
#include"sampler_state.h"
//�X�J�C�}�b�v�̌`
enum class MAPTYPE
{
	BOX,
	SPHERE
};
//�X�J�C�}�b�v�N���X
class SkyMap
{
public:
	//�R���X�g���N�^
	SkyMap(ID3D11Device* device, const wchar_t* textureName, MAPTYPE mapType);
	//getter
	Obj3D* GetPosData() { return mPosData.get(); }
	//�`��
	void Render(ID3D11DeviceContext* context, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void Render(ID3D11DeviceContext* context, DrowShader* shader, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	//setter
	void SaveBeforeWorld()
	{
		mCbBeforeObjBuffer->data = mPosData->GetWorld();
	}
private:
	//�萔�o�b�t�@
	struct Cb
	{
		VECTOR4F color;
		FLOAT4X4 world;

	};
	struct CbScene
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>mCbSceneBuffer;
	std::unique_ptr<ConstantBuffer<Cb>>mCbObjBuffer;
	std::unique_ptr<ConstantBuffer<FLOAT4X4>>mCbBeforeObjBuffer;
	//3D�I�u�W�F�N�g�̃f�[�^
	std::unique_ptr<Obj3D>mPosData;
	//���b�V���f�[�^
	std::unique_ptr<GeometricPrimitive>mObjData;
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mShader;
	//�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	//�`��X�e�[�g
	std::unique_ptr<SamplerState>mSampler;
	std::unique_ptr<RasterizerState>mRasterizer;
};