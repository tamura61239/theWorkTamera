#pragma once
#include <d3d11.h>
#include <wrl.h>

//�`��p�̃V�F�[�_�[�N���X
class DrowShader
{
public:
	//�R���X�g���N�^
	DrowShader(ID3D11Device* device, const char* vsName, const char* gsName, const char* psName, D3D11_INPUT_ELEMENT_DESC* input_element_desc = nullptr, UINT num_elements = 0);
	//�V�F�[�_�[���Z�b�g����
	void Activate(ID3D11DeviceContext* context);
	//�Z�b�g�����V�F�[�_�[����������
	void Deactivate(ID3D11DeviceContext* context);
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
};