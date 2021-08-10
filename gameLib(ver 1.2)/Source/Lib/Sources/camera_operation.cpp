#include "camera_operation.h"
#include<Windows.h>
#include"key_board.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�(�R���X�g���N�^)
/*****************************************************/

CameraOperation::CameraOperation(Camera* camera):mType(CAMERA_TYPE::NORMAL), mOldCursor(0,0),mNewCursor(0,0),mDistance(0)
{
	VECTOR3F focusF = camera->GetFocus();
	VECTOR3F eyeF = camera->GetEye();
	VECTOR3F l = focusF - eyeF;

	mDistance = sqrtf(l.x * l.x + l.y * l.y + l.z * l.z);

}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�G�f�B�^�֐�
/*****************************************************/

void CameraOperation::Editor(void* id, float x, float y)
{
#ifdef USE_IMGUI
	ImGuiID* imguiId = (ImGuiID*)id;
	ImGui::BeginChild(*imguiId, ImVec2(x, y), ImGuiWindowFlags_NoTitleBar);
	if (ImGui::Button("debug"))
	{
		mType = CAMERA_TYPE::DEBUG;
	}
	ImGui::EndChild();
#endif
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/

void CameraOperation::Update(Camera* camera, float elapsedTime)
{
	switch (mType)
	{
	case CAMERA_TYPE::DEBUG:
		DebugCamera(camera);
		break;
	}
}
/***************************�f�o�b�N�J����************************/
void CameraOperation::DebugCamera(Camera* camera)
{
	//�J�������̎擾
	VECTOR3F focusF = camera->GetFocus();
	VECTOR3F upF = camera->GetUp();
	VECTOR3F rightF = camera->GetRight();
	VECTOR3F eyeF = camera->GetEye();
	//�}�E�X���W�̎擾
	POINT cursor;
	::GetCursorPos(&cursor);
	//�O�̃t���[���̃}�E�X���W��ۑ�
	mOldCursor = mNewCursor;
	//�V�����}�E�X���W���Z�b�g
	mNewCursor = VECTOR2F(static_cast<float>(cursor.x), static_cast<float>(cursor.y));

	//�ړ��ʂ��v�Z
	float move_x = (mNewCursor.x - mOldCursor.x) * 0.02f;
	float move_y = (mNewCursor.y - mOldCursor.y) * 0.02f;

	// Alt�L�[
	if (::GetAsyncKeyState(VK_MENU) & 0x8000)
	{
		//���N���b�N
		if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			// Y����]
			mRotate.y += move_x * 0.5f;
			if (mRotate.y > DirectX::XM_PI)
			{
				mRotate.y -= DirectX::XM_2PI;
			}
			else if (mRotate.y < -DirectX::XM_PI)
			{
				mRotate.y += DirectX::XM_2PI;
			}
			// X����]
			mRotate.x += move_y * 0.5f;
			if (mRotate.x > DirectX::XM_PI)
			{
				mRotate.x -= DirectX::XM_2PI;
			}
			else if (mRotate.x < -DirectX::XM_PI)
			{
				mRotate.x += DirectX::XM_2PI;
			}
		}
		else if (::GetAsyncKeyState(VK_MBUTTON) & 0x8000)
		{
			// ���s�ړ�
			float s = mDistance * 0.035f;
			float x = -move_x * s;
			float y = move_y * s;

			focusF.x += rightF.x * x;
			focusF.y += rightF.y * x;
			focusF.z += rightF.z * x;

			focusF.x += upF.x * y;
			focusF.y += upF.y * y;
			focusF.z += upF.z * y;
		}
		//�E�N���b�N
		else if (::GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			// �Y�[��
			mDistance += (-move_y - move_x) * mDistance * 0.1f;
		}

		//��]��sin,cos����
		float sx = ::sinf(mRotate.x);
		float cx = ::cosf(mRotate.x);
		float sy = ::sinf(mRotate.y);
		float cy = ::cosf(mRotate.y);

		//�v�Z����sin,cos���g���đO�x�N�g���A�E�x�N�g���A��x�N�g�����o��
		DirectX::XMVECTOR front = DirectX::XMVectorSet(-cx * sy, -sx, -cx * cy, 0.0f);
		DirectX::XMVECTOR right = DirectX::XMVectorSet(cy, 0, -sy, 0.0f);
		DirectX::XMVECTOR up = DirectX::XMVector3Cross(right, front);

		//�O�x�N�g���Ƌ����ƒ����_(focus)���g���ăJ�������W(eye)�����߂�
		DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&focusF);
		DirectX::XMVECTOR mDistance = DirectX::XMVectorSet(this->mDistance, this->mDistance, this->mDistance, 0.0f);
		DirectX::XMVECTOR eye = DirectX::XMVectorSubtract(focus, DirectX::XMVectorMultiply(front, mDistance));
		DirectX::XMStoreFloat3(&eyeF, eye);
		DirectX::XMStoreFloat3(&upF, up);
		//�o���l���J�����ɃZ�b�g����
		camera->SetEye(eyeF);
		camera->SetFocus(focusF);
		camera->SetUp(upF);
	}
}

