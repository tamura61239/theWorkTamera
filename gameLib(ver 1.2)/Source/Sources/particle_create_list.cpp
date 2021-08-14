#include"particle_create_list.h"
#include"particle_create_base.h"
#include"particle_create_point.h"
#include"framework.h"

#define STR(var) #var   //�����ɂ����ϐ���ϐ��������������񃊃e�����Ƃ��ĕԂ��}�N���֐�

namespace ParticleCreateList
{
	std::array<char*,CreateList::Max>GetCreateList()
	{
		static std::array<char*, CreateList::Max>nameList =
		{
			STR(Point)
		};

		return nameList;
	}

	ParticleCreateBase* CreateParticleCreate(int type)
	{

		switch (type)
		{
		case CreateList::Point:
			return new ParticleCreatePoint(Framework::Instance().GetDevice().Get());
			break;
		}
		return nullptr;
	}

}
