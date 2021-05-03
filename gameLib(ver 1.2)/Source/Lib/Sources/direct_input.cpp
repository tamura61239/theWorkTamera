#include "direct_input.h"
#include <vector>

LPDIRECTINPUT8			g_lpDI = nullptr;
std::vector<LPDIRECTINPUTDEVICE8> g_lpJoysticks;

/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�
/*****************************************************/

BOOL PASCAL EnumJoyDeviceProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	LPDIRECTINPUTDEVICE8 g_lpJoystick;
	// �W���C�X�e�B�b�N�f�o�C�X�̍쐬
	HRESULT ret = g_lpDI->CreateDevice(lpddi->guidInstance, &g_lpJoystick, NULL);
	if (FAILED(ret)) {

		return DIENUM_STOP;
	}

	// ���̓f�[�^�`���̃Z�b�g
	ret = g_lpJoystick->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(ret)) {
		g_lpJoystick->Release();
		return DIENUM_STOP;
	}

	// ���͔͈͂̃Z�b�g
	DIPROPRANGE	diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;
	diprg.lMax = 32767L;
	diprg.lMin = -32767L;

	// X axis
	diprg.diph.dwObj = DIJOFS_X;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// Y axis
	diprg.diph.dwObj = DIJOFS_Y;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// Z axis
	diprg.diph.dwObj = DIJOFS_Z;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// RZ axis
	diprg.diph.dwObj = DIJOFS_RZ;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	diprg.lMax = 255L;
	diprg.lMin = 0L;
	// RX axis
	diprg.diph.dwObj = DIJOFS_RX;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	// RY axis
	diprg.diph.dwObj = DIJOFS_RY;
	g_lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

	g_lpJoystick->Poll();
	g_lpJoysticks.emplace_back(g_lpJoystick);
	// �ŏ���1�݂̂ŏI���
	return DIENUM_CONTINUE;			// ���̃f�o�C�X��񋓂���ɂ�DIENUM_CONTINUE��Ԃ�
}
/*************************************�R���X�g���N�^************************************/
DirectInput::DirectInput(const int id, const float deadzone_x, const float deadzone_y, HINSTANCE instance)
{
	mId = id;

	HRESULT hr = DirectInput8Create(instance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&g_lpDI, NULL);
	if (FAILED(hr))
	{
		assert(!"Could not Create DirectInput8");
	}

	hr = g_lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyDeviceProc, NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr)) {
		g_lpDI->Release();
		assert(!"Could not Create EnumDevices");
	}
	if (g_lpJoysticks.empty())
	{
		return;
	}
	if (!g_lpJoysticks.at(mId)) {
		// The gamepad was not find
		mCurrentState.connected = false;
		return;
	}

	// Device information
	DIDEVCAPS dc;
	dc.dwSize = sizeof(dc);
	g_lpJoysticks.at(mId)->GetCapabilities(&dc);

	// Start of operation
	g_lpJoysticks.at(mId)->Acquire();
	mCurrentState.connected = true;


	mDeadzoneY = deadzone_y;
	mDeadzoneX = deadzone_x;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@����֐�(�f�X�g���N�^)
/*****************************************************/

DirectInput::~DirectInput()
{
	if (g_lpDI != nullptr)
		g_lpDI->Release();
	if (g_lpJoysticks.empty())
	{
		return;
	};
	if (g_lpJoysticks.at(mId) != nullptr)
		g_lpJoysticks.at(mId)->Release();

	
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void DirectInput::Update()
{
	if (!mCurrentState.connected)
		return;
	//�O�̃t���[���̏�Ԃ�ێ�
	mPreviousState = mCurrentState;
	//���͏�Ԃ��擾
	HRESULT hr = g_lpJoysticks.at(mId)->GetDeviceState(sizeof(mCurrentState.state), &mCurrentState.state);
	//���͏�Ԃ̍X�V
	DirectionButtonState();
	StickState();
	TriggerState();
}
/***************************�{�^���̓��͏�Ԃ��擾****************************/
//���͒�
bool DirectInput::ButtonPressedState(int button)
{
	if (!mCurrentState.connected)
		return false;

	if (mCurrentState.state.rgbButtons[button] & 0x80)
	{
		return true;
	}
	return false;
}
//���͊J�n
bool DirectInput::ButtonRisingState(int button)
{
	if ((mCurrentState.state.rgbButtons[button] & 0x80) &&
		!(mPreviousState.state.rgbButtons[button] & 0x80))
	{
		return true;
	}
	return false;
}
//���͏I��
bool DirectInput::ButtonFallingState(int button)
{
	if (!(mCurrentState.state.rgbButtons[button] & 0x80) &&
		(mPreviousState.state.rgbButtons[button] & 0x80))
	{
		return true;
	}
	return false;
}
/**************************�\���{�^���̓��͏�Ԃ��擾**************************/
//���͒�
bool DirectInput::DirectionButtonPressedState(int button)
{
	return mCurrentState.directionButton[button] > 0;
}
//���͊J�n
bool DirectInput::DirectionButtonRisingState(int button)
{
	return mCurrentState.directionButton[button] > 0 && mPreviousState.directionButton[button] == 0;
}
//���͏I��
bool DirectInput::DirectionButtonFallingState(int button)
{
	return mCurrentState.directionButton[button] == 0 && mPreviousState.directionButton[button] > 0;
}
/***********************�\���{�^���̓��͏�Ԃ̍X�V**************************/
void DirectInput::DirectionButtonState()
{
	//��{�^��
	if (mCurrentState.state.rgdwPOV[0] == 0 ||
		mCurrentState.state.rgdwPOV[0] == 4500 ||
		mCurrentState.state.rgdwPOV[0] == 31500)
		++mCurrentState.directionButton[State::UP];
	else
	{//���͂��ĂȂ�
		mCurrentState.directionButton[State::UP] = 0;
	}
	//���{�^��
	if (mCurrentState.state.rgdwPOV[0] == 13500 ||
		mCurrentState.state.rgdwPOV[0] == 18000 ||
		mCurrentState.state.rgdwPOV[0] == 22500)
		++mCurrentState.directionButton[State::DOWN];
	else
	{//���͂��ĂȂ�
		mCurrentState.directionButton[State::DOWN] = 0;
	}
	//���{�^��
	if (mCurrentState.state.rgdwPOV[0] == 22500 ||
		mCurrentState.state.rgdwPOV[0] == 27000 ||
		mCurrentState.state.rgdwPOV[0] == 31500)
		++mCurrentState.directionButton[State::LEFT];
	else
	{//���͂��ĂȂ�
		mCurrentState.directionButton[State::LEFT] = 0;
	}
	//�E�{�^��
	if (mCurrentState.state.rgdwPOV[0] == 4500 ||
		mCurrentState.state.rgdwPOV[0] == 9000 ||
		mCurrentState.state.rgdwPOV[0] == 13500)
		++mCurrentState.directionButton[State::RIGHT];
	else
	{//���͂��ĂȂ�
		mCurrentState.directionButton[State::RIGHT] = 0;
	}
}
/****************************�X�e�B�b�N�̌X����̍X�V**************************/
void DirectInput::StickState()
{
	//����x���̌X���
	mCurrentState.lStick.x = ApplyDeadZone(static_cast<float>(mCurrentState.state.lX), MAX_STICKTILT, mDeadzoneX);
	//����y���̌X���
	mCurrentState.lStick.y = ApplyDeadZone(static_cast<float>(mCurrentState.state.lY), MAX_STICKTILT, mDeadzoneY);
	//�E��x���̌X���
	mCurrentState.rStick.x = ApplyDeadZone(static_cast<float>(mCurrentState.state.lZ), MAX_STICKTILT, mDeadzoneX);
	//�E��y���̌X���
	mCurrentState.rStick.y = ApplyDeadZone(static_cast<float>(mCurrentState.state.lRz), MAX_STICKTILT, mDeadzoneY);
}
/*********************************�g���K�[�̓��͏��*******************************/
void DirectInput::TriggerState()
{
	//���̃g���K�[
	mCurrentState.lTrigger = ApplyDeadZone(static_cast<float>(mCurrentState.state.lRx), MAX_TRRIGERTILT, 0.0f);
	//�E�̃g���K�[
	mCurrentState.rTrigger = ApplyDeadZone(static_cast<float>(mCurrentState.state.lRy), MAX_TRRIGERTILT, 0.0f);
}
/*********************************�������݂𒲂ׂ�*******************************/
float DirectInput::ApplyDeadZone(const float value, const float max_value, const float deadzone)
{
	float normalize_value = value / max_value;

	if (normalize_value > -deadzone && normalize_value < deadzone)
	{
		return 0.0f;
	}

	return (std::max)((std::min)(normalize_value, 1.0f), -1.0f);
}