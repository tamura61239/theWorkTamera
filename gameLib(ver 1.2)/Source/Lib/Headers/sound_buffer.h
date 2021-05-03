#pragma once
#define SOUNDMODO 1
#if SOUNDMODO
#include <xaudio2.h>

#include <queue>
#include <memory>

//**************************************
//   ���̃o�b�t�@�N���X
//**************************************
class SoundBuffer
{
public:
	//�R���X�g���N�^
	SoundBuffer(LPCWSTR szFilename);
	//�f�X�g���N�^
	~SoundBuffer() {
		for (size_t i = 0; i < mPSourceVoices.size(); ++i)
		{
			if (mPSourceVoices.at(i))
				mPSourceVoices.at(i)->DestroyVoice();
		}
		mPSourceVoices.clear();
	};
	//����炵�n�߂�
	HRESULT PlayWave(IXAudio2* pXaudio2, bool loop);
	//�X�V
	void Update();
	//���
	void Uninitlize();
	//�������Ă��邩�ǂ���
	bool Playing();
	//�~�߂�
	void Stop();
	//�ꎞ��~
	void Pause();
	//���ʂ̐ݒ�
	void Volume(float volume);
private:
	//���f�[�^
	struct WAVData
	{
		const WAVEFORMATEX* wfx;
		const unsigned char* startAudio;
		unsigned int audioBytes;
		unsigned int loopStart;
		unsigned int loopLength;
		const unsigned int* seek;       // Note: XMA Seek data is Big-Endian
		unsigned int seekCount;
	};
	std::unique_ptr<uint8_t[]> mWaveFiles;
	WAVData mWaveData;
	std::vector<IXAudio2SourceVoice*> mPSourceVoices;
	//�������Ă�t���O
	bool mPlaying;
	//�t�@�C������f�[�^���擾
	HRESULT FindMediaFile(wchar_t* strDestPath, int cchDest, LPCWSTR strFilename);
	HRESULT LoadWAVAudioFromFile(const wchar_t* szFileName, std::unique_ptr<uint8_t[]>& wavData, WAVData& result);
	//������
	HRESULT Initialize(LPCWSTR szFilename);
};
#endif