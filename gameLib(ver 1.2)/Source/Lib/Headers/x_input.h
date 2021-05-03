#pragma once
#include "vector.h"

#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput9_1_0.lib")
//XInput�N���X
class XInput
{
public:
	//�R���X�g���N�^
	XInput(const int id, const float deadzone_x, const float deadzone_y);
	~XInput() {};
	//�X�V
	void Update();
	//�{�^���������Ă��
	bool ButtonPressedState(int pad) { if (this->mCurrentState.state.Gamepad.wButtons & pad)return true; return false; }
	//�{�^�����������u��
	bool ButtonRisingState(int pad) { if ((this->mCurrentState.state.Gamepad.wButtons & pad) && !(this->mPreviousState.state.Gamepad.wButtons & pad))return true; return false; }
	//�{�^���𗣂����u��
	bool ButtonFallingState(int pad) { if (!(this->mCurrentState.state.Gamepad.wButtons & pad) && (this->mPreviousState.state.Gamepad.wButtons & pad))return true; return false; }

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
	//���͏��
	struct State {
		XINPUT_STATE state;

		float rTrigger;
		float lTrigger;

		VECTOR2F rStick;
		VECTOR2F lStick;

		bool connected = false;
	};

	static constexpr float MAX_STICKTILT = 32767.0f;
	static constexpr float MAX_TRRIGERTILT = 255.0f;

	//���̃t���[���̃p�b�h�̏��
	State mCurrentState;
	//�O�̃t���[���̃p�b�h�̏��
	State mPreviousState;
	//�R���g���[���[��ID
	int mId;

	float mDeadzoneX;
	float mDeadzoneY;

	void StickState();
	void TriggerState();
	float ApplyDeadZone(const float value, const float max_value, const float deadzone);

};