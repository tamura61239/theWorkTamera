#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include<vector>
#include"constant_buffer.h"
#include"drow_shader.h"
#include<memory>

//�f�t�H���g�I�u�W�F�N�g�N���X
class GeometricPrimitive
{
public:
	//�R���X�g���N�^
	GeometricPrimitive(ID3D11Device* device) :mIndexNum(0) {};
	//�f�X�g���N�^
	virtual ~GeometricPrimitive() {};
	//getter
	Microsoft::WRL::ComPtr<ID3D11Buffer>GetVertexBuffer() { return mVertexBuffer; }
	Microsoft::WRL::ComPtr<ID3D11Buffer>GetIndexBuffer() { return mIndexBuffer; }

	const int& GetIndexNum() { return mIndexNum; }
	//���_�\����
	struct Vertex
	{
		VECTOR3F position;
		VECTOR3F normal;
	};
protected:
	//�o�b�t�@����
	void CreateBuffer(ID3D11Device* device, std::vector<Vertex>vertics, std::vector<UINT>index);
private:
	//index��
	int mIndexNum;
	//���_�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVertexBuffer;
	//index�o�b�t�@
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
	GeometricSphere(ID3D11Device* device, u_int slices, u_int stacks);//slices:32,stacks:16
};

//�`��N���X
class PrimitiveRender
{
public:
	//�R���X�g���N�^
	PrimitiveRender(ID3D11Device* device);
	//�`��
	void Begin(ID3D11DeviceContext* context, const VECTOR4F& light, const FLOAT4X4& view, const FLOAT4X4& projection);
	void Render(ID3D11DeviceContext* context, GeometricPrimitive* obj, const FLOAT4X4& world, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void Render(ID3D11DeviceContext* context, DrowShader* shader, GeometricPrimitive* obj, const FLOAT4X4& world, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void End(ID3D11DeviceContext* context);
private:
	//�萔�o�b�t�@
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
	std::unique_ptr<ConstantBuffer<CbScene>>mCbScene;
	std::unique_ptr<ConstantBuffer<CbObj>>mCbObj;
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mShader;
};
