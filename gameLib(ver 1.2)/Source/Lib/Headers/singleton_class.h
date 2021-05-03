#pragma once
#include<memory>
//�V���O���g���N���X
template <class T>

class Singleton
{
public:
	//����
	static void Create()
	{
		if (!ref())
		{
			ref() = std::make_unique<T>();
		}
	}
	//����
	static void Destroy()
	{
		if(ref())ref().reset();
	}
	//�C���X�^���X
	static const std::unique_ptr<T>& GetInctance()
	{
		return ref();
	}
protected:
	Singleton() {}

private:
	static std::unique_ptr<T>& ref()
	{
		static std::unique_ptr<T>p = std::make_unique<T>();
		return p;
	}
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
};
