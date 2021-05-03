#include "x_input.h"
#include <Windows.h>
#include <algorithm>
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

XInput::XInput(const int id, const float deadzone_x, const float deadzone_y)
{
	mId = id;

	DWORD result = XInputGetState(mId, &mCurrentState.state);
	if (result != ERROR_DEVICE_NOT_CONNECTED)
	{
		mCurrentState.connected = true;
	}

	mDeadzoneY = deadzone_y;
	mDeadzoneX = deadzone_x;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void XInput::Update()
{
	if (!mCurrentState.connected)
		return;

	mPreviousState = mCurrentState;

	DWORD result = XInputGetState(mId, &mCurrentState.state);


	StickState();
	TriggerState();
}
/***********************�X�e�B�b�N�̓��͏��****************************/
void XInput::StickState()
{
	mCurrentState.lStick.x = ApplyDeadZone(static_cast<float>(mCurrentState.state.Gamepad.sThumbLX), MAX_STICKTILT, mDeadzoneX);

	mCurrentState.lStick.y = ApplyDeadZone(static_cast<float>(mCurrentState.state.Gamepad.sThumbLY), MAX_STICKTILT, mDeadzoneY);

	mCurrentState.rStick.x = ApplyDeadZone(static_cast<float>(mCurrentState.state.Gamepad.sThumbRX), MAX_STICKTILT, mDeadzoneX);

	mCurrentState.rStick.y = ApplyDeadZone(static_cast<float>(mCurrentState.state.Gamepad.sThumbRY), MAX_STICKTILT, mDeadzoneY);
}
/*******************�g���K�[�̓��͏��**********************/
void XInput::TriggerState()
{
	mCurrentState.lTrigger = ApplyDeadZone(static_cast<float>(mCurrentState.state.Gamepad.bLeftTrigger), MAX_TRRIGERTILT, 0.0f);
	mCurrentState.rTrigger = ApplyDeadZone(static_cast<float>(mCurrentState.state.Gamepad.bRightTrigger), MAX_TRRIGERTILT, 0.0f);
}
/*********************************�������݂𒲂ׂ�*******************************/

float XInput::ApplyDeadZone(const float value, const float max_value, const float deadzone)
{
	float normalize_value = value / max_value;

	if (normalize_value > -deadzone && normalize_value < deadzone)
	{
		return 0.0f;
	}

	return (std::max)((std::min)(normalize_value, 1.0f), -1.0f);
}