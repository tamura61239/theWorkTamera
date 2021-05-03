#pragma once
#define DIRECTINPUT_VERSION     0x0800
#include <windows.h>

#include "vector.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>

//DirectInput�N���X
class DirectInput
{
public:
	//�R���X�g���N�^
	DirectInput(const int id, const float deadzone_x, const float deadzone_y, HINSTANCE instance);
	~DirectInput();
		
	//�X�V
	void Update();
	//�{�^���������Ă��
	bool ButtonPressedState(int pad);
	//�{�^�����������u��
	bool ButtonRisingState(int pad);
	//�{�^���𗣂����u��
	bool ButtonFallingState(int pad);

	//�\���{�^���������Ă��
	bool DirectionButtonPressedState(int pad);
	//�\���{�^�����������u��
	bool DirectionButtonRisingState(int pad);
	//�\���{�^���𗣂����u��
	bool DirectionButtonFallingState(int pad);

	//�X�e�B�b�N�̂����̌X���
	float StickStateX(bool right) { if (right) return mCurrentState.rStick.x; else return mCurrentState.lStick.x; }
	//�X�e�B�b�N�̂����̌X���
	float StickStateY(bool right) { if (right) return mCurrentState.rStick.y; else return mCurrentState.lStick.y; }

	//�X�e�B�b�N�̉������ݒ�
	bool TriggerPressedState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f; else return mCurrentState.lTrigger > 0.0f; }
	//�X�e�B�b�N�̉������݊J�n
	bool TriggerRisingState(bool right) { if (right) return mCurrentState.rTrigger > 0.0f  && mPreviousState.rTrigger == 0.0f; else return mCurrentState.lTrigger > 0.0f  && mPreviousState.lTrigger == 0.0f; }
	//�X�e�B�b�N�̉������ݏI��
	bool TriggerFallingState(bool right) { if (right) return mCurrentState.rTrigger == 0.0f && mPreviousState.rTrigger > 0.0f;  else return mCurrentState.lTrigger == 0.0f && mPreviousState.lTrigger > 0.0f; }
private:
	//���
	struct State
	{
		DIJOYSTATE state;

		enum { UP, DOWN, LEFT, RIGHT };
		int directionButton[4];
		float rTrigger;
		float lTrigger;

		VECTOR2F rStick;
		VECTOR2F lStick;

		bool connected = false;
	};

	static constexpr float MAX_STICKTILT = 32767.0f;
	static constexpr float MAX_TRRIGERTILT = 255.0f;
	//�Q�[���p�b�h��ID
	int mId;
	//���̃t���[���̃p�b�h�̏��
	State mCurrentState;
	//�O�̃t���[���̃p�b�h�̏��
	State mPreviousState;

	float mDeadzoneX;
	float mDeadzoneY;

	void DirectionButtonState();
	void StickState();
	void TriggerState();
	float ApplyDeadZone(const float value, const float max_value, const float deadzone);

};