#include "gpu_particle_test.h"
#include<vector>
#include<random>
#include "misc.h"
#include"shader.h"
#include"vector_combo.h"
#include"file_function.h"
#include"texture.h"
#include"color_anim.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

#if (PARTICLE_SYSTEM_TYPE==0)
//
//GpuParticleTest::GpuParticleTest(ID3D11Device* device) :mMaxParticle(0), mVertexSize(sizeof(Vertex))
//{
//	mObj = std::make_unique<Obj3D>();
//	mObj->CalculateTransform();
//	HRESULT hr = S_OK;
//
//	// create rasterizer state : solid mode
//	{
//		D3D11_RASTERIZER_DESC rasterizerDesc = {};
//		rasterizerDesc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
//		rasterizerDesc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
//		rasterizerDesc.FrontCounterClockwise = FALSE;
//		rasterizerDesc.DepthBias = 0;
//		rasterizerDesc.DepthBiasClamp = 0;
//		rasterizerDesc.SlopeScaledDepthBias = 0;
//		rasterizerDesc.DepthClipEnable = true;
//		rasterizerDesc.ScissorEnable = FALSE;
//		rasterizerDesc.MultisampleEnable = false;
//		rasterizerDesc.AntialiasedLineEnable = FALSE;
//		hr = device->CreateRasterizerState(&rasterizerDesc, mRasterizerState.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	}
//	// �[�x�X�e���V���X�e�[�g�̐ݒ�
//	{
//		//D3D11_DEPTH_STENCIL_DESC desc;
//		//::memset(&desc, 0, sizeof(desc));
//		//desc.DepthEnable = true;
//		//desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//		//desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
//		//hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
//
//		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
//		depth_stencil_desc.DepthEnable = true;
//		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
//		depth_stencil_desc.StencilEnable = false;
//		hr = device->CreateDepthStencilState(&depth_stencil_desc, mDepthStencilState.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	}
//	// create constant buffer
//	{
//		D3D11_BUFFER_DESC bufferDesc = {};
//		bufferDesc.ByteWidth = sizeof(CbScene);
//		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
//		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//		bufferDesc.CPUAccessFlags = 0;
//		bufferDesc.MiscFlags = 0;
//		bufferDesc.StructureByteStride = 0;
//		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbSceneBuffer.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//		bufferDesc.ByteWidth = sizeof(CbObj);
//
//		hr = device->CreateBuffer(&bufferDesc, nullptr, mCbObjBuffer.GetAddressOf());
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	}
//
//	// create sampler state
//	D3D11_SAMPLER_DESC samplerDesc = {};
//	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//	samplerDesc.MipLODBias = 0;
//	samplerDesc.MaxAnisotropy = 1;
//	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//	memcpy(samplerDesc.BorderColor, &VECTOR4F(0.0f, 0.0f, 0.0f, 0.0f), sizeof(VECTOR4F));
//	samplerDesc.MinLOD = -FLT_MAX;
//	samplerDesc.MaxLOD = FLT_MAX;
//	hr = device->CreateSamplerState(&samplerDesc, mSamplerState.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//}
//
//void GpuParticleTest::Update(ID3D11DeviceContext* context, float elapsd_time)
//{
//	if (!mCSShader)return;
//	ID3D11UnorderedAccessView* uavs[] = { mUAV.Get() };
//	ID3D11ComputeShader* compute = mCSShader.Get();
//	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
//	context->CSSetShader(compute, nullptr, 0);
//
//	//���s
//	context->Dispatch(mMaxParticle / 100, 1, 1);
//
//	uavs[0] = nullptr;
//	context->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
//
//	compute = nullptr;
//	context->CSSetShader(compute, nullptr, 0);
//
//}
//
//void GpuParticleTest::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
//{
//	ID3D11Buffer* constant_buffers[] =
//	{
//		mCbSceneBuffer.Get(),
//		mCbObjBuffer.Get(),
//	};
//	context->VSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
//	context->PSSetConstantBuffers(0, ARRAYSIZE(constant_buffers), constant_buffers);
//
//	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
//	context->RSSetState(mRasterizerState.Get());
//	CbScene cbScene;
//	cbScene.view = view;
//	cbScene.projection = projection;
//	context->UpdateSubresource(mCbSceneBuffer.Get(), 0, 0, &cbScene, 0, 0);
//	CbObj cbObj;
//	cbObj.world = mObj->GetWorld();
//	context->UpdateSubresource(mCbObjBuffer.Get(), 0, 0, &cbObj, 0, 0);
//	context->VSSetShader(mVSShader.Get(), nullptr, 0);
//	context->PSSetShader(mPSShader.Get(), nullptr, 0);
//	context->IASetInputLayout(mInput.Get());
//
//	UINT stride = mVertexSize;
//	UINT offset = 0;
//	ID3D11Buffer* vbs[]
//	{
//		mVertexBuffer.Get()
//	};
//	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);
//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//	context->PSSetShaderResources(0, 0, nullptr);
//	context->Draw(mMaxParticle, 0);
//	vbs[0] = nullptr;
//	context->IASetVertexBuffers(0, ARRAYSIZE(vbs), vbs, &stride, &offset);
//
//}
#elif (PARTICLE_SYSTEM_TYPE==1)
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

GpuParticle::GpuParticle(ID3D11Device* device, std::string name, std::string filePath) :mIndexCount(0), mParticleCount(0), mMaxParticle(500000), mCreateCount(0), mCountCrease(1),mDrowType(0), mTexNo(0)
{
	//�p�[�e�B�N���o�b�t�@�̐���
	std::vector<ParticleData>particleData;
	particleData.resize(mMaxParticle);
	mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particleData, true);
	//�`��p�f�[�^�̐���
	std::vector<RenderParticle>renderData;
	renderData.resize(mMaxParticle);
	mRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderData, true);
	//Index�f�[�^�̐���
	std::vector<UINT>indices;
	indices.resize(mMaxParticle);
	memset(indices.data(), 0, sizeof(UINT) * mMaxParticle);
	for (int i = 0; i < 2; i++)mIndex[i] = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
	for (int i = 0; i < mMaxParticle; i++)
	{
		indices[i] = i;
	}
	mDeathIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
	ParticleCount count;
	memset(&count, 0, sizeof(ParticleCount));
	count.deActiveParticleCount = mMaxParticle;
	mCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, count, true, false, D3D11_CPU_ACCESS_READ);
	//�萔�o�b�t�@����
	mCreateData = std::make_unique<ConstantBuffer<CreateData>>(device);
	mCbTimer = std::make_unique<ConstantBuffer<CbTimer>>(device);
	HRESULT hr = S_OK;
	//�V�F�[�_�[�̐���
	hr = CreateCSFromCso(device, "Data/shader/point_create_particle.cso", mCresteShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_last_move.cso", mLastMoveShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_reset.cso", mCountResetShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mName = name;
	mFilePath = filePath;
	mMoves.push_back(std::make_unique<ColorAnim>(device));
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void GpuParticle::Editor(std::vector<std::string>shaders, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>texs)
{
	std::string name = "particle(" + mName + ")";
#ifdef USE_IMGUI
	ImGui::Begin(name.c_str());
	ImGui::InputInt("count", &mCreateCount);
	ImGui::DragFloat3(u8"�������W", &mCreateData->data.position.x);
	ImGui::DragFloat3(u8"�p�[�e�B�N���T�C�Y", &mCreateData->data.scale.x);
	ImGui::DragFloat(u8"��������", &mCreateData->data.life);
	ImGui::DragFloat(u8"���x", &mCreateData->data.speed);
	ImGui::DragFloat(u8"�����x", &mCreateData->data.accel);
	ImGui::ColorPicker4("color", &mCreateData->data.color.x);
	ImGui::Text("%d", mParticleCount);
	ImGuiID id = ImGui::GetID((void*)0);
	DrowEditorFunction(&id, shaders, texs);
	
	for (int i = 0; i < static_cast<int>(mMoves.size()); i++)
	{
		id = ImGui::GetID((void*)(i+1));
		mMoves[i]->Editor(&id);
	}
	ImGui::End();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void GpuParticle::Update(ID3D11DeviceContext* context, float elapsdTime)
{
	mParticle->Activate(context, 0, true);
	mCount->Activate(context, 1, true);
	mRender->Activate(context, 2, true);

	mIndex[mIndexCount]->Activate(context, 3, true);
	mIndex[1-mIndexCount]->Activate(context, 4, true);
	mDeathIndex->Activate(context, 5, true);

	mIndexCount++;
	if (mIndexCount >= 2)
	{
		mIndexCount = 0;
	}
	

	//����
	mCbTimer->data.elapsdTime = elapsdTime;
	mCbTimer->Activate(context, 0, false, false, false, true);
	mCreateData->Activate(context, 1, false, false, false, true);

	context->CSSetShader(mCresteShader.Get(), nullptr, 0);

	UINT count = static_cast<UINT>(mCreateCount * elapsdTime);
	if (count > 0)
	{
		context->Dispatch(count, 1, 1);
	}

	//�X�V
	for (auto& move : mMoves)
	{
		move->Move(context, mMaxParticle / 100);
	}

	//�`��p�̃o�b�t�@�Ƀf�[�^���ڂ�
	context->CSSetShader(mLastMoveShader.Get(), nullptr, 0);
	context->Dispatch(mMaxParticle / 100, 1, 1);


	//�J�E���g���X�V
	context->CSSetShader(mCountResetShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);
	//�㏈��
	mCbTimer->DeActivate(context);
	mCreateData->DeActivate(context);
	mParticle->DeActivate(context);
	mIndex[mIndexCount]->DeActivate(context);
	mIndex[1 - mIndexCount]->DeActivate(context);
	mDeathIndex->DeActivate(context);
	mRender->DeActivate(context);
	mCount->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);

	//�J�E���g�̃o�b�t�@���琶���Ă镪�̃J�E���g���擾
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mParticleCount = particleCount->aliveParticleCount;
	context->Unmap(mCount->GetBuffer(), NULL);


}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/

void GpuParticle::Render(ID3D11DeviceContext* context)
{
	ID3D11Buffer* buffer = mRender->GetBuffer();
	ID3D11Buffer* index = mIndex[mIndexCount]->GetBuffer();
	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	//GPU���Ƀf�[�^�𑗂�
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(mParticleCount, 0,0);

	buffer = nullptr;
	index = nullptr;
	stride = 0;
	//GPU���ɑ������f�[�^�����ɖ߂�
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

}

void GpuParticle::DrowEditorFunction(void* id, std::vector<std::string> shaders, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texs)
{
#ifdef USE_IMGUI
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(0, 275));
	ImGui::Combo("drowType", &mDrowType, vector_getter, static_cast<void*>(&shaders), shaders.size());
	int i = 0;
	int count = 0;
	for (auto& tex : texs)
	{
		if (ImGui::ImageButton(tex.Get(), ImVec2(75, 75)))
		{
			mTexNo = i;
		}
		i++;
		count++;
		if (count == 4)
		{
			count = 0;
			continue;
		}
		ImGui::SameLine();
	}
	ImGui::Text(u8"���I�����Ă�e�N�X�`��");
	if (mTexNo >= 0 && mTexNo < texs.size())ImGui::Image(texs[mTexNo].Get(), ImVec2(112, 112));
	ImGui::EndChild();
#endif

}

///*****************************************************/
////�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
///*****************************************************/
//
//TitleParticle::TitleParticle(ID3D11Device* device) :mSceneChange(false), mStartIdex(0),mIndexNum(0)
//{
//	::memset(&mEditorData, 0, sizeof(EditorData));
//	mMaxParticle = 1000000;
//	HRESULT hr;
//	{
//		//�p�[�e�B�N���̃o�b�t�@
//		std::vector<Particle>particles;
//		particles.resize(mMaxParticle);
//		mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particles, true);
//		//�`��p�o�b�t�@
//		std::vector<RenderParticle>renderParticles;
//		renderParticles.resize(mMaxParticle);
//		mParticleRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderParticles, true);
//		//index�o�b�t�@
//		std::vector<UINT>indices;
//		indices.resize(mMaxParticle);
//		for (int i = 0; i < 2; i++)mParticleIndexs[i] = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, indices, true);
//		//deleteindex�o�b�t�@
//		for (int i = 0; i < mMaxParticle; i++)
//		{
//			indices[i] = i;
//		}
//		mParticleDeleteIndex = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, indices, true);
//		//�J�E���g�o�b�t�@
//		ParticleCount particleCount;
//		memset(&particleCount, 0, sizeof(particleCount));
//		particleCount.deActiveParticleCount = mMaxParticle;
//		mParticleCount = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, particleCount, true, false, D3D11_CPU_ACCESS_READ);
//
//	}
//	//�萔�o�b�t�@����
//	mCbStart = std::make_unique<ConstantBuffer<CbStart>>(device);
//	mCbStart2 = std::make_unique<ConstantBuffer<CbStart2>>(device);
//	mCb = std::make_unique<ConstantBuffer<Cb>>(device);
//	//�`��p�e�N�X�`���̐���
//	wchar_t* names[] =
//	{
//		L"Data/image/��.png",
//		L"",
//		L"Data/image/����1.png",
//		L"Data/image/����2.png",
//		L"Data/image/����3.png",
//		L"Data/image/����4.png",
//		L"Data/image/����5.png",
//		L"Data/image/����6.png",
//		L"Data/image/����7.png",
//		L"Data/image/����8.png",
//	};
//	for (auto& name : names)
//	{
//		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>srv;
//		if (wcscmp(name, L"") == 0)
//		{
//			hr = MakeDummyTexture(device, srv.GetAddressOf());
//		}
//		else
//		{
//			hr = LoadTextureFromFile(device, name, srv.GetAddressOf());
//		}
//		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//		mParticleSRV.push_back(srv);
//	}
//	//�R���s���[�g�V�F�[�_�[�̐���
//	hr = CreateCSFromCso(device, "Data/shader/title_particle_create_cs.cso", mCreateShader.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	hr = CreateCSFromCso(device, "Data/shader/title_particle_cs.cso", mCSShader.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	hr = CreateCSFromCso(device, "Data/shader/title_scene_change_particle_cs.cso", mSceneChangeCSShader.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//	hr = CreateCSFromCso(device, "Data/shader/particle_count_reset.cso", mCSCountShader.GetAddressOf());
//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
//
//	//�`��p�V�F�[�_�[�̐���
//	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
//	{
//		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
//		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
//		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
//		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
//		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
//	};
//	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
//	FileFunction::Load(mEditorData, "Data/file/title_particle_data.bin", "rb");
//}
///*****************************************************/
////�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
///*****************************************************/
//void TitleParticle::Editor()
//{
//#ifdef USE_IMGUI
//	ImGui::Begin("title particle");
//	//�p�����[�^�[�𒲐�����
//	float* angleMovement[3] = { &mEditorData.angleMovement.x,&mEditorData.angleMovement.y,&mEditorData.angleMovement.z };
//	ImGui::SliderFloat3("angleMovement", *angleMovement, -3.14f, 3.14f);
//	ImGui::InputFloat("length", &mEditorData.leng, 1);
//	static float moveSize = 1;
//	ImGui::InputFloat("moveSize", &moveSize, 1);
//	ImGui::InputFloat("startPosition.x", &mEditorData.startPosition.x, moveSize);
//	ImGui::InputFloat("startPosition.y", &mEditorData.startPosition.y, moveSize);
//	ImGui::InputFloat("startPosition.z", &mEditorData.startPosition.z, moveSize);
//	ImGui::InputFloat("life", &mEditorData.life, 1);
//	ImGui::InputFloat("moveLen", &mEditorData.moveLen, 1);
//	float* sphereRatio[3] = { &mEditorData.sphereRatio.x,&mEditorData.sphereRatio.y ,&mEditorData.sphereRatio.z };
//	ImGui::SliderFloat3("sphereRatio", *sphereRatio, 0, 1);
//	float* color[4] = { &mEditorData.color.x,&mEditorData.color.y ,&mEditorData.color.z ,&mEditorData.color.w };
//	ImGui::ColorEdit4("color", *color);
//	ImGui::InputFloat("randSpeed", &mEditorData.randSpeed, 1);
//	ImGui::InputFloat("defSpeed", &mEditorData.defSpeed, 1);
//	ImGui::InputFloat("randMoveLength", &mEditorData.randMoveLength, 0.1f);
//	ImGui::InputFloat("defMoveLength", &mEditorData.defMoveLength, 0.1f);
//	ImGui::InputFloat("randScale", &mEditorData.randScale, 0.1f);
//	ImGui::SliderFloat("randMoveAngle", &mEditorData.randMoveAngle, 3.14f, 3.14f);
//	float* defVelocity[3] = { &mEditorData.defVelocity.x,&mEditorData.defVelocity.y ,&mEditorData.defVelocity.z };
//	ImGui::SliderFloat3("def velocity", *defVelocity, -1, 1);
//	ImGui::Checkbox("scene change", &mSceneChange);
//	//�`��Ɏg���e�N�X�`���̑I��
//	ImVec2 size = ImVec2(75, 75);
//	for (UINT i = 0; i < static_cast<UINT>(mParticleSRV.size()); i++)
//	{
//		if (ImGui::ImageButton(mParticleSRV[i].Get(), size))
//		{
//			mEditorData.textureType = i;
//		}
//		if (i % 4 < 3 && i < static_cast<UINT>(mParticleSRV.size() - 1))ImGui::SameLine();
//	}
//	//���I�����Ă�e�N�X�`���̕\��
//	ImGui::Text(u8"���̃e�N�X�`��");
//	size = ImVec2(150, 150);
//	ImGui::Image(mParticleSRV[mEditorData.textureType].Get(), size);
//	//�Z�[�u
//	if (ImGui::Button("save"))FileFunction::Save(mEditorData, "Data/file/title_particle_data.bin", "wb");
//	ImGui::Text("%f", mNewIndex);
//
//	ImGui::End();
//#endif
//}
///*****************************************************/
////�@�@�@�@�@�@�@�@�@�@�X�V�֐�
///*****************************************************/
//
//void TitleParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
//{
//	//UAV��GPU�ɓn��
//	mParticle->Activate(context, 0, true);
//	mParticleCount->Activate(context, 1, true);
//	mParticleRender->Activate(context, 2, true);
//	mParticleIndexs[mIndexNum]->Activate(context, 3, true);
//	mParticleIndexs[1 - mIndexNum]->Activate(context, 4, true);
//	mParticleDeleteIndex->Activate(context, 5, true);
//	mIndexNum++;
//	if (mIndexNum >= 2)mIndexNum = 0;
//	//�萔�o�b�t�@�̃f�[�^�X�V
//	mCb->data.elapsdTime = elapsdTime;
//	mCb->data.angleMovement = mEditorData.angleMovement;
//	mCbStart->data.color = mEditorData.color;
//	mCbStart->data.startPosition = mEditorData.startPosition;
//	mCbStart->data.leng = mEditorData.leng;
//	mCbStart->data.sphereRatio = mEditorData.sphereRatio;
//	mCbStart->data.life = mEditorData.life;
//	mCbStart->data.moveLen = mEditorData.moveLen;
//	mCbStart->data.randSpeed = mEditorData.randSpeed;
//	mCbStart->data.defSpeed = mEditorData.defSpeed;
//	mCbStart->data.startIndex = mStartIdex;
//	mCbStart2->data.randMoveLength = mEditorData.randMoveLength;
//	mCbStart2->data.defMoveLength = mEditorData.defMoveLength;
//	mCbStart2->data.randMoveAngle = mEditorData.randMoveAngle;
//	mCbStart2->data.randScale = mEditorData.randScale;
//	mCbStart2->data.defVelocity = mEditorData.defVelocity;
//
//	//�萔�o�b�t�@��GPU���ɑ���
//	mCbStart->Activate(context, 0, false, false, false, true);
//	mCbStart2->Activate(context, 1, false, false, false, true);
//	mCb->Activate(context, 2, false, false, false, true);
//	//�V�[���ύX�����O
//	if (!mSceneChange)
//	{
//		//�������̍X�V
//		mNewIndex += 1000 * elapsdTime;
//		float indexSize = mNewIndex - mStartIdex;
//		//��������1�ȏ�Ȃ�
//		if (indexSize >= 1)
//		{
//			context->CSSetShader(mCreateShader.Get(), nullptr, 0);
//			//�p�[�e�B�N���̐���
//			context->Dispatch(static_cast<UINT>(indexSize), 1, 1);
//			mStartIdex = mNewIndex;
//		}
//		//�ő吔��葽�����
//		if (mNewIndex > mMaxParticle)
//		{
//			mNewIndex = 0;
//			mStartIdex = 0;
//		}
//		//�V�F�[�_�[��ݒ�
//		context->CSSetShader(mCSShader.Get(), nullptr, 0);
//	}
//	else
//	{
//		//�V�F�[�_�[��ݒ�
//		context->CSSetShader(mSceneChangeCSShader.Get(), nullptr, 0);
//	}
//	//�p�[�e�B�N���̍X�V
//	context->Dispatch(mMaxParticle/100, 1, 1);
//
//	//GPU���ɑ������f�[�^�����ɖ߂�
//	mCbStart->DeActivate(context);
//	mCbStart2->DeActivate(context);
//	mCb->DeActivate(context);
//	//�p�[�e�B�N�����̍X�V
//	context->CSSetShader(mCSCountShader.Get(), nullptr, 0);
//	context->Dispatch(1, 1, 1);
//
//	//�J�E���g���擾
//	D3D11_MAPPED_SUBRESOURCE ms;
//	context->Map(mParticleCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
//	ParticleCount* particleCount = (ParticleCount*)ms.pData;
//	mRenderCount = particleCount->aliveParticleCount;
//	context->Unmap(mParticleCount->GetBuffer(), NULL);
//	//GPU�̃f�[�^�����
//	context->CSSetShader(nullptr, nullptr, 0);
//	mParticle->DeActivate(context);
//	mParticleCount->DeActivate(context);
//	mParticleRender->DeActivate(context);
//	mParticleIndexs[mIndexNum]->DeActivate(context);
//	mParticleIndexs[1 - mIndexNum]->DeActivate(context);
//	mParticleDeleteIndex->DeActivate(context);
//
//}
///*****************************************************/
////�@�@�@�@�@�@�@�@�@�@�`��֐�
///*****************************************************/
//
//void TitleParticle::Render(ID3D11DeviceContext* context)
//{
//	//�V�F�[�_�[�̐ݒ�
//	mShader->Activate(context);
//	//�`�掞�Ɏg���e�N�X�`���̐ݒ�
//	context->PSSetShaderResources(0, 1, mParticleSRV[mEditorData.textureType].GetAddressOf());
//	//�o�b�t�@�̐ݒ�
//	u_int stride = sizeof(RenderParticle);
//	u_int offset = 0;
//	ID3D11Buffer* buffer = mParticleRender->GetBuffer();
//	ID3D11Buffer* index = mParticleIndexs[mIndexNum]->GetBuffer();
//	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
//	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//
//	context->DrawIndexed(mRenderCount, 0, 0);
//	//GPU����̉��
//	mShader->Deactivate(context);
//	ID3D11ShaderResourceView* srv = nullptr;
//	context->PSSetShaderResources(0, 1, &srv);
//	buffer = nullptr;
//	index = nullptr;
//	stride = 0;
//	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
//	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
//
//}


#endif
