#include "camera_manager.h"
#include<d3d11.h>
#include"screen_size.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif


CameraManager::CameraManager(ID3D11Device* device, std::string fileName)
{
	mCamera = std::make_shared<Camera>(device);
	mCameraOperation = std::make_unique<CameraOperation>(mCamera);
	mCamera->SetPerspective(30 * (3.14f / 180.f), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 1000.0f);

}

void CameraManager::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("camera");
	if (ImGui::Button("debug"))
	{
		mCameraOperation->SetCameraType(CameraOperation::CAMERA_TYPE::DEBUG);
	}
	ImGui::End();
#endif
}

void CameraManager::Update(float elapsed_time)
{
	mCameraOperation->Update(elapsed_time);
	mCamera->CalculateMatrix();
}

