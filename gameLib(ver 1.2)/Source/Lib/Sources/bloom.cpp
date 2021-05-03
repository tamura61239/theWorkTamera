#include "bloom.h"
#include"shader.h"
#include"misc.h"
#include"vector.h"
#include<string>
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
BloomRender::BloomRender(ID3D11Device* device, float screenWidth, float screenHight, const int nowScene) :mNowEditorNo(nowScene), mNowScene(nowScene)
{
	memset(&mEditorData, 0, sizeof(mEditorData));
	unsigned int wight = static_cast<unsigned int>(screenWidth);
	unsigned int hight = static_cast<unsigned int>(screenHight);
	//�e�N�X�`����p��
	for (int i = 0; i < 5; i++)
	{
		mFrameBuffer.push_back(std::make_unique<FrameBuffer>(device, static_cast<int>(wight >> i), static_cast<int>(hight >> i), true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));
	}
	for (int i = 1; i < 5; i++)
	{
		mSidoFrameBuffer.push_back(std::make_unique<FrameBuffer>(device, static_cast<int>(wight >> i), static_cast<int>(hight >> i), true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));
	}
	mFrameBuffer.push_back(std::make_unique<FrameBuffer>(device, static_cast<int>(screenWidth), static_cast<int>(screenHight), true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS));

	//�V�F�[�_�[�̍쐬
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout>input;

	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/bloom_vs.cso", "", "Data/shader/bloomStart_ps.cso"));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/bloom_vs.cso", "", "Data/shader/combined_bloom.cso"));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/bloom_vs.cso", "", "Data/shader/bloom_blur01_ps.cso"));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/bloom_vs.cso", "", "Data/shader/bloom_blur02_ps.cso"));
	//�萔�o�b�t�@�̍쐬
	{
		mCBbuffer = std::make_unique<ConstantBuffer<CbBloom>>(device);
		mCbBluerbuffer = std::make_unique<ConstantBuffer<CbBluer>>(device);
	}
	//�t�@�C������p�����[�^�[���擾
	for (int i = 0; i < 4; i++)
	{
		std::string fileName = "Data/file/bloom" + std::to_string(i) + ".bin";

		FileFunction::Load(mEditorData[i], fileName.c_str(), "rb");
	}
	mDepth = std::make_unique<DepthStencilState>(device, false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_BACK);
	mSampler[0] = std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_ALWAYS, VECTOR4F(0, 0, 0, 0));
	mSampler[1] = std::make_unique<SamplerState>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_ALWAYS, VECTOR4F(0, 0, 0, 0));

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/
void BloomRender::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("bloom");
	for (int i = 1; i < 5; i++)
	{
		D3D11_VIEWPORT viewport = mFrameBuffer[i]->GetViewPort();
		ImVec2 size = ImVec2(192, 108);

		ImGui::Image(mFrameBuffer[i]->GetRenderTargetShaderResourceView().Get(), size);
		if (i < 4)ImGui::SameLine();
	}
	//�ǂ̃V�[���̎��Ɏg���u���[���𑀍삷�邩�����߂�
	ImGui::RadioButton("title bloom", &mNowEditorNo, 0); ImGui::SameLine();
	ImGui::RadioButton("select bloom", &mNowEditorNo, 1); ImGui::SameLine();
	ImGui::RadioButton("game bloom", &mNowEditorNo, 2); ImGui::SameLine();
	ImGui::RadioButton("result bloom", &mNowEditorNo, 3);

	auto& editorData = mEditorData[mNowEditorNo];
	//�p�����[�^�[�𒲐�����
	ImGui::RadioButton("blur 01", &editorData.mBlurType, 0); ImGui::SameLine();
	ImGui::RadioButton("blur 02", &editorData.mBlurType, 1);
	ImGui::SliderInt("filter count", &editorData.count, 0, 4);
	ImGui::SliderFloat("threshold", &editorData.threshold, 0, 10);
	ImGui::SliderFloat("widthBlur", &editorData.widthBlur, 0, 1);
	ImGui::SliderFloat("hightBlur", &editorData.hightBlur, 0, 1);
	ImGui::SliderFloat("devutation", &editorData.deviation, 0, 50);
	ImGui::SliderFloat("multiply", &editorData.multiply, 0, 10);
	int blurCount = static_cast<int>(editorData.blurCount);
	if (ImGui::SliderInt("blurCount", &blurCount, 1, 16))
	{
		editorData.blurCount = static_cast<float>(blurCount);
	}
	std::string fileName = "Data/file/bloom" + std::to_string(mNowEditorNo) + ".bin";
	//�Z�[�u
	if (ImGui::Button("save"))FileFunction::Save(editorData, fileName.c_str(),"rb");
	if (ImGui::Button("all save"))
	{
		for (int i = 0; i < 4; i++)
		{
			fileName = "Data/file/bloom" + std::to_string(i) + ".bin";
			FileFunction::Save(mEditorData[i], fileName.c_str(), "wb");
		}
	}
	ImGui::End();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/

/*************************�u���[���̏k�����ڂ������e�N�X�`���̍쐬******************************/
void BloomRender::BlurTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv)
{
	auto& editorData = mEditorData[mNowScene];
	if (editorData.count <= 0)return;
	mDepth->Activate(context);
	mRasterizer->Activate(context);
	mSampler[0]->Activate(context, 0, false, true);
	mSampler[1]->Activate(context, 2, false, true);

	mFrameBuffer[0]->Clear(context);
	mFrameBuffer[0]->Activate(context);
	//�萔�o�b�t�@�̐ݒ�
	mCBbuffer->data.blurCount = editorData.blurCount;
	mCBbuffer->data.hightBlur = editorData.hightBlur;
	mCBbuffer->data.threshold = editorData.threshold;
	mCBbuffer->data.widthBlur = editorData.widthBlur;
	mCBbuffer->Activate(context, 0, true, true);

	//�V�[��������点����������ʂ̃e�N�X�`���ɏ����o��
	mShader[0]->Activate(context);
	context->PSSetShaderResources(0, 1, &colorSrv);
	//context->IASetInputLayout(nullptr);
	//context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);
	mShader[0]->Deactivate(context);

	mFrameBuffer[0]->Deactivate(context);

	//�k�������e�N�X�`�����擾
	switch (editorData.mBlurType)
	{
	case 0:
		Blur01(context);
		break;
	case 1:
		Blur02(context);
		break;
	}

}
/********************�u���[����������********************/
void BloomRender::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv)
{
	auto& editorData = mEditorData[mNowScene];
	if (editorData.count <= 0)return;
	mDepth->Activate(context);
	mRasterizer->Activate(context);
	mSampler[0]->Activate(context, 0, false, true);
	mSampler[1]->Activate(context, 2, false, true);

	//�S�Ẵe�N�X�`��������
	mShader[1]->Activate(context);
	context->PSSetShaderResources(0, 1, &colorSrv);
	for (int i = 0; i < 3; i++)
	{
		context->PSSetShaderResources(i + 1, 1, mFrameBuffer[i]->GetRenderTargetShaderResourceView().GetAddressOf());
	}

	context->Draw(4, 0);
	//�㏈��
	mShader[1]->Deactivate(context);
	mCBbuffer->DeActivate(context);

	for (int i = 0; i < 4; i++)
	{
		ID3D11ShaderResourceView* srv = nullptr;
		context->PSSetShaderResources(i, 1, &srv);
	}
	mDepth->DeActive(context);
	mRasterizer->DeActivate(context);
	mSampler[0]->DeActivate(context);
	mSampler[1]->DeActivate(context);

}
/**************************�ڂ������e�N�X�`�����쐬������@�@********************************/
void BloomRender::Blur01(ID3D11DeviceContext* context)
{
	auto& editorData = mEditorData[mNowScene];

	mShader[2]->Activate(context);

	for (int i = 1; i < 5; i++)
	{
		if (editorData.count < i)
		{
			mFrameBuffer[i]->Clear(context);
			continue;
		}
		//�k�������e�N�X�`���̍쐬
		mFrameBuffer[i]->Clear(context);
		mFrameBuffer[i]->Activate(context);
		D3D11_VIEWPORT viewport = mFrameBuffer[i]->GetViewPort();
		//�u���[�̌v�Z������
		CalucurateBluer(viewport.Width, viewport.Height, VECTOR2F(editorData.widthBlur, editorData.hightBlur), editorData.deviation, editorData.multiply);
		//GPU���Ƀf�[�^�𑗂�
		mCbBluerbuffer->Activate(context, 1, true, true);
		//�O�ɍ�����e�N�X�`���𑗂�
		context->PSSetShaderResources(0, 1, mFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		//�`��
		context->Draw(4, 0);
		//GPU���ɑ������f�[�^�����ɖ߂�
		mCbBluerbuffer->DeActivate(context);
		ID3D11ShaderResourceView* srv = nullptr;
		context->PSSetShaderResources(0, 1, &srv);

		mFrameBuffer[i]->Deactivate(context);
	}
	mShader[2]->Deactivate(context);

}
/**************************�ڂ������e�N�X�`�����쐬������@�A********************************/

void BloomRender::Blur02(ID3D11DeviceContext* context)
{
	auto& editorData = mEditorData[mNowScene];

	mShader[3]->Activate(context);

	for (int i = 1; i < 5; i++)
	{
		if (editorData.count < i)
		{
			mFrameBuffer[i]->Clear(context);
			continue;
		}
		//���ɂڂ������e�N�X�`�����쐬
		mSidoFrameBuffer[i - 1]->Clear(context);
		mSidoFrameBuffer[i - 1]->Activate(context);
		
		D3D11_VIEWPORT viewport = mSidoFrameBuffer[i - 1]->GetViewPort();
		//�u���[�̌v�Z������
		CalucurateBluer(viewport.Width, viewport.Height, VECTOR2F(editorData.widthBlur, 0), editorData.deviation, editorData.multiply);
		//GPU���Ƀf�[�^�𑗂�
		mCbBluerbuffer->Activate(context, 1, true, true);
		//�O�ɍ�����e�N�X�`���𑗂�
		context->PSSetShaderResources(0, 1, mFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		//�`��
		context->Draw(4, 0);
		//GPU���ɑ������f�[�^�����ɖ߂�
		mCbBluerbuffer->DeActivate(context);
		ID3D11ShaderResourceView* srv = nullptr;
		context->PSSetShaderResources(0, 1, &srv);

		mSidoFrameBuffer[i - 1]->Deactivate(context);

		//�k�������e�N�X�`���̍쐬
		mFrameBuffer[i]->Clear(context);
		mFrameBuffer[i]->Activate(context);
		viewport = mFrameBuffer[i]->GetViewPort();
		//�u���[�̌v�Z������
		CalucurateBluer(viewport.Width, viewport.Height, VECTOR2F(0, editorData.hightBlur), editorData.deviation, editorData.multiply);
		//GPU���Ƀf�[�^�𑗂�
		mCbBluerbuffer->Activate(context, 1, true, true);
		//���ɂڂ������e�N�X�`���𑗂�
		context->PSSetShaderResources(0, 1, mSidoFrameBuffer[i - 1]->GetRenderTargetShaderResourceView().GetAddressOf());
		//�`��
		context->Draw(4, 0);
		//GPU���ɑ������f�[�^�����ɖ߂�
		mCbBluerbuffer->DeActivate(context);

		context->PSSetShaderResources(0, 1, &srv);

		mFrameBuffer[i]->Deactivate(context);
	}
	mShader[3]->Deactivate(context);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�u���[�̌v�Z�֐�
/*****************************************************/

/***********************�v�Z�Ɏg��*************************/

float BloomRender::GaussianDistribution(const VECTOR2F& position, const float rho)
{
	return static_cast<float>(exp(-(position.x * position.x + position.y * position.y) / (2.0f * rho * rho)));
}
/***********************�u���[�̌v�Z*************************/

void BloomRender::CalucurateBluer(const float width, const float hight, const VECTOR2F& dir, const float deviation, const float multiply)
{
	//�ő�l��1�Ƃ����Ƃ���
	float uvX = 1.0f / width;
	float uvY = 1.0f / hight;

	mCbBluerbuffer->data.mOffset[0].z = GaussianDistribution(VECTOR2F(0, 0), deviation) * multiply;
	float totalWeigh = mCbBluerbuffer->data.mOffset[0].z;

	mCbBluerbuffer->data.mOffset[0].x = 0;
	mCbBluerbuffer->data.mOffset[0].y = 0;
	//�����̌v�Z����
	for (int i = 1; i < 8; i++)
	{
		mCbBluerbuffer->data.mOffset[i].x = dir.x * i * uvX;
		mCbBluerbuffer->data.mOffset[i].y = dir.y * i * uvY;
		mCbBluerbuffer->data.mOffset[i].z = GaussianDistribution(dir * float(i), deviation) * multiply;
		totalWeigh += mCbBluerbuffer->data.mOffset[i].z * 2.0f;
	}
	for (int i = 0; i < 8; i++)
	{
		mCbBluerbuffer->data.mOffset[i].z /= totalWeigh;
	}
	//�v�Z�������𔽓]�������l���c�蔼���ɓ��Ă͂߂�
	for (auto i = 8; i < 15; ++i)
	{
		mCbBluerbuffer->data.mOffset[i].x = -mCbBluerbuffer->data.mOffset[i - 7].x;
		mCbBluerbuffer->data.mOffset[i].y = -mCbBluerbuffer->data.mOffset[i - 7].y;
		mCbBluerbuffer->data.mOffset[i].z = mCbBluerbuffer->data.mOffset[i - 7].z;
	}

}
