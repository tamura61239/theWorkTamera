#pragma once
#include<d3d11.h>
#include <wrl.h>
#include <assert.h>
#include <memory>
#include<vector>
#include<DirectXTex.h>
//�f�v�X�o�b�t�@�N���X
class FrameBuffer
{
public:
	//�R���X�g���N�^
	FrameBuffer() = default;
	FrameBuffer(ID3D11Device* device, int width, int height, bool msaaFlag = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthStencilTexture2dFormat = DXGI_FORMAT_R24G8_TYPELESS,
		bool renderTargetShaderResourceViewFlag = true, bool depthStencilShaderResourceViewFlag = true, bool mipsFlag = false);
	//�f�v�X�o�b�t�@�̃e�N�X�`��������������
	void Clear(ID3D11DeviceContext* context, float r = 0, float g = 0, float b = 0, float a = 1, unsigned int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, unsigned char stencil = 0);
	//�f�v�X�o�b�t�@���Z�b�g����
	void Activate(ID3D11DeviceContext* context);
	//�Z�b�g�����o�b�t�@�����ɖ߂�
	void Deactivate(ID3D11DeviceContext* context);

	//getter
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetRenderTargetShaderResourceView() { return mRenderTargetShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetDepthStencilShaderResourceView() { return mDepthStencilShaderResourceView; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>GetRenderargetView() { return mRenderTargetView; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>GetDepthStencilView() { return mDepthStencilView; }
	D3D11_VIEWPORT GetViewPort() { return mViewport; }
	//setter
	void SetPsTexture(ID3D11DeviceContext* context, const int number);
	void SetPsDepth(ID3D11DeviceContext* context, const int number);
	//SRV��dds�t�@�C���`���ŕۑ�����
	void SaveDDSFile(ID3D11DeviceContext* context, const wchar_t* fileName, ID3D11ShaderResourceView* srv);
	virtual~FrameBuffer() {}

protected:
	//�����_�[�^�[�Q�b�g�r���[�̃f�[�^
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mRenderTargetShaderResourceView;
	//�f�v�X�X�e���V���r���[�̃f�[�^
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mDepthStencilShaderResourceView;
	//���̃f�[�^��ێ�����
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mDefaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDefaultDepthStencilView;
	unsigned int mViewportsNumber;
	D3D11_VIEWPORT mDefaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	//��ʔ䗦��ێ�
	D3D11_VIEWPORT mViewport;

};
//�}���`�����_�[�^�[�Q�b�g�N���X
class MulltiRenderTargetFunction
{
public:
	//�R���X�g���N�^
	MulltiRenderTargetFunction():mViewportsNumber(0){}
	//setter
	void SetFrameBuffer(std::shared_ptr<FrameBuffer>frameBuffer)
	{
		mFrameBuffers.push_back(frameBuffer);
	}
	//�f�v�X�o�b�t�@���w�肵�������
	void DeleteFrameBuffer(const int num)
	{
		mFrameBuffers.erase(mFrameBuffers.begin() + num);
	}
	//�f�v�X�o�b�t�@���������
	void ClearFrameBuffer()
	{
		mFrameBuffers.clear();
	}
	//�f�v�X�o�b�t�@�̃e�N�X�`��������������
	void Clear(ID3D11DeviceContext* context, float r = 0, float g = 0, float b = 0, float a = 1, unsigned int clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, float depth = 1, unsigned char stencil = 0);
	//�f�v�X�o�b�t�@���Z�b�g����
	void Activate(ID3D11DeviceContext* context);
	//�Z�b�g�����o�b�t�@�����ɖ߂�
	void Deactivate(ID3D11DeviceContext* context);
	//getter
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetDepthStencilShaderResourceView() 
	{ 
		if (mFrameBuffers.size() <= 0)return nullptr;
		return mFrameBuffers[0]->GetDepthStencilShaderResourceView(); 
	}

private:
	//�f�v�X�o�b�t�@
	std::vector<std::shared_ptr<FrameBuffer>>mFrameBuffers;
	//���̃f�[�^
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>mDefaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>mDefaultDepthStencilView;
	unsigned int mViewportsNumber;
	D3D11_VIEWPORT mDefaultViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];

};