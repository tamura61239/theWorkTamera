#pragma once
#include"framebuffer.h"
#include<vector>
#include"vector.h"
#include"constant_buffer.h"
#include"drow_shader.h"
#include"render_state.h"
#include"sampler_state.h"
//�u���[���N���X
class BloomRender
{
public:
	//�R���X�g���N�^
	BloomRender(ID3D11Device* device, float screenWidth, float screenHight,const int nowScene);
	//�G�f�B�^
	void Editor();
	//�u���[���Ɏg���e�N�X�`���̍쐬
	void BlurTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv);
	//�u���[����������
	void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv);
	//setter
	void SetNowScene(const int nowScene) 
	{ 
		mNowScene = nowScene; 
		mNowEditorNo = mNowScene;
	}
private:
	//�u���[���Ŏg���e�N�X�`���̐������@
	void Blur01(ID3D11DeviceContext* context);
	void Blur02(ID3D11DeviceContext* context);
	//�萔�o�b�t�@
	struct CbBloom
	{
		float threshold;
		float widthBlur;
		float hightBlur;
		float blurCount;
	};
	struct CbBluer
	{
		VECTOR4F mOffset[16];
	};
	std::unique_ptr<ConstantBuffer<CbBloom>>mCBbuffer;
	std::unique_ptr<ConstantBuffer<CbBluer>>mCbBluerbuffer;
	//�G�f�B�^�f�[�^
	struct EditorData
	{
		float threshold;
		float widthBlur;
		float hightBlur;
		float blurCount;
		int mBlurType;
		float deviation;
		float multiply;
		int count;
	};
	EditorData mEditorData[4];
	int mNowEditorNo;
	//�`��p�f�[�^
	std::vector<std::unique_ptr<FrameBuffer>>mFrameBuffer;
	std::vector<std::unique_ptr<FrameBuffer>>mSidoFrameBuffer;
	std::vector<std::unique_ptr<DrowShader>>mShader;
	std::unique_ptr<DepthStencilState>mDepth;
	std::unique_ptr<RasterizerState>mRasterizer;
	enum SamplerType
	{
		warp, anisotropic,max
	};
	std::unique_ptr<SamplerState>mSampler[SamplerType::max];
	//�`��Ɏg���f�[�^���v�Z����
	float GaussianDistribution(const VECTOR2F& position, const float rho);
	void CalucurateBluer(const float width, const float hight, const VECTOR2F& dir, const float deviation, const float multiply);
	//���̃V�[����ێ�����
	int mNowScene;
};