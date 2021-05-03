#pragma once
#include "sound_buffer.h"

//**************************************
//   ��
//**************************************
class Sound
{
public:
	//�R���X�g���N�^
	Sound(const char* name);
	//�f�X�g���N�^
	~Sound();
	//�X�V
	void Update();
	//����炵�n�߂�
	void Play(bool loop = false);
	//�������Ă��邩�ǂ���
	bool Playing();
	//�ꎞ��~
	void Pause();
	//��~
	void Stop();
	//���ʂ̐ݒ�
	void SetVolume(const float volume);
private:
	IXAudio2* mPXaudio2 = nullptr;
	IXAudio2MasteringVoice* mPMasteringVoice = nullptr;

	std::unique_ptr<SoundBuffer> mSoundBuffer;

	const char* mWavName;

};
