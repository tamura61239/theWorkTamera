#pragma once
#include <memory>
#include "x_input.h"
#include "direct_input.h"

//�Q�[���p�b�h�̏�Ԏ擾�N���X
class GamePad
{
public:
	//�R���X�g���N�^
	GamePad(const int id, const float deadzone_x, const float deadzone_y, bool xinput, HINSTANCE m_hinstance);
	//�f�X�g���N�^
	~GamePad() {};
	//�X�V
	void Update();
	/*****************************�{�^���̏��************************************/
	//�����Ă��
	bool ButtonPressedState(int pad) { return mXinput ? mXPad->ButtonPressedState(pad) : mDPad->ButtonPressedState(pad); }
	//�����n��
	bool ButtonRisingState(int pad) { return mXinput ? mXPad->ButtonRisingState(pad) : mDPad->ButtonRisingState(pad); }
	//�������u��
	bool ButtonFallingState(int pad) { return mXinput ? mXPad->ButtonFallingState(pad) : mDPad->ButtonFallingState(pad); }

	/**************************DirectInput���玝���Ă����\���{�^���̏��*************************/
	//�����Ă��
	bool DirectionButtonPressedState(int pad) { return mDPad->DirectionButtonPressedState(pad); }
	//�����n��
	bool DirectionButtonRisingState(int pad) { return mDPad->DirectionButtonRisingState(pad); }
	//�������u��
	bool DirectionButtonFallingState(int pad) { return mDPad->DirectionButtonFallingState(pad); }

	/**********************************�X�e�B�b�N�̌X���**************************************/
	//X��
	float StickStateX(bool right) { return  mXinput ? mXPad->StickStateX(right) : mDPad->StickStateX(right); }
	//Y��
	float StickStateY(bool right) { return  mXinput ? mXPad->StickStateY(right) : mDPad->StickStateY(right); }

	/***************************�X�e�B�b�N�̉�������*************************/
	//�����Ă��
	bool TriggerPressedState(bool right) { return mXinput ? mXPad->TriggerPressedState(right) : mDPad->TriggerPressedState(right); }
	//�����n��
	bool TriggerRisingState(bool right) { return mXinput ? mXPad->TriggerRisingState(right) : mDPad->TriggerRisingState(right); }
	//�������u��
	bool TriggerFallingState(bool right) { return mXinput ? mXPad->TriggerFallingState(right) : mDPad->TriggerFallingState(right); }
	//getter
	bool GetXInput() { return mXinput; }
private:
	//XInput�Ŏ擾���邩�ǂ���
	bool mXinput;
	//XInput
	std::unique_ptr<XInput> mXPad;
	//DirectInput
	std::unique_ptr<DirectInput> mDPad;
};
//�����̃Q�[���p�b�h�̊Ǘ��N���X
class GamePadManager
{
public:
	//������
	void Initialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x = 0.1f, const float deadzone_y = 0.1f)
	{
		mPads[id] = std::make_unique<GamePad>(id, deadzone_x, deadzone_y, xinput, m_hinstance);
	};
	//�X�V
	void Update()
	{
		for (std::unique_ptr<GamePad>& pad : mPads)
		{
			if (pad.get() != nullptr)
				pad->Update();
		}
	}
	/*****************************�{�^���̏��************************************/
	//�����Ă��
	bool ButtonPressedState(int id, int pad) { return this->mPads[id]->ButtonPressedState(pad); }
	//�����n��
	bool ButtonRisingState(int id, int pad) { return this->mPads[id]->ButtonRisingState(pad); }
	//�������u��
	bool ButtonFallingState(int id, int pad) { return this->mPads[id]->ButtonFallingState(pad); }

	/**************************DirectInput���玝���Ă����\���{�^���̏��*************************/
	//�����Ă��
	bool DirectionButtonPressedState(int id, int pad) { return mPads[id]->DirectionButtonPressedState(pad); }
	//�����n��
	bool DirectionButtonRisingState(int id, int pad) { return mPads[id]->DirectionButtonRisingState(pad); }
	//�������u��
	bool DirectionButtonFallingState(int id, int pad) { return mPads[id]->DirectionButtonFallingState(pad); }

	/**********************************�X�e�B�b�N�̌X���**************************************/
	//X��
	float StickStateX(int id, bool right) { return this->mPads[id]->StickStateX(right); }
	//Y��
	float StickStateY(int id, bool right) { return this->mPads[id]->StickStateY(right); }

	/***************************�X�e�B�b�N�̉�������*************************/
	//�����Ă��
	bool TriggerPressedState(int id, bool right) { return this->mPads[id]->TriggerPressedState(right); }
	//�����n��
	bool TriggerRisingState(int id, bool right) { return this->mPads[id]->TriggerRisingState(right); }
	//�������u��
	bool TriggerFallingState(int id, bool right) { return this->mPads[id]->TriggerFallingState(right); }
	//getter
	bool GetXinput(int id) { return mPads[id]->GetXInput(); }
	//�C���X�^���X
	static GamePadManager& GetInstance()
	{
		static GamePadManager instance;
		return instance;
	}
private:
	//�Q�[���p�b�h
	std::unique_ptr<GamePad> mPads[4];
	//�R���X�g���N�^
	GamePadManager() {};
	//�f�X�g���N�^
	~GamePadManager() {};

};

namespace input
{
	//�{�^���̎��
	enum class PadLabel
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		START,
		BACK,
		LTHUMB,
		RTHUMB,
		LSHOULDER,
		RSHOULDER,
		A,
		B,
		X,
		Y,
	};
	//������
	void GamepadInitialize(int id, bool xinput, HINSTANCE m_hinstance, const float deadzone_x = 0.1f, const float deadzone_y = 0.1f);
	//�X�V
	void GamepadUpdate();
	/*****************************�{�^���̏��************************************/
    //�����Ă��
	bool ButtonPressedState(const int pad_id, PadLabel pad);
	//�����n��
	bool ButtonRisingState(const int pad_id, PadLabel pad);
	//�������u��
	bool ButtonFallingState(const int pad_id, PadLabel pad);
	/**********************************�X�e�B�b�N�̌X���**************************************/
	//X��
	float StickStateX(const int pad_id, bool right);
	//Y��
	float StickStateY(const int pad_id, bool right);
	/***************************�X�e�B�b�N�̉�������*************************/
	//�����Ă��
	bool TriggerPressedState(const int pad_id, bool right);
	//�����n��
	bool TriggerRisingState(const int pad_id, bool right);
	//�������u��
	bool TriggerFallingState(const int pad_id, bool right);
}
using namespace input;
