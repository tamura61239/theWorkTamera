#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>

class Sprite
{
public:
	//�R���X�g���N�^
	Sprite(ID3D11Device* device, const wchar_t* fileName);
	//�`��
	void Render(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size,
		const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F&color = VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f));
	//~Sprite()
	//{
	//	delete mVSBuffer;
	//}
private:
	//********************************//
    //             �ϐ�               //
    //********************************//
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVSBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mTexview;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	D3D11_TEXTURE2D_DESC texture2d;
	//
	struct Vertex
	{
		VECTOR3F position;
		VECTOR4F color;
		VECTOR2F texcoord;
	};

};
