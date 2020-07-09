#include "sprite.h"
#include "misc.h"
#include<memory>
#include<WICTextureLoader.h>


Sprite::Sprite(ID3D11Device* device, const wchar_t* fileName)
{
	HRESULT hr;
	Vertex vertices[] =
	{
		{VECTOR3F(-0.5f,0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(0.0f,0.0f)},
		{VECTOR3F(0.5f,0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(1.0f,0.0f)},
		{VECTOR3F(-0.5f,-0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(0.0f,1.0f)},
		{VECTOR3F(0.5f,-0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(1.0f,1.0f)},
	};

	//���_�o�b�t�@�쐬
	{
		D3D11_BUFFER_DESC desc = {};
		//ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&desc, &data, mVSBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//���_�V�F�[�_�[�����_���̓��C�A�E�g�̐���
	{
		// ���_�V�F�[�_�[�t�@�C�����J��(sprite_vs.hlsl ���R���p�C�����Ăł����t�@�C��)
		FILE* fp = nullptr;
		fopen_s(&fp, "Data/shader/sprite_vs.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// ���_�V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// ���_�V�F�[�_�[�f�[�^����ɒ��_�V�F�[�_�[�I�u�W�F�N�g�𐶐�����
		hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, mVSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// GPU�ɒ��_�f�[�^�̓��e�������Ă����邽�߂̐ݒ�
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
		    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

		};

		// ���̓��C�A�E�g�̐���
		hr = device->CreateInputLayout(
			input_element_desc,				// ���_�f�[�^�̓��e
			ARRAYSIZE(input_element_desc),	// ���_�f�[�^�̗v�f��
			cso_data.get(),						// ���_�V�F�[�_�[�f�[�^�iinput_element_desc�̓��e�� sprite_vs.hlsl�̓��e�ɕs��v���Ȃ����`�F�b�N���邽�߁j
			cso_sz,							// ���_�V�F�[�_�[�f�[�^�T�C�Y
			mInput.GetAddressOf()					// ���̓��C�A�E�g�I�u�W�F�N�g�̃|�C���^�̊i�[��B
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// �s�N�Z���V�F�[�_�[�̐���
	{
		// �s�N�Z���V�F�[�_�[�t�@�C�����J��(sprite_ps.hlsl ���R���p�C�����Ăł����t�@�C��)
		FILE* fp = nullptr;
		fopen_s(&fp, "Data/shader/sprite_ps.cso", "rb");
		_ASSERT_EXPR_A(fp, "CSO File not found");

		// �s�N�Z���V�F�[�_�[�t�@�C���̃T�C�Y�����߂�
		fseek(fp, 0, SEEK_END);
		long cso_sz = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// ��������ɒ��_�V�F�[�_�[�f�[�^���i�[����̈��p�ӂ���
		std::unique_ptr<u_char[]> cso_data = std::make_unique<u_char[]>(cso_sz);
		fread(cso_data.get(), cso_sz, 1, fp);
		fclose(fp);

		// �s�N�Z���V�F�[�_�[�̐���
		hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, mPSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//RasterizerState�̐���
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//ShaderResourceView�̐���
	{
		//Microsoft::WRL::ComPtr<ID3D11Resource> resource;
		//hr = DirectX::CreateWICTextureFromFile(device, fileName, resource.GetAddressOf(), mTexview.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//ID3D11Texture2D* tex2d;
		//hr = resource->QueryInterface(&tex2d);
		//tex2d->GetDesc(&texture2d);
		Microsoft::WRL::ComPtr<ID3D11Resource>resource;
		ID3D11ShaderResourceView** shader_resource_view = mTexview.GetAddressOf();
		hr = DirectX::CreateWICTextureFromFile(device, fileName, resource.GetAddressOf(), mTexview.GetAddressOf());
		ID3D11Texture2D* tex2d;
		hr = resource.Get()->QueryInterface(&tex2d);
		tex2d->GetDesc(&texture2d);
	}

	//SamplerState�̐���
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//DepthStencilState�̐���
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void Sprite::Render(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	HRESULT hr;
	//D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	Vertex vertices[] =
	{
		{VECTOR3F(position.x,position.y,0),color,VECTOR2F(texPosition.x,texPosition.y)},
		{VECTOR3F(position.x + size.x,position.y,0),color,VECTOR2F(texPosition.x + texSize.x,texPosition.y)},
		{VECTOR3F(position.x,position.y + size.y,0),color,VECTOR2F(texPosition.x,texPosition.y + texSize.y)},
		{VECTOR3F(position.x + size.x,position.y + size.y,0),color,VECTOR2F(texPosition.x + texSize.x,texPosition.y + texSize.y)},
	};
	float texX = position.x + size.x / 2.0f;
	float texY = position.y + size.y / 2.0f;
	//	��]���̕ύX(�Z�`������)
	for (int i = 0; i < 4; i++) {
		vertices[i].position.x -= texX;
		vertices[i].position.y -= texY;
	}
	//    ��]
	float degree = DirectX::XMConvertToRadians(angle);
	float si = sinf(degree);
	float co = cosf(degree);

	for (int i = 0; i < 4; i++) {
		float pos_x = vertices[i].position.x;
		float pos_y = vertices[i].position.y;
		vertices[i].position.x = pos_x * co - pos_y * si;
		vertices[i].position.y = pos_x * si + pos_y * co;
	}
	//��]���̕ύX(���̈ʒu��)
	for (int i = 0; i < 4; i++) {
		vertices[i].position.x += texX;
		vertices[i].position.y += texY;
	}

	//���_�o�b�t�@�̍X�V
	D3D11_MAPPED_SUBRESOURCE mapped;
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);

	hr = context->Map(mVSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	Vertex* v = (Vertex*)mapped.pData;

	for (int i = 0; i < 4; i++) {
		v[i].position.x = (vertices[i].position.x / viewport.Width) * 2.0f - 1.0f;
		v[i].position.y = -(vertices[i].position.y / viewport.Height) * 2.0f + 1.0f;
		v[i].color = vertices[i].color;
		v[i].texcoord.x = vertices[i].texcoord.x / texture2d.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / texture2d.Height;
	}

	context->Unmap(mVSBuffer.Get(), 0);


	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(mInput.Get());
	context->RSSetState(mRasterizerState.Get());
	context->VSSetShader(mVSShader.Get(), NULL, 0);
	context->PSSetShader(mPSShader.Get(), NULL, 0);
	context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	context->Draw(4, 0);

}
