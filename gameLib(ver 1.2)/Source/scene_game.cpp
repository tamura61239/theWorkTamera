#include "scene_game.h"
#include"key_board.h"
#include"scene_manager.h"
#include"blend_state.h"
#include"sound_manager.h"
#include"camera_manager.h"
#include"gamepad.h"
#include"light.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

std::unique_ptr<blend_state> blend[2];
SceneGame::SceneGame(ID3D11Device* device)
{
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);
			//mesh = std::make_unique<StaticMesh>(device, "Data/FBX/source/Dragon_ver2.fbx", SHADER_TYPE::USEALLY);
			//meshs.push_back(std::make_unique<StaticMesh>(device, "Data/FBX/source/Dragon_ver2.fbx", SHADER_TYPE::NORMAL));
			//meshs.push_back(std::make_unique<StaticMesh>(device, "Data/FBX/Stage/stage01.fbx", SHADER_TYPE::USEALLY));
			staticObjs.push_back(std::make_unique<StaticObj>(device, "Data/FBX/source/Dragon_ver2.fbx", SHADER_TYPE::USEALLY));
			staticObjs.back()->SetPosition(VECTOR3F(0, 0, 0));
			//staticObjs.back()->SetScale(VECTOR3F(0.1, 0.1, 0.1));
			staticObjs.back()->CalculateTransform();
			staticObjs.push_back(std::make_unique<StaticObj>(device, "Data/FBX/floor01/floor01.fbx", SHADER_TYPE::USEALLY));
			staticObjs.back()->SetPosition(VECTOR3F(0, -10, 0));
			//staticObjs.back()->SetScale(VECTOR3F(20, 20, 20));
			staticObjs.back()->CalculateTransform();
			std::vector<TextureMapData>datas;
			//knight
			shadowMap = std::make_unique<FrameBuffer>(device, 1024 * 5, 1024 * 5, false/*enable_msaa*/, 1, DXGI_FORMAT_UNKNOWN/*not needed*/, DXGI_FORMAT_R32_TYPELESS);
			frameBuffer[0] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			frameBuffer[1] = std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R24G8_TYPELESS);
			meshRender = std::make_unique<MeshRender>(device);
			renderEffects = std::make_unique<RenderEffects>(device);
			std::unique_ptr<ModelData>data = std::make_unique<ModelData>("Data/FBX/Knight/knight_attack03(v03).fbx", true);
			std::shared_ptr<ModelResource>resource = std::make_shared<ModelResource>(device, std::move(data),SHADER_TYPE::NORMAL);
			player = std::make_unique<Player>(std::make_shared<Character>(resource));

			modelRenderer = std::make_unique<ModelRenderer>(device);
			bloom = std::make_unique<BloomRender>(device, 1920, 1080);
			mGParticleManager = std::make_unique<GpuParticleManager>(device);
		}, device);
	test = std::make_unique<Sprite>(device, L"Data/image/�Q�[���e�X�g.png");
	nowLoading = std::make_unique<Sprite>(device, L"Data/image/wp-thumb.jpg");
	blend[0] = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);
	blend[1] = std::make_unique<blend_state>(device, BLEND_MODE::ADD);
	pCamera.CreateCamera();
	pCamera.GetCamera()->SetEye(VECTOR3F(100, 40, -200));
	//pSoundManager.Play(0, true);
	//pSoundManager.SetVolume(0, 1.0f);
	pLight.CreateLightBuffer(device);
}
static 	float playerPosition[3] = { 20, -10, 0 };
static float de = 50;
void SceneGame::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();
	pCamera.Update(elapsed_time);

#ifdef USE_IMGUI
	//// �X�|�b�g���C�g 
	VECTOR4F light = pLight.GetLightDirection();
	ImGui::Begin("light");
	ImGui::DragFloat("de", &de);
	ImGui::SliderFloat("x", &light.x, -1, 1);
	ImGui::SliderFloat("y", &light.y, -1, 1);
	ImGui::SliderFloat("z", &light.z, -1, 1);
	ImGui::SliderFloat("w", &light.w, -1, 1);
	ImGui::End();
	pLight.SetLightDirection(light);
	ImGui::Begin("player");
	ImGui::DragFloat3("position", playerPosition);
	ImGui::End();
	ImGui::Begin("buffer");
	ImGui::Image(frameBuffer[0]->GetRenderTargetShaderResourceView().Get(),ImVec2(1920,1080));
	ImGui::End();
#endif
	player->Update(elapsed_time);
	bloom->ImGuiUpdate();
	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		pSceneManager.ChangeScene(SCENETYPE::OVER);
		//pSoundManager.Stop(0);
		return;
	}
}

void SceneGame::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);

	if (IsNowLoading())
	{
		static float loadTimer = 0;

		VECTOR4F color;
		if (loadTimer < 1)
		{
			color = VECTOR4F(1, 1, 1, loadTimer);
		}
		else if (loadTimer < 3)
		{
			color = VECTOR4F(1, 1, 1, 1);
		}
		else if (loadTimer < 4)
		{
			color = VECTOR4F(1, 1, 1, 4 - loadTimer);
		}
		else
		{
			loadTimer = 0;
			color = VECTOR4F(1, 1, 1, 0);
		}
		loadTimer += elapsed_time;
		blend[0]->activate(context);
		//nowLoading->Render(context, VECTOR2F(0, 0), VECTOR2F(viewport.Width, viewport.Height), VECTOR2F(0, 0), VECTOR2F(1200, 675), 0, color);
		blend[0]->deactivate(context);

		return;
	}
	mGParticleManager->Update(context,elapsed_time);
	/**********************�V���h�E�}�b�v�e�N�X�`���̍쐬************************/
	//shadowMap->Clear(context);
	//shadowMap->Activate(context);
	//context->RSGetViewports(&num_viewports, &viewport);
	//static float aspect = viewport.Width / viewport.Height;

	//Camera lightCamera;
	//lightCamera.SetPerspective(100.0f * 0.01745f, aspect, 1.0f, 1000.0f);
	//VECTOR3F lFocus = VECTOR3F(playerPosition[0],playerPosition[1],playerPosition[2]);

	//DirectX::XMVECTOR direction = DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&pLight.GetLightDirection()));
	//DirectX::XMVECTOR focus = DirectX::XMLoadFloat3(&VECTOR3F(lFocus.x, lFocus.y, lFocus.z));
	//VECTOR3F d;
	//DirectX::XMStoreFloat3(&d, DirectX::XMVectorAdd(DirectX::XMVectorScale(direction, de), focus));
	//lightCamera.SetEye(VECTOR3F(d.x, d.y, d.z));
	//lightCamera.SetFocus(lFocus);
	//lightCamera.CalculateMatrix();

	//FLOAT4X4 lightView = lightCamera.GetView();
	//FLOAT4X4 lightProjection = lightCamera.GetProjection();
	//meshRender->ShadowBegin(context, lightView, lightProjection);
	//for (auto& obj : staticObjs)
	//{
	//	meshRender->ShadowRender(context, obj->GetMesh(), obj->GetWorld());
	//}
	//meshRender->ShadowEnd(context);
	//FLOAT4X4 lightViewProjection;
	//DirectX::XMStoreFloat4x4(&lightViewProjection, DirectX::XMLoadFloat4x4(&lightView) * DirectX::XMLoadFloat4x4(&lightProjection));
	//modelRenderer->ShadowBegin(context, lightViewProjection);
	//modelRenderer->ShadowDraw(context, *player->GetCharacter()->GetModel());
	//modelRenderer->ShadowEnd(context);
	//shadowMap->Deactivate(context);

	//test->Render(context, VECTOR2F(0, 0), VECTOR2F(222, 96), VECTOR2F(0, 0), VECTOR2F(111, 48), 0);
	//const VECTOR4F light = VECTOR4F(0, -0.5f, -1, 0);
	FLOAT4X4 view = pCamera.GetCamera()->GetView();
	FLOAT4X4 projection = pCamera.GetCamera()->GetProjection();
	FLOAT4X4 viewProjection;

	DirectX::XMStoreFloat4x4(&viewProjection, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));

	///************************�J���[�}�b�v�e�N�X�`���̍쐬***********************/
	frameBuffer[0]->Clear(context);
	frameBuffer[0]->Activate(context);
	pLight.ConstanceLightBufferSetShader(context);

	//modelRenderer->Begin(context, viewProjection, pLight.GetLightDirection());
	//modelRenderer->Draw(context, *player->GetCharacter()->GetModel());
	//modelRenderer->End(context);
	blend[0]->activate(context);
	meshRender->Begin(context, pLight.GetLightDirection(), view, projection);
	//for (auto& obj : staticObjs)
	//{
	//	meshRender->Render(context, obj->GetMesh(), obj->GetWorld());
	//}
	meshRender->Render(context, staticObjs[1]->GetMesh(), staticObjs[1]->GetWorld(),VECTOR4F(0.1,0.1,0.1,1));
	meshRender->End(context);
	//blend[0]->deactivate(context);
	//blend[0]->activate(context);
	mGParticleManager->Render(context, view, projection);
	blend[0]->deactivate(context);
	frameBuffer[0]->Deactivate(context);

	///****************�e������******************/
	//renderEffects->ShadowRender(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), frameBuffer[0]->GetDepthStencilShaderResourceView().Get(), shadowMap->GetDepthStencilShaderResourceView().Get(), view, projection, lightCamera.GetView(), lightCamera.GetProjection());
	bloom->Render(context, frameBuffer[0]->GetRenderTargetShaderResourceView().Get(), true);
	//test->Render(context, bloom->GetFrameBuffer()[5]->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
}

SceneGame::~SceneGame()
{
	pCamera.DestroyCamera();
}
