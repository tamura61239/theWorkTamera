#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include"misc.h"
//�T���v���[�X�e�[�g�N���X
class SamplerState
{
public:
	//�R���X�g���N�^
	SamplerState(ID3D11Device* device, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_MODE addressMode = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_COMPARISON_FUNC comparisonFunc = D3D11_COMPARISON_NEVER, VECTOR4F color = VECTOR4F(1, 1, 1, 1))
	{
		//�T���v���[�X�e�[�g
		HRESULT hr = S_OK;
		D3D11_SAMPLER_DESC desc = {};
		desc.Filter = filter;
		desc.AddressU = addressMode;
		desc.AddressV = addressMode;
		desc.AddressW = addressMode;
		desc.ComparisonFunc = comparisonFunc;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 1;
		memcpy(desc.BorderColor, &color, sizeof(VECTOR4F));
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//�T���v���[�X�e�[�g���Z�b�g����
	void Activate(ID3D11DeviceContext* context, UINT slot, bool vs = false, bool ps = false, bool gs = false, bool cs = false)
	{
		if (vs)
		{
			context->VSGetSamplers(slot, 1, mDefaltSampler[0].GetAddressOf());
			context->VSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		if (ps)
		{
			context->PSGetSamplers(slot, 1, mDefaltSampler[1].GetAddressOf());
			context->PSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		if (gs)
		{
			context->GSGetSamplers(slot, 1, mDefaltSampler[2].GetAddressOf());
			context->GSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		if (cs)
		{
			context->CSGetSamplers(slot, 1, mDefaltSampler[3].GetAddressOf());
			context->CSSetSamplers(slot, 1, mSamplerState.GetAddressOf());
		}
		mSlotNum = slot;
		mSetFlag[0] = vs;
		mSetFlag[1] = ps;
		mSetFlag[2] = gs;
		mSetFlag[3] = cs;

	}
	//�Z�b�g�����T���v���[�X�e�[�g�����ɖ߂�
	void DeActivate(ID3D11DeviceContext* context)
	{
		if (mSetFlag[0])context->VSSetSamplers(mSlotNum, 1, mDefaltSampler[0].GetAddressOf());
		if (mSetFlag[1])context->PSSetSamplers(mSlotNum, 1, mDefaltSampler[1].GetAddressOf());
		if (mSetFlag[2])context->GSSetSamplers(mSlotNum, 1, mDefaltSampler[2].GetAddressOf());
		if (mSetFlag[3])context->CSSetSamplers(mSlotNum, 1, mDefaltSampler[3].GetAddressOf());
	}
private:
	//�T���v���[�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;
	//�Z�b�g�O�̃T���v���[�X�e�[�g��ێ�
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mDefaltSampler[4] = { 0 };
	//GPU���̓o�^�ԍ�
	UINT mSlotNum;
	//�Z�b�g�����V�F�[�_�[��m��
	bool mSetFlag[4] = { false };

};