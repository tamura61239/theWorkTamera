#include "framework.h"
#include"screen_size.h"
#include"blend_state.h"
#include"scene_manager.h"
#include"camera_manager.h"
#include"gamepad.h"
Framework* Framework::inst = nullptr;

bool Framework::Initialize(HWND hwnd)
{
	HRESULT hr;

	//SwapChain��Device,Devicecontext�̐���
	{
		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};


		DXGI_SWAP_CHAIN_DESC    desc = {};
		//ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		desc.BufferCount = 1;					// �o�b�t�@�̐�
		desc.BufferDesc.Width = SCREEN_WIDTH;	// �o�b�t�@�̉���
		desc.BufferDesc.Height = SCREEN_HEIGHT;	// �o�b�t�@�̏c��
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �J���[�t�H�[�}�b�g
		desc.BufferDesc.RefreshRate.Numerator = 60;		// ���t���b�V�����[�g�̕���
		desc.BufferDesc.RefreshRate.Denominator = 1;		// ���t���b�V�����[�g�̕��q
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;		// �o�b�N�o�b�t�@�̐�
		desc.OutputWindow = hwnd;	// DirectX�ŕ`�������\������E�C���h�E
		desc.Windowed = TRUE;		// �E�C���h�E���[�h���A�t���X�N���[���ɂ��邩�B
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH

		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,	// �ǂ̃r�f�I�A�_�v�^���g�p���邩�H����Ȃ��nullptr�ŁAIDXGIAdapter�̃A�h���X��n��.
			D3D_DRIVER_TYPE_HARDWARE,	// �h���C�o�̃^�C�v��n���B����ȊO�͊�{�I�Ƀ\�t�g�E�F�A�����ŁA�ǂ����Ă��Ƃ�������f�o�O�p�ɗp����ׂ�.
			nullptr,	// ��L��D3D_DRIVER_TYPE_SOFTWARE�ɐݒ肵���ۂɁA���̏������s��DLL�̃n���h����n���B����ȊO���w�肵�Ă���ۂɂ͕K��nullptr��n��.
			0,			// ���炩�̃t���O���w�肷��B�ڂ�����D3D11_CREATE_DEVICE�񋓌^�Ō���������.
			featureLevels,	// ���͂�����D3D_FEATURE_LEVEL�񋓌^�̔z���^����Bnullptr�ɂ��邱�Ƃŏ�Lfeature�Ɠ����̓��e�̔z�񂪎g�p�����.
			ARRAYSIZE(featureLevels),			// ��L�����ŁA�����Œ�`�����z���^���Ă����ꍇ�A���̔z��̗v�f���������ɋL�q����.
			D3D11_SDK_VERSION,	// SDK�̃o�[�W�����B�K�����̒l.
			&desc,		// DXGI_SWAP_CHAIN_DESC�\���̂̃A�h���X��ݒ肷��B�����Őݒ肵���\�����ɐݒ肳��Ă���p�����[�^��SwapChain���쐬�����.
			&mSwapChain,	// �쐬�����������ꍇ�ɁA����SwapChain�̃A�h���X���i�[����|�C���^�ϐ��ւ̃A�h���X�B�����Ŏw�肵���|�C���^�ϐ��o�R��SwapChain�𑀍삷��.
			&mDevice,	// ��L�Ƃقړ��l�ŁA������ɂ�Device�̃|�C���^�ϐ��̃A�h���X��ݒ肷��.
			&level,		// ���ۂɍ쐬�ɐ�������D3D_FEATURE_LEVEL���i�[���邽�߂�D3D_FEATURE_LEVEL�񋓌^�ϐ��̃A�h���X��ݒ肷��.
			&mDeviceContext	// SwapChain��Device�Ɠ��l�ɁA������ɂ�Context�̃|�C���^�ϐ��̃A�h���X��ݒ肷��.
		);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//RenderTargetView�̐���
	{
		ID3D11Texture2D* texture2D;

		hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texture2D);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = mDevice->CreateRenderTargetView(texture2D, nullptr, &mRenderTargetView);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//DepthStencilView�̐���
	{
		ID3D11Texture2D* texture2D;
		D3D11_TEXTURE2D_DESC desc;

		desc.Width = SCREEN_WIDTH;
		desc.Height = SCREEN_HEIGHT;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = mDevice->CreateTexture2D(&desc, nullptr, &texture2D);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = mDevice->CreateDepthStencilView(texture2D, nullptr, &mDepthStencilView);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//viewport�̐ݒ�
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(SCREEN_WIDTH);
		viewport.Height = static_cast<float>(SCREEN_HEIGHT);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		mDeviceContext->RSSetViewports(1, &viewport);

	}
	//ImGui�̏�����
#ifdef USE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(mDevice.Get(), mDeviceContext.Get());

	ImGui::StyleColorsDark();

#endif
	//sprites = std::make_unique<Sprite>(mDevice.Get(), L"Data/image/���߂ꂨ��̔q�[.png");
	//primitiveRender = std::make_unique<PrimitiveRender>(mDevice.Get());
	//cube = std::make_unique<GeometricCube>(mDevice.Get());
	//sphere = std::make_unique<GeometricSphere>(mDevice.Get(), 32, 16);
	//cylinder = std::make_unique<GeometricCylinder>(mDevice.Get(), 32);
	//staticMesh = std::make_unique<StaticMesh>(mDevice.Get(), "Data/FBX/source/Dragon_ver2.fbx");
	//meshRender = std::make_unique<MeshRender>(mDevice.Get());
	//std::unique_ptr<ModelData>modelData = std::make_unique<ModelData>("Data/FBX/Deathwing.fbx", true,0);
	//std::shared_ptr<ModelResource>modelResource = std::make_shared<ModelResource>(mDevice.Get(), std::move(modelData));
	//model = std::make_unique<Model>(modelResource);
	//model->PlayAnimation(0, true);
	//modelRenderer = std::make_unique<ModelRenderer>(mDevice.Get());
	//board[0] = std::make_unique<Board>(mDevice.Get(), L"Data/image/koko.png");
	//board[1] = std::make_unique<Board>(mDevice.Get(), L"Data/image/RESULT_ani.png");
	//blend = std::make_unique<blend_state>(mDevice.Get(), BLEND_MODE::ADD);
	//camera = std::make_shared<Camera>();
	//cameraOperation = std::make_unique<CameraOperation>(camera);
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	mDeviceContext.Get()->RSGetViewports(&num_viewports, &viewport);
	pCamera.SetDefaultPerspective(30 * (3.14f / 180.f), viewport.Width / viewport.Height, 0.1f, 10000.0f);
	//camera->SetPerspective(30 * (3.14f / 180.f), viewport.Width / viewport.Height, 0.1f, 1000.0f);
	//cameraOperation->SetCameraType(CameraOperation::CAMERA_TYPE::DEBUG);
	pSceneManager.SetDevice(mDevice.Get());
	pSceneManager.ChangeScene(SCENETYPE::GAME);
	return true;
}
//�X�V
void Framework::Update(float elapsed_time)
{
	input::GamepadUpdate();
	
	//ImGui�̍X�V
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	//cameraOperation->Update(elapsed_time);
	//camera->CalculateMatrix();
	//model->UpdateAnimation(elapsed_time);
	//model->CalculateLocalTransform();
	//model->CalculateWorldTransform(DirectX::XMLoadFloat4x4(&CalculateTransform(VECTOR3F(0, 3, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 3.14f, 0))));
	pSceneManager.Update(elapsed_time);
}
//�`��
void Framework::Render(float elapsed_time)
{
	float ClearColor[4] = { .0f, .0f, .0f, 1.0f }; //red,green,blue,alpha

	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), ClearColor);
	
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	
	pSceneManager.Render(mDeviceContext.Get(), elapsed_time);
	//FLOAT4X4 view = camera->GetView();
	//DirectX::XMMATRIX V = DirectX::XMLoadFloat4x4(&view);
	//{
	//	//�J�����̐ݒ�
	//	DirectX::XMVECTOR eye, focus, up;
	//	eye = DirectX::XMVectorSet(0.0f, 0.0f, -200.0f, 1.0f);//���_
	//	focus = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);//�����_
	//	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);//��x�N�g��

	//	V = DirectX::XMMatrixLookAtLH(eye, focus, up);

	//	DirectX::XMStoreFloat4x4(&view, V);
	//}

	//FLOAT4X4 projection = camera->GetProjection();
	//DirectX::XMMATRIX P = DirectX::XMLoadFloat4x4(&projection);
	//{
	//	//��ʃT�C�Y�擾�̂��߃r���[�|�[�g���擾
	//	D3D11_VIEWPORT viewport;
	//	UINT num_viewports = 1;
	//	mDeviceContext.Get()->RSGetViewports(&num_viewports, &viewport);

	//	//�p�x�����W�A��(��)�ɕϊ�
	//	float fov_y = 30 * (3.14f / 180.f);//�p�x
	//	float aspect = viewport.Width / viewport.Height;//��ʔ䗦
	//	float near_z = 0.1f;//�\���ŋߖʂ܂ł̋���
	//	float far_z = 1000.0f;//�\���ŉ��ʂ܂ł̋���

	//	P = DirectX::XMMatrixPerspectiveFovLH(fov_y, aspect, near_z, far_z);

	//	DirectX::XMStoreFloat4x4(&projection, P);
	//}
//	const VECTOR4F light = VECTOR4F(0, 0, 1, 0);
//
//#if TYPE
//	cube->CalculateTransform(VECTOR3F(5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0));
//	sphere->CalculateTransform(VECTOR3F(0, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0));
//	cylinder->CalculateTransform(VECTOR3F(-5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0));
//	//sprites->Render(mDeviceContext.Get(), VECTOR2F(0, 0), VECTOR2F(SCREEN_WIDTH, SCREEN_HEIGHT), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
//	primitiveRender->Begin(mDeviceContext.Get(), light);
//	primitiveRender->Render(mDeviceContext.Get(), cube.get(), view, projection);
//	primitiveRender->Render(mDeviceContext.Get(), sphere.get(), view, projection);
//	primitiveRender->Render(mDeviceContext.Get(), cylinder.get(), view, projection);
//	primitiveRender->End(mDeviceContext.Get());
//#else
//	sprites->Render(mDeviceContext.Get(), VECTOR2F(0, 0), VECTOR2F(SCREEN_WIDTH/3.0f, SCREEN_HEIGHT/3.0f), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
//
//	primitiveRender->Begin(mDeviceContext.Get(), light,view,projection);
//	primitiveRender->Render(mDeviceContext.Get(), cube.get(), CalculateTransform(VECTOR3F(5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	primitiveRender->Render(mDeviceContext.Get(), sphere.get(), CalculateTransform(VECTOR3F(0, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	primitiveRender->Render(mDeviceContext.Get(), cylinder.get(), CalculateTransform(VECTOR3F(-5, 0, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	primitiveRender->End(mDeviceContext.Get());
//
//	meshRender->Begin(mDeviceContext.Get(), light, view, projection);
//	meshRender->Render(mDeviceContext.Get(), staticMesh.get(), CalculateTransform(VECTOR3F(0, -5, 0), VECTOR3F(1, 1, 1), VECTOR3F(0, 0, 0)));
//	meshRender->End(mDeviceContext.Get());
//
//	FLOAT4X4 view_projection;
//	DirectX::XMMATRIX C = DirectX::XMMatrixSet(
//		1, 0, 0, 0,
//		0, 1, 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1
//	);
//
//	DirectX::XMStoreFloat4x4(&view_projection, C * V * P);
//	modelRenderer->Begin(mDeviceContext.Get(), view_projection, light);
//	modelRenderer->Draw(mDeviceContext.Get(), *model.get());
//	modelRenderer->End(mDeviceContext.Get());
//	static float timer = 0;
//	VECTOR2F texturePosition = VECTOR2F(1920 * static_cast<int>(timer*2.0f), 0);
//	timer += elapsed_time;
//	if (timer >= 4.0f)timer = 0;
//	board[1]->Anim(mDeviceContext.Get(), texturePosition, VECTOR2F(1920, 1080));
//	blend->activate(mDeviceContext.Get());
//	board[0]->Render(mDeviceContext.Get(), VECTOR3F(20, 0, 0), VECTOR3F(10, 10, 10), VECTOR3F(DirectX::XMConvertToRadians(45), 0, 0), view, projection);
//	blend->deactivate(mDeviceContext.Get());
//	board[1]->Render(mDeviceContext.Get(), VECTOR3F(-20, 0, 0), VECTOR3F(10, 10, 10), VECTOR3F(0, 0, 0), view, projection);

//#endif
	//ImGui�̕`��
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	mSwapChain->Present(1, 0);
}

