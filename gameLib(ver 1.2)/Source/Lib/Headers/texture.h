#pragma once
#include <d3d11.h>
//�e�N�X�`���̓ǂݍ���
HRESULT LoadTextureFromFile(ID3D11Device* device, const wchar_t* file_name, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc=nullptr,bool mise=true);
//�_�~�[�e�N�X�`���̐���
HRESULT MakeDummyTexture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view);
//�e�N�X�`���̃t�@�C���p�X��g�ݑւ���
void CombineResourcePath(wchar_t(&combined_resource_path)[256], const wchar_t* referrer_filename, const wchar_t* referent_filename);
