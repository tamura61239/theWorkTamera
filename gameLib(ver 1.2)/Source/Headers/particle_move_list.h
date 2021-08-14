#pragma once
#include<array>
class ParticleMove;

namespace ParticleMoveList
{
	enum MoveList
	{
		Color,
		Max
	};
	std::array<char*, MoveList::Max> GetMoveList();

	ParticleMove* CreateParticleMove(int type);
};
