#include "logger.h"
#include "model_renderer.h"
#include "misc.h"
#include"shader.h"
#include"camera_manager.h"
#include"texture.h"
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

ModelRenderer::ModelRenderer(ID3D11Device* device)
{
	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/model_vs.cso", "", "Data/shader/model_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc)));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/model_normal_vs.cso", "", "Data/shader/model_normal_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc)));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/model_motion_data_vs.cso", "Data/shader/model_motion_data_gs.cso", "Data/shader/model_motion_data_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc)));
	mShadowShader = std::make_unique<DrowShader>(device, "Data/shader/model_shadow_vs.cso", "", "", input_element_desc, ARRAYSIZE(input_element_desc));
	// �萔�o�b�t�@
	{
		mCbScene = std::make_unique<ConstantBuffer<CbScene>>(device);
		mCbMesh = std::make_unique<ConstantBuffer<CbMesh>>(device);
		mCbSubset = std::make_unique<ConstantBuffer<CbSubset>>(device);
	}



	// �_�~�[�e�N�X�`��
	{
		HRESULT hr = MakeDummyTexture(device, mDummySRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�ʏ�`��֐�
/*****************************************************/
/*************************�`��J�n***************************/
void ModelRenderer::Begin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection)
{
	// �V�[���p�萔�o�b�t�@�X�V
	CbScene cb_scene;
	mCbScene->data.viewProjection = view_projection;

	mCbScene->Activate(context, 0, true, true, true);
}

/**************************** �`��*************************/
void ModelRenderer::Draw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color)
{
	//�`��Ɏg�����f���f�[�^�̎擾

	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();

	SHADER_TYPE shaderType = model_resource->GetShaderType();
	//�V�F�[�_�[��ݒ肷��
	mShader[static_cast<int>(shaderType)]->Activate(context);
	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		SetRenderBoneTransform(mesh, nodes);
		//PU���Ƀf�[�^�𑗂�
		mCbMesh->Activate(context, 1, true, true, true);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			mCbSubset->data.materialColor = VECTOR4F(subset.diffuse->color.x * color.x, subset.diffuse->color.y * color.y, subset.diffuse->color.z * color.z, subset.diffuse->color.w * color.w);
			mCbSubset->Activate(context, 2, true, true, true);
			context->PSSetShaderResources(0, 1, subset.diffuse->SRV.Get() ? subset.diffuse->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			if (shaderType == SHADER_TYPE::NORMAL)
			{
				context->PSSetShaderResources(1, 1, subset.normal->SRV.Get() ? subset.normal->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
				context->PSSetShaderResources(2, 1, subset.bump->SRV.Get() ? subset.bump->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			}
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
			mCbSubset->DeActivate(context);
		}
		//GPU���ɑ������f�[�^�����ɖ߂�

		mCbMesh->DeActivate(context);
	}
	mShader[static_cast<int>(shaderType)]->Deactivate(context);
}
/***********************************�`��(�V�F�[�_�[���擾����)****************************************/
void ModelRenderer::Draw(ID3D11DeviceContext* context, DrowShader* shader, Model& model, const VECTOR4F& color)
{
	//�`��Ɏg�����f���f�[�^�̎擾

	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();

	SHADER_TYPE shaderType = model_resource->GetShaderType();
	//�V�F�[�_�[��ݒ肷��

	shader->Activate(context);
	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		SetRenderBoneTransform(mesh, nodes);
		//PU���Ƀf�[�^�𑗂�
		mCbMesh->Activate(context, 1, true, true, true);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			mCbSubset->data.materialColor = VECTOR4F(subset.diffuse->color.x * color.x, subset.diffuse->color.y * color.y, subset.diffuse->color.z * color.z, subset.diffuse->color.w * color.w);
			mCbSubset->Activate(context, 2, true, true, true);
			context->PSSetShaderResources(0, 1, subset.diffuse->SRV.Get() ? subset.diffuse->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			if (shaderType == SHADER_TYPE::NORMAL)
			{
				context->PSSetShaderResources(1, 1, subset.normal->SRV.Get() ? subset.normal->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
				context->PSSetShaderResources(2, 1, subset.bump->SRV.Get() ? subset.bump->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			}
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
			mCbSubset->DeActivate(context);
		}
		//GPU���ɑ������f�[�^�����ɖ߂�

		mCbMesh->DeActivate(context);

	}
	shader->Deactivate(context);

}

/**************************** �`��I��***************************/
void ModelRenderer::End(ID3D11DeviceContext* context)
{
	//GPU���ɑ������f�[�^�����ɖ߂�

	mCbScene->DeActivate(context);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@	�e�̕`��֐�
/*****************************************************/
/*************************�`��J�n***************************/

void ModelRenderer::ShadowBegin(ID3D11DeviceContext* context)
{
	//�V�F�[�_�[��ݒ肷��

	mShadowShader->Activate(context);



}
/**************************** �`��*************************/

void ModelRenderer::ShadowDraw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color)
{
	//�`��Ɏg�����f���f�[�^�̎擾

	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();

	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		SetRenderBoneTransform(mesh, nodes);
		//PU���Ƀf�[�^�𑗂�
		mCbMesh->Activate(context, 1, true, true, true);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			mCbSubset->data.materialColor = VECTOR4F(subset.diffuse->color.x * color.x, subset.diffuse->color.y * color.y, subset.diffuse->color.z * color.z, subset.diffuse->color.w * color.w);
			mCbSubset->Activate(context, 2, true, true);
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
			mCbSubset->DeActivate(context);
		}
		//GPU���ɑ������f�[�^�����ɖ߂�

		mCbMesh->DeActivate(context);
	}

}
/**************************** �`��I��***************************/
void ModelRenderer::ShadowEnd(ID3D11DeviceContext* context)
{
	//GPU���ɑ������f�[�^�����ɖ߂�

	mShadowShader->Deactivate(context);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@	���x�}�b�v�̕`��֐�
/*****************************************************/
/*************************�`��J�n***************************/

void ModelRenderer::VelocityBegin(ID3D11DeviceContext* context)
{
	//�V�F�[�_�[��ݒ肷��

	mShader[2]->Activate(context);

}
/**************************** �`��*************************/

void ModelRenderer::VelocityDraw(ID3D11DeviceContext* context, Model& model)
{
	//�`��Ɏg�����f���f�[�^�̎擾
	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();
	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		SetRenderBoneTransform(mesh, nodes);
		//PU���Ƀf�[�^�𑗂�
		mCbMesh->Activate(context, 1, true, true, true);
		
		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//�`��
		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
		//GPU���ɑ������f�[�^�����ɖ߂�
		mCbMesh->DeActivate(context);
	}

}
/**************************** �`��I��***************************/

void ModelRenderer::VelocityEnd(ID3D11DeviceContext* context)
{
	//GPU���ɑ������f�[�^�����ɖ߂�
	mShader[2]->Deactivate(context);
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@	�萔�o�b�t�@�̃f�[�^�X�V�֐�
/*****************************************************/

void ModelRenderer::SetRenderBoneTransform(const ModelResource::Mesh& mesh, const std::vector<Model::Node>& nodes)
{
	if (mesh.nodeIndices.size() > 0)
	{//�{�[��������ꍇ
		for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
		{
			DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverseTransforms.at(i));
			DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
			DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
			DirectX::XMStoreFloat4x4(&mCbMesh->data.boneTransforms[i], bone_transform);
			DirectX::XMMATRIX beforeWorldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).beforeWorldTransform);
			bone_transform = inverse_transform * beforeWorldTransform;
			DirectX::XMStoreFloat4x4(&mCbMesh->data.beforeBoneTransforms[i], bone_transform);

		}
	}
	else
	{
		mCbMesh->data.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
		mCbMesh->data.beforeBoneTransforms[0] = nodes.at(mesh.nodeIndex).beforeWorldTransform;
	}

}
