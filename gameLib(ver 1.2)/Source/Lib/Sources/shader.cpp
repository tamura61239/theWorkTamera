#include "shader.h"
#include "misc.h"

#include<memory>

#include<map>
#include <wrl.h>
#include<string>
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�����֐�
/*****************************************************/

/*************************VS�V�F�[�_�[�𐶐�����****************************/

HRESULT CreateVSFromCso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader, ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
	//�t�@�C������V�F�[�_�[�𐶐�����

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	if (num_elements <= 0)return hr;
	hr = device->CreateInputLayout(input_element_desc, num_elements, cso_data.get(), cso_sz, input_layout);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));



	return hr;
}
/*************************PS�V�F�[�_�[�𐶐�����****************************/

HRESULT CreatePSFromCso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	//std::map�Ƀf�[�^��ۑ�����
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11PixelShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{//�f�[�^��ۑ����Ă���擾����
		*pixel_shader = it->second.Get();
		(*pixel_shader)->AddRef();
		return S_OK;
	}
	//�t�@�C������V�F�[�_�[�𐶐�����

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//�ۑ�����

	cache.insert(std::make_pair(cso_name, *pixel_shader));

	return hr;
}
/************************GP�V�F�[�_�[�𐶐�����*****************************/

HRESULT CreateGSFromCso(ID3D11Device* device, const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	//std::map�Ƀf�[�^��ۑ�����
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11GeometryShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{//�f�[�^��ۑ����Ă���擾����
		*geometry_shader = it->second.Get();
		(*geometry_shader)->AddRef();
		return S_OK;
	}
	//�t�@�C������V�F�[�_�[�𐶐�����

	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//�ۑ�����

	cache.insert(std::make_pair(cso_name, *geometry_shader));

	return hr;
}
/**************************CS�V�F�[�_�[�𐶐�����***************************/

HRESULT CreateCSFromCso(ID3D11Device* device, const char* cso_name, ID3D11ComputeShader** compute_shader)
{
	//std::map�Ƀf�[�^��ۑ�����
	static std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache;
	auto it = cache.find(cso_name);
	if (it != cache.end())
	{//�f�[�^��ۑ����Ă���擾����
		*compute_shader = it->second.Get();
		(*compute_shader)->AddRef();
		return S_OK;
	}
	//�t�@�C������V�F�[�_�[�𐶐�����
	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	HRESULT hr = device->CreateComputeShader(cso_data.get(), cso_sz, nullptr, compute_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//�ۑ�����
	cache.insert(std::make_pair(cso_name, *compute_shader));

	return hr;
}
