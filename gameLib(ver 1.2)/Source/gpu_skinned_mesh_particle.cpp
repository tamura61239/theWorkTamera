#include "gpu_skinned_mesh_particle.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
#ifdef USE_IMGUI
#include<imgui.h>
#include"vector_combo.h"
#endif

/**************************************************/
// 初期化
/**************************************************/
GpuSkinnedMeshParticle::GpuSkinnedMeshParticle(ID3D11Device* device, const char* fileName) :GpuParticleTest(device)
{
	std::unique_ptr<ModelData>modelData = std::make_unique<ModelData>(fileName);
	std::shared_ptr<ModelResource>resouce = std::make_shared<ModelResource>(device, std::move(modelData));
	mModel = std::make_unique<Model>(resouce);
	CreateBuffer(device);
	mAnimNo = 0;
	mModel->PlayAnimation(mAnimNo, true);
	mCreateTime = 1;
	mTimer = 0;
	mCbCreate.startIndex = 0;

}

GpuSkinnedMeshParticle::GpuSkinnedMeshParticle(ID3D11Device* device, std::shared_ptr<ModelResource>& resource) : GpuParticleTest(device)
{
	mModel = std::make_unique<Model>(resource);
	CreateBuffer(device);
	mAnimNo = 0;
	mModel->PlayAnimation(mAnimNo, true);
	mCreateTime = 1;
	mTimer = 0;
	mCbCreate.startIndex = 0;
}

void GpuSkinnedMeshParticle::CreateBuffer(ID3D11Device* device)
{
	HRESULT hr;
	int totalIndex = 0;
	//メッシュ情報の取得
	for (auto& mesh : mModel->GetModelResource()->GetMeshData())
	{
		Mesh meshData;
		Microsoft::WRL::ComPtr<ID3D11Buffer>vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>indexBuffer;
		//頂点のバッファ
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			desc.ByteWidth = sizeof(ModelData::Vertex) * mesh.vertices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
			desc.StructureByteStride = sizeof(ModelData::Vertex);

			data.pSysMem = &mesh.vertices[0];
			hr = device->CreateBuffer(&desc, &data, vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		}
		//indexのバッファ
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			meshData.indexSize = mesh.indices.size();
			desc.ByteWidth = sizeof(int) * meshData.indexSize;
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
			desc.StructureByteStride = sizeof(int);

			data.pSysMem = &mesh.indices[0];
			hr = device->CreateBuffer(&desc, &data, indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		}
		//SRV
		{
			//頂点情報のSRV
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Buffer.NumElements = mesh.vertices.size();
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			hr = device->CreateShaderResourceView(vertexBuffer.Get(), &desc, meshData.mMeshBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			//index情報のSRV
			desc.BufferEx.NumElements = meshData.indexSize;
			hr = device->CreateShaderResourceView(indexBuffer.Get(), &desc, meshData.mIndexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//indexの合計値の取得
		totalIndex += meshData.indexSize;
		mMeshs.push_back(meshData);
	}
	int count = totalIndex / 10000;
	mMaxParticle = 50000 * (count + 1);
	//パーティクルのバッファ
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer>particleBuffer;
		//Microsoft::WRL::ComPtr<ID3D11Buffer>deactiveParticleBuffer;
		{
			D3D11_BUFFER_DESC desc;
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&desc, sizeof(desc));
			ZeroMemory(&data, sizeof(data));
			desc.ByteWidth = sizeof(Particle) * mMaxParticle;
			desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
			desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
			desc.StructureByteStride = sizeof(Particle);

			std::vector<Particle>particles;
			//std::vector<Particle>deactiveParticles;

			particles.resize(mMaxParticle);
			//deactiveParticles.resize(mMaxParticle);
			data.pSysMem = &particles[0];

			hr = device->CreateBuffer(&desc, &data, particleBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			//描画用バッファ
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			int size = sizeof(RenderParticle);

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
	//定数バッファ
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
		desc.ByteWidth = sizeof(CbBone);

		hr = device->CreateBuffer(&desc, nullptr, mCbBoneBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//shader生成
	hr = create_cs_from_cso(device, "Data/shader/gpu_skinned_mesh_create_praticle_cs.cso", mCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/gpu_skinned_mesh_particle_cs.cso", mUpdateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32_UINT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render2.cso", "", "Data/shader/particle_render_point_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	D3D11_TEXTURE2D_DESC desc;
	hr = load_texture_from_file(device, L"Data/image/○.png", mTexturte.GetAddressOf(), &desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
/**************************************************/
// 更新
/**************************************************/

void GpuSkinnedMeshParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("skinned mesh particle");
	ImGui::ColorEdit4("color", mEditorColor);
	ImGui::Text("colorData:%x", mCbCreate.color);
	ImGui::InputFloat("create time", &mCreateTime, 0.1f);
	VECTOR3F position = mObj->GetPosition();
	float* positionF[3] = { &position.x,&position.y ,&position.z };
	ImGui::DragFloat3("position", *positionF);
	mObj->SetPosition(position);
	VECTOR3F scale = mObj->GetScale();
	float* scaleF[3] = { &scale.x,&scale.y ,&scale.z };
	ImGui::DragFloat3("scale", *scaleF);
	mObj->SetScale(scale);
	VECTOR3F angle = mObj->GetAngle();
	float* angleF[3] = { &angle.x,&angle.y ,&angle.z };
	ImGui::DragFloat3("angle", *angleF);
	mObj->SetAngle(angle);
	ImGui::Text("index%f", mCbCreate.startIndex);
	std::vector<std::string>animName;
	for (auto& anime : mModel->GetModelResource()->GetAnimations())
	{
		animName.push_back(anime.animName);
	}
	int animNo = mAnimNo;
	ImGui::Combo("anim", &mAnimNo, vector_getter, static_cast<void*>(&animName), animName.size());
	if (animNo != mAnimNo)
	{
		mModel->PlayAnimation(mAnimNo, true);
	}
	ImGui::End();
#endif
}

void GpuSkinnedMeshParticle::Update(ID3D11DeviceContext* context, float elapsdTime)
{
	ImGuiUpdate();
	mObj->CalculateTransform();
	mModel->UpdateAnimation(elapsdTime);
	mModel->CalculateLocalTransform();
	mModel->CalculateWorldTransform(DirectX::XMLoadFloat4x4(&mObj->GetWorld()));
	context->CSSetShader(mCreateShader.Get(), nullptr, 0);
	ID3D11Buffer* buffers[] =
	{
		mCbCreateBuffer.Get(),
		mCbBoneBuffer.Get(),
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
	//生成
	if (mTimer > mCreateTime)
	{
		mCbCreate.color = 0;
		mCbCreate.color |= (static_cast<UINT>(mEditorColor[0] * 255) & 0x00FFFFFF) << 24;
		mCbCreate.color |= (static_cast<UINT>(mEditorColor[1] * 255) & 0x00FFFFFF) << 16;
		mCbCreate.color |= (static_cast<UINT>(mEditorColor[2] * 255) & 0x00FFFFFF) << 8;
		mCbCreate.color |= (static_cast<UINT>(mEditorColor[3] * 255) & 0x00FFFFFF) << 0;


		const ModelResource* resource = mModel->GetModelResource();
		const std::vector<Model::Node>& nodes = mModel->GetNodes();

		for (int j = 0; j < mMeshs.size(); j++)
		{
			auto& mesh = resource->GetMeshes()[j];
			CbBone cbBone;
			::memset(&cbBone, 0, sizeof(cbBone));
			if (mesh.node_indices.size() > 0)
			{
				for (size_t i = 0; i < mesh.node_indices.size(); ++i)
				{
					DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverse_transforms.at(i));
					DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.node_indices.at(i)).world_transform);
					DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
					DirectX::XMStoreFloat4x4(&cbBone.boneWorld[i], bone_transform);
				}
			}
			else
			{
				cbBone.boneWorld[0] = nodes.at(mesh.node_index).world_transform;
			}
			auto& mesh2 = mMeshs[j];
			if (mCbCreate.startIndex + mesh2.indexSize/3 >= mMaxParticle)
			{
				mCbCreate.startIndex = 0;
			}
			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &mCbCreate, 0, 0);
			context->UpdateSubresource(mCbBoneBuffer.Get(), 0, 0, &cbBone, 0, 0);

			srv[0] = mesh2.mMeshBuffer.Get();
			srv[1] = mesh2.mIndexBuffer.Get();
			context->CSSetShaderResources(0, ARRAYSIZE(srv), srv);
			context->Dispatch(mesh2.indexSize/3, 1, 1);
			mCbCreate.startIndex += mesh2.indexSize/3;
		}
		mTimer = 0;
	}
	//更新
	context->CSSetShader(mUpdateShader.Get(), nullptr, 0);
	CbUpdate cbUpdate;
	cbUpdate.elapsdTime = elapsdTime;
	context->UpdateSubresource(mCbUpdateBuffer.Get(), 0, 0, &cbUpdate, 0, 0);
	context->Dispatch(mMaxParticle / 100, 1, 1);

	//リセット
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
/**************************************************/
// 描画
/**************************************************/

void GpuSkinnedMeshParticle::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
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

}

