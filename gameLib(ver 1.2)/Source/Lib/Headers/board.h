#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include"drow_shader.h"
#include"constant_buffer.h"
#include<memory>
//�e�N�X�`���𒣂��|���N���X
class Board
{
public:
	//�R���X�g���N�^
	Board(ID3D11Device* device, const wchar_t* fileName);
	//UV�A�j���[�V����
	void Anim(ID3D11DeviceContext* context, const VECTOR2F&texturePosition, const VECTOR2F&textureSize);
	//�`��
	void Render(ID3D11DeviceContext* context, const VECTOR3F& position, const VECTOR3F& scale, const VECTOR3F& angle, const FLOAT4X4& view, const FLOAT4X4& projection,const VECTOR4F&color=VECTOR4F(1,1,1,1));
private:
	//���_�o�b�t�@
	struct Vertex
	{
		VECTOR3F position;
		VECTOR2F texcoord;
		VECTOR4F color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVertexBuffer;
	//�萔�o�b�t�@
	struct CbScene
	{
		FLOAT4X4 worldViewProjection;
		VECTOR4F color;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>mCbBuffer;
	//�e�N�X�`���f�[�^
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mShaderResourceView;
	D3D11_TEXTURE2D_DESC texture2d;
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mShader;

};