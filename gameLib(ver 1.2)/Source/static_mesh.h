#pragma once
#include <d3d11.h>
#include <wrl.h>
#include"vector.h"
#include<vector>
#include<fbxsdk.h>
#include"serialize_function.h"
#include"shader_type.h"

struct TextureMapData
{
	wchar_t* deffuseName;
	wchar_t* normalName;
	wchar_t* bumpName;
};
class StaticMesh
{
public:
	//�R���X�g���N�^
	StaticMesh(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType= SHADER_TYPE::USEALLY,bool pathOrganize=false,int organizeType=0);
	virtual~StaticMesh() {}
	int RayPick(
		const VECTOR3F& startPosition,//���C���΂��J�n���W
		const VECTOR3F& endPosition,//���C���΂��I�����W
		VECTOR3F* outPosition,//���C�������������W
		VECTOR3F* outNormal,//���C�����������ʂ̖@��
		float* outLength//���C�����������ʂ܂ł̋���
	);
	Microsoft::WRL::ComPtr<ID3D11VertexShader>GetVSShader() { return mVSShader; }
	Microsoft::WRL::ComPtr<ID3D11PixelShader>GetPSShader() { return mPSShader; }
	Microsoft::WRL::ComPtr<ID3D11InputLayout>GetInputLayout() { return mInput; }
	void ChangeShader(ID3D11Device* device, SHADER_TYPE shaderType, std::vector<TextureMapData>data);
public:
	//�V���A���C�Y
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
	struct Material
	{
		VECTOR4F color = { 1.0f,1.0f,1.0f,1.0f };
		std::wstring textureName;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mShaderResourceView;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				color,
				textureName
			);
		}
	};
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
	std::vector<Mesh>meshes;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			meshes
		);
	}
public:
	void SetShaderResouceView(ID3D11DeviceContext* context, Subset& subset);
private:
	void CreateMesh(ID3D11Device* device, const char* fileName, bool pathOrganize, int organizeType);
	void FbxTettureNameLoad(const char* property_name,const char* factor_name, Material& material, const FbxSurfaceMaterial* surfaceMaterial, const char* fileName, bool pathOrganize, int organizeType);
	void CreateBuffers(ID3D11Device* device);
	void CreateShaderResourceView(ID3D11Device* device, SHADER_TYPE shaderType);
	void CreateShader(ID3D11Device* device, SHADER_TYPE shaderType);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	SHADER_TYPE mShaderType;
};

class MeshRender
{
public:
	MeshRender(ID3D11Device* device);
	void ShadowBegin(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection);
	void ShadowRender(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world);
	void ShadowEnd(ID3D11DeviceContext* context);
	void Begin(ID3D11DeviceContext* context, const VECTOR4F& light,const FLOAT4X4&view,const FLOAT4X4&projection);
	void Render(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world,const VECTOR4F color=VECTOR4F(1,1,1,1));
	void End(ID3D11DeviceContext* context);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbScene;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbObj;


	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>mDiffuseSamplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mNormalSamplerState;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>mShadowVSShader;
	struct CbScene
	{
		VECTOR4F lightColor;
		VECTOR4F lightDirection;
		VECTOR4F ambientColor;
		VECTOR4F eyePosition;
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	struct CbObj
	{
		VECTOR4F color;
		FLOAT4X4 world;
	};

};