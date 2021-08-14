#pragma once
#include<array>
class ParticleCreateBase;

namespace ParticleCreateList
{
	enum CreateList
	{
		Point,
		Max
	};

	std::array<char*, CreateList::Max>GetCreateList();



	ParticleCreateBase* CreateParticleCreate(int type);
};
