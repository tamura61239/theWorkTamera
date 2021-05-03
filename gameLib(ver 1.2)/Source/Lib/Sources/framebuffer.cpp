#include "framebuffer.h"
#include"misc.h"
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
FrameBuffer::FrameBuffer(ID3D11Device* device, int width, int height, bool msaaFlag, int subsamples, DXGI_FORMAT renderTargetTexture2dFormat, DXGI_FORMAT depthStencilTexture2dFormat,
	bool renderTargetShaderResourceViewFlag, bool depthStencilShaderResourceViewFlag, bool mipsFlag)
{
	HRESULT hr = S_OK;
	UINT qualityLevel;
	UINT sampleCount = subsamples;
	hr = device->CheckMultisampleQualityLevels(renderTargetTexture2dFormat, sampleCount, &qualityLevel);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	if (renderTargetTexture2dFormat != DXGI_FORMAT_UNKNOWN)
	{
		//�����_�[�^�[�Q�b�g�p�̃o�b�t�@����
		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = mipsFlag ? 0 : 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = renderTargetTexture2dFormat;
		texture2dDesc.SampleDesc.Count = msaaFlag ? sampleCount : 1;
		texture2dDesc.SampleDesc.Quality = msaaFlag ? qualityLevel - 1 : 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = renderTargetShaderResourceViewFlag ? D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_RENDER_TARGET;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = mipsFlag ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//�����_�[�^�[�Q�b�g�̐���
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
		renderTargetViewDesc.Format = renderTargetTexture2dFormat;
		renderTargetViewDesc.ViewDimension = msaaFlag ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = device->CreateRenderTargetView(texture2d.Get(), &renderTargetViewDesc, mRenderTargetView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (renderTargetShaderResourceViewFlag)
		{
			//�����_�[�^�[�Q�b�g�ɕ`�悵�����̂��������ރe�N�X�`���̐���
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = renderTargetTexture2dFormat;
			shaderResourceViewDesc.ViewDimension = msaaFlag ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MipLevels = mipsFlag ? -1 : 1;
			hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, mRenderTargetShaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

	}
	if (depthStencilTexture2dFormat != DXGI_FORMAT_UNKNOWN)
	{
		const DXGI_FORMAT combinationsOfDepthStencilFormats[3][3] =
		{
			{ DXGI_FORMAT_R24G8_TYPELESS,  DXGI_FORMAT_D24_UNORM_S8_UINT ,DXGI_FORMAT_R24_UNORM_X8_TYPELESS },
			{ DXGI_FORMAT_R32_TYPELESS,  DXGI_FORMAT_D32_FLOAT, DXGI_FORMAT_R32_FLOAT },
			{ DXGI_FORMAT_R16_TYPELESS,  DXGI_FORMAT_D16_UNORM , DXGI_FORMAT_R16_UNORM },
		};
		int depthStencilTexture2dFormatIndex = 0;
		switch (depthStencilTexture2dFormat)
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			depthStencilTexture2dFormatIndex = 0;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			depthStencilTexture2dFormatIndex = 1;
			break;
		case DXGI_FORMAT_R16_TYPELESS:
			depthStencilTexture2dFormatIndex = 2;
			break;
		}
		//�f�v�X�X�e���V���r���[�p�̃o�b�t�@����
		D3D11_TEXTURE2D_DESC texture2dDesc = {};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][0];
		texture2dDesc.SampleDesc.Count = msaaFlag ? sampleCount : 1;
		texture2dDesc.SampleDesc.Quality = msaaFlag ? qualityLevel - 1 : 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = depthStencilShaderResourceViewFlag ? D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_DEPTH_STENCIL;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2dDesc, 0, texture2d.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//�f�v�X�X�e���V���r���[�p�̃o�b�t�@����
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		depthStencilViewDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][1];
		depthStencilViewDesc.ViewDimension = msaaFlag ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		hr = device->CreateDepthStencilView(texture2d.Get(), &depthStencilViewDesc, mDepthStencilView.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (depthStencilShaderResourceViewFlag)
		{
			//�f�v�X�X�e���V���r���[�ŕ`�悵���[�x�l���������ރe�N�X�`���̐���
			D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
			shaderResourceViewDesc.Format = combinationsOfDepthStencilFormats[depthStencilTexture2dFormatIndex][2];
			shaderResourceViewDesc.ViewDimension = msaaFlag ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, mDepthStencilShaderResourceView.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�ݒ�֐�
/*****************************************************/
/**************************�����_�[�^�[�Q�b�g�e�N�X�`���̏�����***************************/
void FrameBuffer::Clear(ID3D11DeviceContext* context, float r, float g, float b, float a, unsigned int clearFlags, float depth, unsigned char stencil)
{
	//�����̐F
	float color[4] = { r,g,b,a };
	if (mRenderTargetView)
	{
		//�����_�[�^�[�Q�b�g�e�N�X�`����������
		context->ClearRenderTargetView(mRenderTargetView.Get(), color);
	}
	if (mDepthStencilView)
	{
		//�[�x�e�N�X�`����������
		context->ClearDepthStencilView(mDepthStencilView.Get(), clearFlags, depth, stencil);
	}
}
/***************************�����_�[�^�[�Q�b�g��ݒ�*****************************/
void FrameBuffer::Activate(ID3D11DeviceContext* context)
{
	//���̃����_�[�^�[�Q�b�g���擾
	mViewportsNumber = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	context->RSGetViewports(&mViewportsNumber, mDefaultViewports);

	context->OMGetRenderTargets(1, mDefaultRenderTargetView.GetAddressOf(), mDefaultDepthStencilView.GetAddressOf());
	//�����_�[�^�[�Q�b�g��ݒ�
	context->RSSetViewports(1, &mViewport);
	context->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

}
/****************************�ݒ肵�������_�[�^�[�Q�b�g�������ɖ߂�*******************************/
void FrameBuffer::Deactivate(ID3D11DeviceContext* context)
{
	//���̃����_�[�^�[�Q�b�g�ɖ߂�
	context->RSSetViewports(mViewportsNumber, mDefaultViewports);
	context->OMSetRenderTargets(1, mDefaultRenderTargetView.GetAddressOf(), mDefaultDepthStencilView.Get());

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@GPU���ɃZ�b�g����֐�
/*****************************************************/

/*************************�����_�[�^�[�Q�b�g�e�N�X�`����GPU���ɑ���**************************/
void FrameBuffer::SetPsTexture(ID3D11DeviceContext* context, const int number)
{
	context->PSSetShaderResources(number, 1, mRenderTargetShaderResourceView.GetAddressOf());
}
/*************************�[�x�e�N�X�`����GPU���ɑ���**************************/
void FrameBuffer::SetPsDepth(ID3D11DeviceContext* context, const int number)
{
	context->PSSetShaderResources(number, 1, mDepthStencilShaderResourceView.GetAddressOf());

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�e�N�X�`���̃Z�[�u�֐�
/*****************************************************/

void FrameBuffer::SaveDDSFile(ID3D11DeviceContext* context, const wchar_t* fileName, ID3D11ShaderResourceView* srv)
{
	DirectX::ScratchImage image;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	srv->GetResource(resource.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	context->GetDevice(device.ReleaseAndGetAddressOf());
	HRESULT hr = S_OK;
	hr = CaptureTexture(device.Get(), context, resource.Get(), image);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::DDS_FLAGS_NONE, fileName);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

/*********************************MulltiRenderTargetFunction**********************************/

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/
/**************************�����_�[�^�[�Q�b�g�e�N�X�`���̏�����***************************/

void MulltiRenderTargetFunction::Clear(ID3D11DeviceContext* context, float r, float g, float b, float a, unsigned int clearFlags, float depth, unsigned char stencil)
{
	for (auto& frame : mFrameBuffers)
	{
		frame->Clear(context, r, g, b, a, clearFlags, depth, stencil);
	}
}
/***************************�����_�[�^�[�Q�b�g��ݒ�*****************************/

void MulltiRenderTargetFunction::Activate(ID3D11DeviceContext* context)
{
	if (mFrameBuffers.size() <= 0)return;
	std::vector<ID3D11RenderTargetView*>rtvs;
	for (auto& frame : mFrameBuffers)
	{
		rtvs.push_back(frame->GetRenderargetView().Get());
	}
	//���̃����_�[�^�[�Q�b�g���擾
	mViewportsNumber = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	context->RSGetViewports(&mViewportsNumber, mDefaultViewports);
	//�����_�[�^�[�Q�b�g��ݒ肷��
	context->RSSetViewports(1, &mFrameBuffers[0]->GetViewPort());
	context->OMGetRenderTargets(1, mDefaultRenderTargetView.GetAddressOf(), mDefaultDepthStencilView.GetAddressOf());
	context->OMSetRenderTargets(rtvs.size(), &rtvs[0], mFrameBuffers[0]->GetDepthStencilView().Get());

}
/****************************�ݒ肵�������_�[�^�[�Q�b�g�������ɖ߂�*******************************/

void MulltiRenderTargetFunction::Deactivate(ID3D11DeviceContext* context)
{
	if (mFrameBuffers.size() <= 0)return;
	//���̃����_�[�^�[�Q�b�g�ɖ߂�
	context->RSSetViewports(mViewportsNumber, mDefaultViewports);
	context->OMSetRenderTargets(1, mDefaultRenderTargetView.GetAddressOf(), mDefaultDepthStencilView.Get());
}
