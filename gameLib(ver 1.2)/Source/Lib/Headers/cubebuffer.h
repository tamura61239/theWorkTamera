#pragma once
#include"framebuffer.h"

//�L���[�u�}�b�v�p�̃f�v�X�o�b�t�@�N���X
class CubeBuffer :public FrameBuffer
{
public:
	//�R���X�g���N�^
	CubeBuffer(ID3D11Device* device, int width, int height, bool msaaFlag = false, int subsamples = 1, DXGI_FORMAT renderTargetTexture2dFormat = DXGI_FORMAT_R8G8B8A8_UNORM, int arraySize = 6,
		DXGI_FORMAT depthStencilTexture2dFormat = DXGI_FORMAT_R32_TYPELESS,
		bool renderTargetShaderResourceViewFlag = true, bool depthStencilShaderResourceViewFlag = true, bool mipsFlag = true);
};