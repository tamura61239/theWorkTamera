#pragma once
#include <string>
#include <vector>
#include"vector.h"
#include"serialize_function.h"
//���f���̃f�[�^�N���X	
struct ModelData
{
	//�{�[��(��)�̃f�[�^
	struct Node
	{
		std::string			name;
		int					parentIndex;
		VECTOR3F	scale;
		VECTOR4F	rotate;
		VECTOR3F	translate;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				name,
				parentIndex,
				scale,
				rotate,
				translate
			);
		}
	};
	//���_�f�[�^
	struct Vertex
	{
		VECTOR3F	position;
		VECTOR3F	normal;
		VECTOR2F	texcoord;
		VECTOR4F	boneWeight;
		UVECTOR4	boneIndex;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				position,
				normal,
				texcoord,
				boneWeight,
				boneIndex
			);
		}

	};
	//�}�e���A�����ƂɃ��b�V������؂�
	struct Subset
	{
		int					materialIndex;
		int					startIndex;
		int					indexCount;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				materialIndex,
				startIndex,
				indexCount
			);
		}

	};
	//���b�V���f�[�^
	struct Mesh
	{
		std::vector<Vertex>	vertices;
		std::vector<int>	indices;
		std::vector<Subset>	subsets;

		int					nodeIndex;

		std::vector<int>					nodeIndices;
		std::vector<FLOAT4X4>	inverseTransforms;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				vertices,
				indices,
				subsets,
				nodeIndex,
				nodeIndices,
				inverseTransforms
			);
		}

	};
	//�}�e���A���f�[�^
	struct Material
	{
		VECTOR4F	color;
		std::string			textureFilename;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				color.x, color.y, color.z, color.w,
				textureFilename
			);
		}

	};
	//�L�[�t���[���̃m�[�h�f�[�^
	struct NodeKeyData
	{
		VECTOR3F	scale;
		VECTOR4F	rotate;
		VECTOR3F	translate;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				scale,
				rotate,
				translate
			);
		}

	};
	//�L�[�t���[��
	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				seconds,
				nodeKeys
			);
		}

	};
	//�A�j���[�V����
	struct Animation
	{
		float						secondsLength;
		std::vector<Keyframe>		keyframes;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				secondsLength,
				keyframes
			);
		}

	};

	std::vector<Node>		nodes;

	std::vector<Mesh>		meshes;
	std::vector<Material>	diffuses;
	std::vector<Material>	normals;
	std::vector<Material>	bumps;

	std::vector<Animation>	animations;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			nodes,
			meshes,
			diffuses,
			normals,
			bumps,
			animations
		);
	}
	//�R���X�g���N�^
	ModelData(const char* filename,const bool textureFlag=false, int filetype = 0);
};