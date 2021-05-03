#pragma once
#include<d3d11.h>
#include<wrl.h>

class ParticleBuffer
{
public:
	template<class T>

	ParticleBuffer(ID3D11Device* device, D3D11_USAGE usage, UINT bindFlags, UINT miscFlags, T*objs,bool uav=false,bool srv=false)
	{
		HRESULT hr;

		//bufferの生成
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Usage = usage;
			desc.BindFlags = bindFlags;
			desc.MiscFlags = miscFlags;
			desc.StructureByteStride = miscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED ? sizeof(objs[0]) : 0;
			desc.ByteWidth = sizeof(objs);
			D3D11_SUBRESOURCE_DATA data;
			ZeroMemory(&data, sizeof(data));
			data.pSysMem = objs;
			hr = device->CreateBuffer(&desc, &data, mBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		if (uav)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			if (miscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				desc.Buffer.NumElements = sizeof(objs) / sizeof(T);
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			}
			else
			{
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				desc.Buffer.FirstElement = 0;
				desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
				desc.Buffer.NumElements = sizeof(objs) / 4;

			}
			hr = device->CreateUnorderedAccessView(mBuffer.Get(), &desc, mUAV.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}
		if (srv)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			if (miscFlags == D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
			{
				desc.Buffer.NumElements = sizeof(objs) / sizeof(T);
				desc.Format = DXGI_FORMAT_UNKNOWN;
				desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			}
			else
			{
				desc.Format = DXGI_FORMAT_R32_TYPELESS;
				desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				desc.BufferEx.FirstElement = 0;
				desc.BufferEx.Flags = D3D11_SRV_DIMENSION_BUFFEREX;
				desc.BufferEx.NumElements = sizeof(objs) / 4;
			}
			hr = device->CreateShaderResourceView(mBuffer.Get(), &desc, mSRV.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		}
	}
	//データをGPU側に送る
	void Activate(ID3D11DeviceContext* context, UINT shot, bool uav = false, bool srv = false)
	{
		if (uav)
		{
			context->CSGetUnorderedAccessViews(shot, 1, mDefUAV.GetAddressOf());
			context->CSSetUnorderedAccessViews(shot,1, mUAV.GetAddressOf(), nullptr);
		}
		if (srv)
		{
			context->CSGetShaderResources(shot, 1, mDefSRV.GetAddressOf());
			context->CSSetShaderResources(shot, 1,mSRV.GetAddressOf());
		}
		mUavFlag = uav;
		mSrvFlag = srv;
		mShot = shot;
	}
	//GPU側にセットしたのを元に戻す
	void DeActivate(ID3D11DeviceContext* context)
	{
		if (mUavFlag)context->CSSetUnorderedAccessViews(mShot, 1, mDefUAV.GetAddressOf(), nullptr);
		if (mSrvFlag)context->CSSetShaderResources(mShot, 1, mDefSRV.GetAddressOf());
	}
	//getter
	ID3D11Buffer* GetBuffer() { return mBuffer.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>mBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	bool mUavFlag;
	bool mSrvFlag;
	UINT mShot;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mDefUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mDefSRV;

};

