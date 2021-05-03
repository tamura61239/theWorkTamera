#pragma once
#include"sound.h"
//�点�鉹�̊Ǘ��N���X
class SoundManager
{
public:
	//�炵�n�߂�
	void Play(int soundNo, bool loop = false)
	{
		mSound.at(soundNo)->Play(loop);
	}
	//��~
	void Stop(int soundNo)
	{
		mSound.at(soundNo)->Pause();
	}
	//�X�V
	void Update()
	{
		for (auto& sound : mSound)
		{
			sound->Update();
		}
	}
	//���ʐݒ�
	void SetVolume(int soundNo, const float volume)
	{
		mSound.at(soundNo)->SetVolume(volume);
	}
	//�f�X�g���N�^
	~SoundManager()
	{
		mSound.clear();
	}
	//���̎�ނƔz��ԍ�
	struct SoundData
	{
		int soundNo;
		const char* soundfile;
	};
	//���̎��
	enum SOUND
	{
		BGM_Title,
		BGM_Loading,
		BGM_Game,
		BGM_CLEAR,
		BGM_OVER,
		max
	};
	//�C���X�^���X
	static SoundManager& getinctance()
	{
		static SoundManager s;
		return s;
	}
private:
	//���̃��X�g
	std::vector<std::unique_ptr<Sound>>mSound;
	//�R���X�g���N�^
	SoundManager();

};
#define pSoundManager (SoundManager::getinctance())

