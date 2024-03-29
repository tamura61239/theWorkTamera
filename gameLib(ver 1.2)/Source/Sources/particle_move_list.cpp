#include"particle_move.h"
#include"color_anim.h"
#include"particle_move_list.h"
#include"framework.h"
#define STR(var) #var   //引数にした変数を変数名を示す文字列リテラルとして返すマクロ関数

namespace ParticleMoveList
{
	std::array<char*, MoveList::Max> GetMoveList()
	{
		static std::array<char*, MoveList::Max>nameList =
		{
			STR(Color)
		};

		return nameList;
	}

	ParticleMove* CreateParticleMove(int type)
	{
		switch (type)
		{
		case MoveList::Color:
			return new ColorAnim(Framework::Instance().GetDevice().Get());
			break;
		}

		return nullptr;
	}

}
