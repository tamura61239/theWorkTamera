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
	mCamera = std::make_unique<Camera>(device);
	mCameraOperation = std::make_unique<CameraOperation>(mCamera.get());
	mCamera->SetPerspective(30 * (3.14f / 180.f), static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT), 0.1f, 10000.0f);

}

void CameraManager::Editor()
{
#ifdef USE_IMGUI
	ImGui::Begin("camera");
	ImGuiID id = ImGui::GetID((void*)0);
	mCameraOperation->Editor(&id, 0, 50);
	ImGui::Text("eye: %.4f,%.4f,%.4f", mCamera->GetEye().x, mCamera->GetEye().y, mCamera->GetEye().z);
	ImGui::Text("focus: %.4f,%.4f,%.4f", mCamera->GetFocus().x, mCamera->GetFocus().y, mCamera->GetFocus().z);
	ImGui::Text("up: %.4f,%.4f,%.4f", mCamera->GetUp().x, mCamera->GetUp().y, mCamera->GetUp().z);
	ImGui::Text("front: %.4f,%.4f,%.4f", mCamera->GetFront().x, mCamera->GetFront().y, mCamera->GetFront().z);
	ImGui::Text("right: %.4f,%.4f,%.4f", mCamera->GetRight().x, mCamera->GetRight().y, mCamera->GetRight().z);
	ImGui::End();
#endif
}

void CameraManager::Update(float elapsed_time)
{
	mCameraOperation->Update(mCamera.get(),elapsed_time);
	mCamera->CalculateMatrix();
}

