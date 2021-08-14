#pragma once
#include<string>
#include<vector>
class ParticleEditor
{
public:
	ParticleEditor();
	int Editor(bool&create,bool&dead,int&type,int&index,std::vector<std::string>nameList);
	enum EditorType
	{
		Normal,Create,Operation
	};

private:
	struct CreateData
	{
		std::string name;
		int type;
		bool flag;
	};
	void CreateEdit(void*id);
	void OperationEdit(void* id,std::vector<std::string> nameList);
	//void CreateParticleEditor(bool* create, CreateData* data);
	//void OperationParticleEditor(std::vector<std::string> nameList,bool* dead, int* operationNo);
	int mEditorType;
	std::vector<std::string>mEditorTypeList;
	CreateData mData;
	bool mDeadFlag;
	int mOparationIndex;
};