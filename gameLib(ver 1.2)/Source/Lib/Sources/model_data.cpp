#include "model_data.h"
#include <Shlwapi.h>
#include <fstream>
#include"fbx_load.h"
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

ModelData::ModelData(const char* filename,  const bool textureFlag ,int filetype)
{
	//���ɃV���A���C�Y�����t�@�C�������邩�ǂ����𒲂ׂ�
	std::string fbxName = filename;
	size_t engineer = fbxName.find_last_of(".") + 1;
	std::string name = fbxName.substr(0, engineer);
	name += "bin";
	if (PathFileExistsA((name).c_str()))
	{//�������炻�̃t�@�C������f�[�^���擾
		std::ifstream ifs;

		ifs.open((name).c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);

	}
	else
	{//�Ȃ�������FbxLoad�N���X���g����fbx�t�@�C������f�[�^���擾
		FbxLoader load;
		load.Load(filename, *this, textureFlag, filetype);
		//�f�[�^���V���A���C�Y���ăZ�[�u����
		std::ofstream ofs;
		ofs.open((name).c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

}
