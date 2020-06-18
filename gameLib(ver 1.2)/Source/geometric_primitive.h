#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include<vector>

#define TYPE 0

//�I�u�W�F�N�g�N���X
class GeometricPrimitive
{
public:
	GeometricPrimitive(ID3D11Device* device):indexNum(0) {};
	virtual ~GeometricPrimitive() {};
	Microsoft::WRL::ComPtr<ID3D11Buffer>GetVertexBuffer() { return mVertexBuffer; }
	Microsoft::WRL::ComPtr<ID3D11Buffer>GetIndexBuffer() { return mIndexBuffer; }
	
	const int& GetIndexNum() { return indexNum; }
#if TYPE
	const FLOAT4X4& GetWorld() { return world; }
	void CalculateTransform(const VECTOR3F& position, const VECTOR3F& scale, const VECTOR3F& angle)
	{
		DirectX::XMMATRIX W, s, r, t;
		//�X�P�[���s��
		s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		//��]�s��
		r = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
		//�ړ��s��
		t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		//���[���h�s��
		W = s * r * t;

		DirectX::XMStoreFloat4x4(&world, W);
	}
#else
#endif
	//���_�\����
	struct Vertex
	{
		VECTOR3F position;
		VECTOR3F normal;
	};
protected:
	void CreateBuffer(ID3D11Device* device, std::vector<Vertex>vertics,std::vector<UINT>index);
private:
	FLOAT4X4 world;
	int indexNum;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mIndexBuffer;
};
//�L���[�u
class GeometricCube :public GeometricPrimitive
{
public:
	GeometricCube(ID3D11Device* device);
};
//�~��
class GeometricCylinder :public GeometricPrimitive
{
public:
	GeometricCylinder(ID3D11Device* device, u_int slices);
};
//��
class GeometricSphere :public GeometricPrimitive
{
public:
	GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks);
};

//�`��N���X
class PrimitiveRender
{
public:
	PrimitiveRender(ID3D11Device* device);
#if TYPE
	void Begin(ID3D11DeviceContext* context, const VECTOR4F& light);
	void Render(ID3D11DeviceContext* context, GeometricPrimitive* obj,const FLOAT4X4& view, const FLOAT4X4& projection ,const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
#else
	void Begin(ID3D11DeviceContext* context, const VECTOR4F& light, const FLOAT4X4& view, const FLOAT4X4& projection);
	void Render(ID3D11DeviceContext* context, GeometricPrimitive* obj, const FLOAT4X4& world, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
#endif
	void End(ID3D11DeviceContext* context);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbSceneBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbObjBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
#if TYPE
	struct CbScene
	{
		VECTOR4F lightDirection;
	};
	struct CbObj
	{
		FLOAT4X4 worldViewProjection;
		FLOAT4X4 world;
		VECTOR4F color;
	};
#else
	struct CbScene
	{
		VECTOR4F lightDirection;
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	struct CbObj
	{
		FLOAT4X4 world;
		VECTOR4F color;
	};
#endif
};
