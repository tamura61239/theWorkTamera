#include "framework.h"
#include"screen_size.h"
#include"blend_state.h"
#include"scene_manager.h"
#include"camera_manager.h"
#include"gamepad.h"
#include<time.h>
Framework* Framework::mInst = nullptr;
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�������֐�
/*****************************************************/

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
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
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
			&mLevel,		// ���ۂɍ쐬�ɐ�������D3D_FEATURE_LEVEL���i�[���邽�߂�D3D_FEATURE_LEVEL�񋓌^�ϐ��̃A�h���X��ݒ肷��.
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
	ImGuiIO& io = ImGui::GetIO(); /*(void)io;*/

	io.Fonts->AddFontFromFileTTF("Data\\ipag.ttf", 17.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

#endif
	//�V�[���̏�����
	pSceneManager.Initialize(mDevice.Get());
	return true;
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�X�V�֐�
/*****************************************************/
void Framework::Update(float elapsed_time)
{
	input::GamepadUpdate();
	
	//ImGui�̍X�V
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
	//rand�֐��̍X�V
	srand((unsigned int)time(NULL));
	//�V�[���̍X�V
	pSceneManager.Update(elapsed_time,mDevice.Get());
}
/*****************************************************/
//�@�@�@�@�@�@�@�@�@�@�`��֐�
/*****************************************************/
void Framework::Render(float elapsed_time)
{
	float ClearColor[4] = { .0f, .0f, .0f, 1.0f }; //red,green,blue,alpha
	//�����_�[�^�[�Q�b�g�Ȃǂ̏�����
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), ClearColor);
	
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	//�V�[���̕`��
	pSceneManager.Render(mDeviceContext.Get(), elapsed_time);
	//ImGui�̕`��
#ifdef USE_IMGUI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
	//fps�Œ肩�ǂ��������߂�
	mSwapChain->Present(0, 0);
}

