#include "..\Headers\particle_base.h"
#include"particle_move_list.h"
#include"particle_create_list.h"
#include"particle_create_point.h"
#include"shader.h"
#ifdef USE_IMGUI
#include<imgui.h>
#include"vector_combo.h"
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/

ParticleBase::ParticleBase(ID3D11Device* device, int createType, std::string name)
{
	mMaxParticle = 1000;
	CreateBuffer(device);
	mName = name;
	mCreate = std::make_unique<ParticleCreatePoint>(device);
	mMoves.emplace_back(ParticleMoveList::CreateParticleMove(0));

	HRESULT hr = S_OK;
	//シェーダーの生成
	hr = CreateCSFromCso(device, "Data/shader/point_create_particle.cso", mCresteShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_last_move.cso", mLastMoveShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = CreateCSFromCso(device, "Data/shader/particle_count_reset.cso", mCountResetShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
/**************************バッファ生成関数***************************/

void ParticleBase::CreateBuffer(ID3D11Device* device)
{
	//パーティクルバッファの生成
	std::vector<ParticleMoveParameter>particleData;
	particleData.resize(mMaxParticle);
	mParticle = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, particleData, true);
	//描画用データの生成
	std::vector<ParticleRenderParameter>renderData;
	renderData.resize(mMaxParticle);
	mRender = std::make_unique<CSBuffer>(device, D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, renderData, true);
	//Indexデータの生成
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

}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

void ParticleBase::Editor(std::vector<std::string> shaders, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texs)
{
#ifdef USE_IMGUI
	ImGui::Begin(mName.c_str());
	ImGuiID id = ImGui::GetID((void*)0);
	mCreate->Editor((void*)&id);
	id = ImGui::GetID((void*)1);
	DrawEditorFunction((void*)&id, shaders, texs);
	id = ImGui::GetID((void*)2);
	ParticleMoveEdit((void*)&id);
	ImGui::End();
#endif
}
void ParticleBase::DrawEditorFunction(void* id, std::vector<std::string> shaders, std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texs)
{
	//描画するシェーダーとテクスチャの種類を設定
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
	ImGui::Text(u8"今選択してるテクスチャ");
	if (mTexNo >= 0 && mTexNo < texs.size())ImGui::Image(texs[mTexNo].Get(), ImVec2(112, 112));
	ImGui::EndChild();
#endif

}

void ParticleBase::ParticleMoveEdit(void* id)
{
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(0, 275));
	if (ImGui::CollapsingHeader("create particle move"))
	{
		//particlemoveクラスを生成
		ImGui::Combo("new move", &mMoveCreateTypeNo, ParticleMoveList::GetMoveList().data(), ParticleMoveList::MoveList::Max);
		if (ImGui::Button("create move"))
		{
			mMoves.emplace_back(ParticleMoveList::CreateParticleMove(mMoveCreateTypeNo));
		}
	}
	if (ImGui::CollapsingHeader("edit particle move"))
	{
		//生成されているpartclemoveクラスの操作
		for (int i = 0; i < mMoves.size(); i++)
		{
			auto& move = mMoves[i];
			if (ImGui::CollapsingHeader(("move" + std::to_string(i)).c_str()))
			{
				move->Editor(id);
			}
			if (ImGui::Button("delete"))
			{
				mMoves.erase(mMoves.begin() + i);
				i--;
			}
		}
	}
	ImGui::EndChild();

}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/

void ParticleBase::Update(ID3D11DeviceContext* context, float elapsdTime)
{
	return;

	//コンピュートシェーダーにパラメーターを送信する
	mParticle->Activate(context, 0, true);
	mCount->Activate(context, 1, true);
	mRender->Activate(context, 2, true);
	mIndex[mIndexCount]->Activate(context, 3, true);
	mIndex[1 - mIndexCount]->Activate(context, 4, true);
	mDeathIndex->Activate(context, 5, true);
	mIndexCount++;
	if (mIndexCount >= 2)
	{
		mIndexCount = 0;
	}
	//生成
	mCreate->CreateParticle(context, elapsdTime);
	//更新
	for (auto& move : mMoves)
	{
		move->Move(context, mMaxParticle / 100);
	}
	context->CSSetShader(mLastMoveShader.Get(), nullptr, 0);
	context->Dispatch(mMaxParticle / 100, 1, 1);

	//カウントを更新
	context->CSSetShader(mCountResetShader.Get(), nullptr, 0);
	context->Dispatch(1, 1, 1);
	//後処理
	mParticle->DeActivate(context);
	mIndex[mIndexCount]->DeActivate(context);
	mIndex[1 - mIndexCount]->DeActivate(context);
	mDeathIndex->DeActivate(context);
	mRender->DeActivate(context);
	mCount->DeActivate(context);
	context->CSSetShader(nullptr, nullptr, 0);
	//カウントのバッファから生きてる分のカウントを取得
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(mCount->GetBuffer(), NULL, D3D11_MAP_READ, NULL, &ms);
	ParticleCount* particleCount = (ParticleCount*)ms.pData;
	mRenderCount = particleCount->aliveParticleCount;
	context->Unmap(mCount->GetBuffer(), NULL);

}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void ParticleBase::Draw(ID3D11DeviceContext* context)
{
	return;
	ID3D11Buffer* buffer = mRender->GetBuffer();
	ID3D11Buffer* index = mIndex[mIndexCount]->GetBuffer();
	u_int stride = sizeof(ParticleRenderParameter);
	u_int offset = 0;
	//GPU側にデータを送る
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->DrawIndexed(mRenderCount, 0, 0);

	buffer = nullptr;
	index = nullptr;
	stride = 0;
	//GPU側に送ったデータを元に戻す
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetIndexBuffer(index, DXGI_FORMAT_R32_UINT, 0);

}


