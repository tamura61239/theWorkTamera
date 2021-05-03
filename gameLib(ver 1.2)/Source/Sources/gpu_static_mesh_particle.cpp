#include "gpu_static_mesh_particle.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
#include"light.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
#if (PARTICLE_SYSTEM_TYPE==0)

/**********************************************************/
//  ������
/**********************************************************/
GpuStaticMeshParticle::GpuStaticMeshParticle(ID3D11Device* device, const char* fileName):GpuParticleTest(device)
{
	mCbCreate.color = VECTOR3F(1, 1, 1);
	mCbCreate.startIndex = 0;
	mCreateTime = 1;
	mTimer = 0;
	mMesh = std::make_unique<StaticMesh>(device, fileName);
	mRender = std::make_unique<MeshRender>(device);
	CreateBuffer(device, mMesh.get());
}

GpuStaticMeshParticle::GpuStaticMeshParticle(ID3D11Device* device, StaticMesh* mesh) :GpuParticleTest(device)
{
	//mCbCreate.color = VECTOR3F(1, 1, 1);
	//mCbCreate.startIndex = 0;
	//mCreateTime = 1;
	//mTimer = 0;
	//CreateBuffer(device, mesh);
}

void GpuStaticMeshParticle::CreateBuffer(ID3D11Device* device, StaticMesh* mesh)
{
	HRESULT hr;
	int totalIndex = 0;
	//���b�V�����̎擾
	for (auto& m : mesh->meshes)
	{
		Mesh meshData;
		Microsoft::WRL::ComPtr<ID3D11Buffer>vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>indexBuffer;
		//���_�̃o�b�t�@
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));

			desc.ByteWidth = sizeof(StaticMesh::Vertex) * m.vertices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
			desc.StructureByteStride = sizeof(StaticMesh::Vertex);

			data.pSysMem = &m.vertices[0];
			hr = device->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}
		//index�̃o�b�t�@
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			meshData.indexSize = m.indices.size();
			desc.ByteWidth = sizeof(u_int) * meshData.indexSize;
			desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
			desc.StructureByteStride = sizeof(u_int);

			data.pSysMem = &m.indices[0];
			hr = device->CreateBuffer(&desc, &data, indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		}

		//SRV
		{
			//���_����SRV
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Buffer.NumElements = m.vertices.size();
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			hr = device->CreateShaderResourceView(vertexBuffer.Get(), &desc, meshData.mMeshBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			//index����SRV
			desc.BufferEx.NumElements = meshData.indexSize;
			hr = device->CreateShaderResourceView(indexBuffer.Get(), &desc, meshData.mIndexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//index�̍��v�l�̎擾
		totalIndex += meshData.indexSize;
		mMeshs.push_back(meshData);
	}
	int count = totalIndex / 10000;
	mMaxParticle = 50000 * (count + 1);
	//�p�[�e�B�N���̃o�b�t�@
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
		//Microsoft::WRL::ComPtr<ID3D11Buffer>deactiveParticleBuffer;
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			desc.ByteWidth = sizeof(Particle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//�X�e�[�W�̓��o�͂�OK�BGPU�̓��o��OK�B
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // �\�����o�b�t�@
			desc.StructureByteStride = sizeof(Particle);

			std::vector<Particle>particles;
			//std::vector<Particle>deactiveParticles;

			particles.resize(mMaxParticle);
			//deactiveParticles.resize(mMaxParticle);
			data.pSysMem = &particles[0];

			hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//�`��p�o�b�t�@
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.StructureByteStride = 0;
			std::vector<RenderParticle>renderParticles;
			renderParticles.resize(mMaxParticle);
			data.pSysMem = &renderParticles[0];
			hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Buffer.NumElements = mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		ZeroMemory(&desc, sizeof(desc));

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;

		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//�萔�o�b�t�@
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(CbUpdate);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbUpdateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbCreate);

		hr = device->CreateBuffer(&desc, nullptr, mCbCreateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//shader����
	hr = create_cs_from_cso(device, "Data/shader/gpu_static_mesh_create_particle_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/gpu_static_mesh_particle_cs.cso", mUpdateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	D3D11_TEXTURE2D_DESC desc;
	hr = load_texture_from_file(device, L"Data/image/��.png", mTexturte.GetAddressOf(), &desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

/**********************************************************/
//  �X�V
/**********************************************************/

void GpuStaticMeshParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("static mesh particle");
	float* color[3] = { &mCbCreate.color.x,&mCbCreate.color.y,&mCbCreate.color.z };
	ImGui::ColorEdit3("color", *color);
	UINT fColor = 0;
	fColor |= (static_cast<UINT>(mCbCreate.color.x * 255) & 0x00FFFFFF)<<16;
	fColor |= (static_cast<UINT>(mCbCreate.color.y * 255) & 0x00FFFFFF)<<8;
	fColor |= (static_cast<UINT>(mCbCreate.color.z * 255) & 0x00FFFFFF)<<0;
	ImGui::Text("color%x", fColor);
	ImGui::InputFloat("create time", &mCreateTime, 0.1f);
	VECTOR3F position = mObj->GetPosition();
	float* fposition[3] = { &position.x,&position.y ,&position.z };
	ImGui::DragFloat3("position", *fposition, 1);
	mObj->SetPosition(position);
	VECTOR3F angle = mObj->GetAngle();
	float* fangle[3] = { &angle.x,&angle.y ,&angle.z };
	ImGui::DragFloat3("angle", *fangle, 1);
	mObj->SetAngle(angle);

	VECTOR3F scale = mObj->GetScale();
	float* fscale[3] = { &scale.x,&scale.y ,&scale.z };
	ImGui::DragFloat3("scale", *fscale, 1);
	mObj->SetScale(scale);

	ImGui::End();
#endif
}

void GpuStaticMeshParticle::Update(ID3D11DeviceContext* context, float elapsdTime)
{
	ImGuiUpdate();
	context->CSSetShader(mCreateShader.Get(), nullptr, 0);
	ID3D11Buffer* buffers[] =
	{
		mCbCreateBuffer.Get(),
		mCbUpdateBuffer.Get(),
	};
	context->CSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
	ID3D11ShaderResourceView* srv[2] =
	{
		nullptr,nullptr
	};
	ID3D11UnorderedAccessView* uav[] =
	{
		mParticleUAV.Get(),
		mRenderUAV.Get(),
	};
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, nullptr);
	mTimer += elapsdTime;
	//����
	if (mTimer > mCreateTime)
	{
		mObj->CalculateTransform();
		mCbCreate.world = mObj->GetWorld();
		for (auto& mesh : mMeshs)
		{
			if (mCbCreate.startIndex+ mesh.indexSize/3>=mMaxParticle)
			{
				mCbCreate.startIndex = 0;
			}
			srv[0] = mesh.mMeshBuffer.Get();
			srv[1] = mesh.mIndexBuffer.Get();
			context->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &mCbCreate, 0, 0);
			context->Dispatch(mesh.indexSize/3, 1, 1);
			mCbCreate.startIndex += mesh.indexSize/3;
		}
		mTimer = 0;
	}
	//�X�V
	context->CSSetShader(mUpdateShader.Get(), nullptr, 0);
	CbUpdate cbUpdate;
	cbUpdate.elapsdTime = elapsdTime;
	context->UpdateSubresource(mCbUpdateBuffer.Get(), 0, 0, &cbUpdate, 0, 0);
	context->Dispatch(mMaxParticle/100, 1, 1);

	//���Z�b�g
	for (int i = 0; i < ARRAYSIZE(srv); i++)
	{
		srv[i] = nullptr;
	}
	for (int i = 0; i < ARRAYSIZE(uav); i++)
	{
		uav[i] = nullptr;
	}
	for (int i = 0; i < ARRAYSIZE(buffers); i++)
	{
		buffers[i] = nullptr;
	}
	context->CSSetConstantBuffers(0, ARRAYSIZE(buffers), buffers);
	context->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uav), uav, nullptr);
	context->CSSetShader(nullptr, nullptr, 0);

}
/**********************************************************/
//  �`��
/**********************************************************/

void GpuStaticMeshParticle::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	ID3D11Buffer* constant_buffers[] =
	{
		mCbSceneBuffer.Get(),
	};
	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->GSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);

	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	context->RSSetState(mRasterizerState.Get());
	CbScene cbScene;
	cbScene.view = view;
	cbScene.projection = projection;
	context->UpdateSubresource(mCbSceneBuffer.Get(), 0, 0, &cbScene, 0, 0);
	mShader->Activate(context);
	context->IASetInputLayout(mInput.Get());

	UINT stride = sizeof(RenderParticle);
	UINT offset = 0;
	ID3D11Buffer* vbs[]
	{
		mRenderBuffer.Get()
	};
	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->PSSetShaderResources(0, 1, mTexturte.GetAddressOf());
	context->Draw(mMaxParticle, 0);
	mShader->Deactivate(context);

	vbs[0] = nullptr;
	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);
	context->PSSetShaderResources(0, 0, nullptr);

	mRender->Begin(context, pLight.GetLightDirection(), view, projection);
	mRender->Render(context, mMesh.get(), mObj->GetWorld());
	mRender->End(context);
}
#elif (PARTICLE_SYSTEM_TYPE==1)

#endif
