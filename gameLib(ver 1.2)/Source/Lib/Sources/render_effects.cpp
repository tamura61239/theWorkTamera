#include "render_effects.h"
#include"shader.h"
#include"misc.h"
#include"file_function.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

RenderEffects::RenderEffects(ID3D11Device* device, std::string fileName)
{
	HRESULT hr;
	//�V�F�[�_�[�𐶐�����
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Microsoft::WRL::ComPtr<ID3D11InputLayout>input;

	mShader = std::make_unique<DrowShader>(device, "Data/shader/render_effects_vs.cso", "", "Data/shader/render_effects_ps.cso");
	//�萔�o�b�t�@�쐬
	mCbBuffer = std::make_unique<ConstantBuffer<CbScene>>(device);
	//�t�@�C������f�[�^���擾
	std::string filePas = "Data/file/" + fileName + ".bin";

	FileFunction::Load(mCbBuffer->data.saveData, filePas.c_str(), "rb");
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void RenderEffects::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("shadow map data");
	//�p�����[�^�[�𒲐�����
	float* color[3] = { &mCbBuffer->data.saveData.shadowColor.x,&mCbBuffer->data.saveData.shadowColor.y,&mCbBuffer->data.saveData.shadowColor.z };
	ImGui::ColorEdit3("shadow color", *color);
	ImGui::InputFloat("shadow bisa", &mCbBuffer->data.saveData.shadowbisa, 0.0001f, 0, "%f");
	ImGui::InputFloat("slope scale bias", &mCbBuffer->data.saveData.slopeScaledBias, 0.0001f, 0, "%f");
	ImGui::InputFloat("depth bias clamp", &mCbBuffer->data.saveData.depthBiasClamp, 0.0001f, 0, "%f");
	//�Z�[�u����t�@�C���̖��O����͂���
	static char name[256] = "";
	ImGui::InputText("file name", name, 256);
	//�Z�[�u
	if (ImGui::Button("save"))
	{
		std::string fName = name;
		std::string filePas = "Data/file/" + fName + ".bin";

		FileFunction::Save(mCbBuffer->data.saveData, filePas.c_str(), "wb");
		strcpy_s(name, "");
	}

	ImGui::End();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
/*************************�e��t����****************************/

void RenderEffects::ShadowRender(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
	, const FLOAT4X4& view, const FLOAT4X4& projection, const FLOAT4X4& lightView, const FLOAT4X4& lightProjection)
{
	//�萔�o�b�t�@�̃f�[�^���X�V
	DirectX::XMStoreFloat4x4(&mCbBuffer->data.lightViewProjection, DirectX::XMLoadFloat4x4(&lightView) * DirectX::XMLoadFloat4x4(&lightProjection));
	DirectX::XMStoreFloat4x4(&mCbBuffer->data.inverseViewProjection, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection)));
	//GPU���Ƀf�[�^�𑗂�
	mCbBuffer->Activate(context, 0, true, true);

	mShader->Activate(context);

	context->PSSetShaderResources(0, 1, &colorMapSRV);
	context->PSSetShaderResources(1, 1, &depthMapSRV);
	context->PSSetShaderResources(2, 1, &shadowMapSRV);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�`��
	context->Draw(4, 0);
	//GPU���ɑ������f�[�^�����ɖ߂�
	mShader->Deactivate(context);

	mCbBuffer->DeActivate(context);
	ID3D11ShaderResourceView* nullSRV[3] = {};
	context->PSSetShaderResources(0, 3, nullSRV);
}
/*************************�e��t����(�V�F�[�_�[���擾)****************************/

void RenderEffects::ShadowRender(ID3D11DeviceContext* context, DrowShader* shader, ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
	, const FLOAT4X4& view, const FLOAT4X4& projection, const FLOAT4X4& lightView, const FLOAT4X4& lightProjection)
{
	//�萔�o�b�t�@�̃f�[�^���X�V
	DirectX::XMStoreFloat4x4(&mCbBuffer->data.lightViewProjection, DirectX::XMLoadFloat4x4(&lightView) * DirectX::XMLoadFloat4x4(&lightProjection));
	DirectX::XMStoreFloat4x4(&mCbBuffer->data.inverseViewProjection, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection)));
	//GPU���Ƀf�[�^�𑗂�
	mCbBuffer->Activate(context, 0, true, true);

	shader->Activate(context);

	context->PSSetShaderResources(0, 1, &colorMapSRV);
	context->PSSetShaderResources(1, 1, &depthMapSRV);
	context->PSSetShaderResources(2, 1, &shadowMapSRV);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�`��
	context->Draw(4, 0);
	//GPU���ɑ������f�[�^�����ɖ߂�
	shader->Deactivate(context);

	mCbBuffer->DeActivate(context);
	ID3D11ShaderResourceView* nullSRV[3] = {};
	context->PSSetShaderResources(0, 3, nullSRV);

}
