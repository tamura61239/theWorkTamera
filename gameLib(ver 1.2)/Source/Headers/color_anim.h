#pragma once
#include"particle_move.h"

class ColorAnim :public ParticleMove
{
public:
	ColorAnim(ID3D11Device* device, std::string fileName = "");
	void Editor(void* id);
	void Move(ID3D11DeviceContext* context, UINT x);
private:
	struct ColorData
	{
		VECTOR4F color[4];
		float time[4];
	};
	std::unique_ptr<ConstantBuffer<ColorData>>mCBuffer;

};