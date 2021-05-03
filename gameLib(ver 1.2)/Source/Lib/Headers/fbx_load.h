#pragma once
#include <fbxsdk.h> 
#include "model_data.h"

//fbx�t�@�C������f�[�^��ǂݍ��ރN���X
class FbxLoader
{
public:
	// FBX�t�@�C�����烂�f���f�[�^�ǂݍ���
	bool Load(const char* filename, ModelData& data,const bool textureFlag ,int filetype = 0);

private:
	// ���f���f�[�^���\�z
	bool BuildModel(const char* dirname, FbxScene* fbx_scene, ModelData& data);

	// �m�[�h�f�[�^���\�z
	void BuildNodes(FbxNode* fbx_node, ModelData& data, int parent_node_index);
	void BuildNode(FbxNode* fbx_node, ModelData& data, int parent_node_index);

	// ���b�V���f�[�^���\�z
	void BuildMeshes(FbxNode* fbx_node, ModelData& data);
	void BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh, ModelData& data);

	// �}�e���A���f�[�^���\�z
	void BuildMaterials(const char* dirname, FbxScene* fbx_scene, ModelData& data);
	ModelData::Material BuildMaterial(const char* dirname, FbxSurfaceMaterial* fbx_surface_material, const char* property_name, const char* factor_name);

	// �A�j���[�V�����f�[�^���\�z
	void BuildAnimations(FbxScene* fbx_scene, ModelData& data);

	// �C���f�b�N�X�̌���
	int FindNodeIndex(ModelData& data, const char* name);
	int FindMaterialIndex(FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material);
	int mFileType;
	bool mTextureFlag;
};
