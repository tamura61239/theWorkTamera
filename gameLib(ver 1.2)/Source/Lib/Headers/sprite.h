#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include"drow_shader.h"
#include<memory>
//2D�̃e�N�X�`���`��N���X
class Sprite
{
public:
	//�R���X�g���N�^
	Sprite(ID3D11Device* device, const wchar_t* fileName);
	Sprite(ID3D11Device* device);
	//�摜
	void Load(ID3D11Device* device,const wchar_t* fileName);
	//�`��
	void Render(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size,
		const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F&color = VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	void Render(ID3D11DeviceContext* context,ID3D11ShaderResourceView*srv, const VECTOR2F& position, const VECTOR2F& size,
		const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color = VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	void Render(ID3D11DeviceContext* context, DrowShader* shader, const VECTOR2F& position, const VECTOR2F& size,
		const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color = VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	void Render(ID3D11DeviceContext* context, DrowShader* shader, ID3D11ShaderResourceView* srv, const VECTOR2F& position, const VECTOR2F& size,
		const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color = VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));

private:
	void BufferUpdate(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size,
		const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color, D3D11_TEXTURE2D_DESC texture2dDesc);
	//�V�F�[�_�[
	std::unique_ptr<DrowShader>mShader;
	//�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVSBuffer;
	//�e�N�X�`���f�[�^
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mTexview;
	D3D11_TEXTURE2D_DESC mTexture2d;
	//���_�f�[�^
	struct Vertex
	{
		VECTOR3F position;
		VECTOR4F color;
		VECTOR2F texcoord;
	};

};
