#pragma once
#include <d3d11.h>
#include <wrl.h>
#include"vector.h"
#include<vector>
#include<fbxsdk.h>
#include"serialize_function.h"
#include"shader_type.h"
#include"drow_shader.h"
#include"constant_buffer.h"

struct TextureMapData
{
	wchar_t* deffuseName;
	wchar_t* normalName;
	wchar_t* bumpName;
};
//���ʂ̃��f���N���X
class StaticMesh
{
public:
	//�R���X�g���N�^
	StaticMesh(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType = SHADER_TYPE::USEALLY, bool pathOrganize = false, int organizeType = 0);
	//�f�X�g���N�^
	virtual~StaticMesh() {}
	//���C�s�b�N
	int RayPick(
		const VECTOR3F& startPosition,//���C���΂��J�n���W
		const VECTOR3F& endPosition,//���C���΂��I�����W
		VECTOR3F* outPosition,//���C�������������W
		VECTOR3F* outNormal,//���C�����������ʂ̖@��
		float* outLength//���C�����������ʂ܂ł̋���
	);
	//getter
	const VECTOR3F& GetMaxPosition() { return mMaxPosition; }
	const VECTOR3F& GetMinPosition() { return mMinPosition; }
	SHADER_TYPE GetShaderType() { return mShaderType; }
	//�e�N�X�`����ݒ肷��
	void ChangeShaderResourceView(ID3D11Device* device, SHADER_TYPE shaderType, std::vector<TextureMapData>data);
public:
	//���_�f�[�^
	struct Vertex
	{
		VECTOR3F position;
		VECTOR3F normal;
		VECTOR2F texcoord;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				position,
				normal,
				texcoord
			);
		}
	};
	//�}�e���A��
	struct Material
	{
		VECTOR4F color = { 1.0f,1.0f,1.0f,1.0f };
		std::wstring textureName;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>SRV;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				color,
				textureName
			);
		}
	};
	//�}�e���A���P�ʂŃ��b�V������؂�
	struct Subset
	{
		u_int indexStart = 0;	// start number of index buffer
		u_int indexCount = 0;	// number of vertices (indices)
		Material diffuse;
		Material normal;
		Material bump;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				indexStart,
				indexCount,
				diffuse,
				normal,
				bump
			);
		}
	};
	//
	struct Face
	{
		VECTOR3F position[3];
		int materialIndex;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				position[0],
				position[1],
				position[2],
				materialIndex
			);
		}
	};
	//���b�V��
	struct Mesh
	{
		std::vector<Vertex> vertices;
		std::vector<u_int> indices;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		std::vector<Subset> subsets;
		std::vector<Face>faces;
		FLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				vertices,
				indices,
				subsets, faces,
				globalTransform
			);
		}
	};
	//���b�V���f�[�^
	std::vector<Mesh>mMeshes;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			mMeshes
		);
	}
public:
	//�e�N�X�`���𐶐�
	void SetShaderResouceView(ID3D11DeviceContext* context, Subset& subset);
private:
	//���_�̍ŏ��n�ƍő�l��ݒ肷��
	void SetMinAndMaxPosition();
	//���b�V���𐶐�
	void CreateMesh(ID3D11Device* device, const char* fileName, bool pathOrganize, int organizeType);
	//�e�N�X�`���f�[�^��ǂݍ���
	void FbxTettureNameLoad(const char* property_name, const char* factor_name, Material& material, const FbxSurfaceMaterial* surfaceMaterial, const char* fileName, bool pathOrganize, int organizeType);
	//�o�b�t�@�𐶐�
	void CreateBuffers(ID3D11Device* device);
	//SRV�𐶐�
	void CreateShaderResourceView(ID3D11Device* device, SHADER_TYPE shaderType);
private:
	//�V�F�[�_�[�̎��
	SHADER_TYPE mShaderType;
	//���_�̍ŏ��l�ƍő�l
	VECTOR3F mMinPosition;
	VECTOR3F mMaxPosition;
};
//���f����`�悷��N���X
class MeshRender
{
public:
	//�R���X�g���N�^
	MeshRender(ID3D11Device* device);
	//�V���h�E�}�b�v�̕`��
	void ShadowBegin(ID3D11DeviceContext* context);
	void ShadowRender(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world, const VECTOR4F&color=VECTOR4F(1,1,1,1));
	void ShadowEnd(ID3D11DeviceContext* context);
	//�`��
	void Begin(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const bool w = false);
	void Render(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world, const VECTOR4F color = VECTOR4F(1, 1, 1, 1));
	void Render(ID3D11DeviceContext* context, DrowShader* shader, StaticMesh* obj, const FLOAT4X4& world, const VECTOR4F color = VECTOR4F(1, 1, 1, 1));
	void End(ID3D11DeviceContext* context);
	//���x�}�b�v�̕`��
	void VelocityBegin(ID3D11DeviceContext* context, const bool w = false);
	void VelocityRender(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world, const FLOAT4X4& beforeWorld, const VECTOR4F color = VECTOR4F(1, 1, 1, 1));
	void VelocityEnd(ID3D11DeviceContext* context);

private:
	//�萔�o�b�t�@
	struct CbScene
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	struct CbObj
	{
		VECTOR4F color;
		FLOAT4X4 world;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>mCbScene;
	std::unique_ptr<ConstantBuffer<CbObj>>mCbObj;
	std::unique_ptr<ConstantBuffer<FLOAT4X4>>mCbBeforeObj;
	//�V�F�[�_�[
	std::vector<std::unique_ptr<DrowShader>>mShader;
	std::unique_ptr<DrowShader>mShadowShader;

};