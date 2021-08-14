#include "..\Headers\particle_editor.h"
#include"particle_create_list.h"
#ifdef USE_IMGUI
#include<imgui.h>
#include"vector_combo.h"
#include"gpu_particle_manager.h"
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

ParticleEditor::ParticleEditor():mEditorType(0),mDeadFlag(false),mOparationIndex(0)
{
	mEditorTypeList.push_back("");
	mEditorTypeList.push_back("create");
	mEditorTypeList.push_back("operation");
}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/

int ParticleEditor::Editor(bool& create, bool& dead, int& type, int& index, std::vector<std::string>nameList)
{
#ifdef USE_IMGUI
	//GpuParticleManager* particleManager = (GpuParticleManager*)manager;
	ImGui::Begin("Particle Create Editor");
	ImGui::Combo("editor type", &mEditorType, vector_getter, static_cast<void*>(&mEditorTypeList), mEditorTypeList.size());
	ImGuiID id = ImGui::GetID((void*)0);
	switch (mEditorType)
	{
	case EditorType::Create:
		CreateEdit(&id);
		break;
	case EditorType::Operation:
		OperationEdit(&id, nameList);
		break;
	}
	ImGui::End();
	
	create = mData.flag;
	dead = mDeadFlag;
	type = mData.type;
	index = mOparationIndex;
#endif
	return mEditorType;
}
/***************************生成情報の操作関数**************************/
void ParticleEditor::CreateEdit(void* id)
{
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(0, 275));

	ImGui::Combo("particleName", &mData.type, ParticleCreateList::GetCreateList().data(), ParticleCreateList::CreateList::Max);
		mData.flag = ImGui::Button("create particle");
	ImGui::EndChild();

}

/****************************操作するパーティクルを選択・消去する関数*************************/
void ParticleEditor::OperationEdit(void* id, std::vector<std::string> nameList)
{
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(0, 275));
	ImGui::Combo("particleName", &mOparationIndex, vector_getter, static_cast<void*>(&nameList), nameList.size());
	mDeadFlag = ImGui::Button("delete");
	ImGui::EndChild();
}

