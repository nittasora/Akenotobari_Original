#include "CMainWindow.h"
#include "CTimer.h"
#include "CInput.h"
#include "CDirect3D.h"
#include "CXAudio2.h"
#include "CSceneManager.h"

#include "CSingleton.h"

// �E�B���h�E�̌`��
#define WINDOW_CLASS WS_POPUPWINDOW

//WS_OVERLAPPEDWINDOW

// �E�B���h�E�̏������W
constexpr int SCREEN_POS_X = 0;					// �E�C���h�E�̏��� X �ʒu
constexpr int SCREEN_POS_Y = 0;					// �E�C���h�E�̏��� Y �ʒu

// �N���C�A���g�̈�̕�
constexpr int CLIENT_WIDTH = 1500;					// �N���C�A���g�̈�̉���
constexpr int CLIENT_HEIGHT = 800;					// �N���C�A���g�̈�̏c��

//=================================================
// �|�b�v�A�b�v�E�B���h�E�̏ꍇ�̃E�B���h�E�T�C�Y
//=================================================

#if WINDOW_CLASS == WS_POPUPWINDOW

#if defined(_DEBUG)

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 2;		// �E�C���h�E�̕�
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 2;	// �E�C���h�E�̕�

#else

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 2;		// �E�C���h�E�̕�
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 2;	// �E�C���h�E�̕�

#endif

//=================================================
// �ʏ�E�B���h�E�̏ꍇ�̃E�B���h�E�T�C�Y
//=================================================

#else

#if defined(_DEBUG)

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 16;		// �E�C���h�E�̕�
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 39;	// �E�C���h�E�̕�

#else

constexpr int SCREEN_WIDTH = CLIENT_WIDTH + 16;		// �E�C���h�E�̕�
constexpr int SCREEN_HEIGHT = CLIENT_HEIGHT + 39;	// �E�C���h�E�̕�

#endif

#endif

const char* CLASS_NAME = "��̒�";	// �E�C���h�E�N���X�̖��O
const char* WINDOW_NAME = "��̒�";	// �E�B���h�E�̖��O

// �g���E�B���h�E�X�^�C��
constexpr DWORD WINDOW_EX_STYLE = 0;

// �E�B���h�E�X�^�C��
constexpr DWORD WINDOW_STYLE = ((((WINDOW_CLASS | WS_VISIBLE) & ~WS_MINIMIZEBOX) & ~WS_MAXIMIZEBOX) & ~WS_SIZEBOX);

LRESULT CMainWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	int sts;
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_KEYDOWN:  // �����L�[�������ꂽ��
		//switch (wp)
		//{
		//	//case VK_ESCAPE:  // ESC�L�[��������
		//	//	int sts = MessageBoxA(hWnd, "�I�����܂���?", "�I���m�F", MB_YESNO);
		//	//	if (sts == IDYES) {
		//	//		DestroyWindow(hWnd);  // �E�C���h�E���I��������֐��Ăяo��
		//	//		this->mb_mainloop_is_continue = false;
		//	//		return 0;
		//	//	}
		//	//	else {
		//	//		return 0;
		//	//	}
		//	//	break;
		//}
		break;
	case WM_CLOSE:  // x�{�^���������ꂽ��
		sts = MessageBoxA(hWnd, "�I�����܂���?", "�I���m�F", MB_YESNO);
		if (sts == IDYES) {
			DestroyWindow(hWnd);  // �E�C���h�E���I��������֐��Ăяo��
			this->mb_mainloop_is_continue = false;
			return 0;
		}
		else {
			return 0;
		}
		break;
	case WM_ACTIVATE:  // �E�C���h�E���A�N�e�B�u�܂��͔�A�N�e�B�u�ɕω������ꍇ
		if (LOWORD(wp) != WA_INACTIVE) {  // �A�N�e�B�u�ɂȂ����ꍇ
			CSingleton<CTimer>::GetInstance()->OnResume();
		}
		break;

	case WM_EXITSIZEMOVE: // �E�C���h�E�̃T�C�Y�ύX�܂��͈ړ������������ꍇ
		CSingleton<CTimer>::GetInstance()->OnResume();
		break;
	case WM_DESTROY:
		return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

void CMainWindow::Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	//=============================
	// WinMain�̏��i�[
	//=============================

	this->mh_instance = hInstance;
	this->mh_previnstance = hPrevInstance;
	this->mlp_cmdline = lpCmdLine;
	this->n_cmdshow = nCmdShow;

	//=============================
	// �t���O�̏�����
	//=============================

	this->mb_initialize_error = false;		// �G���[�Ȃ��Ƃ���
	this->mb_mainloop_is_continue = true;	// ���[�v�͑�������̂Ƃ���

	//=============================
	// �E�B���h�E�쐬�Ɠo�^
	//=============================

	this->RegistWindow(CLASS_NAME, this->mh_instance);
	this->nuCreateWindow(WINDOW_EX_STYLE, WINDOW_NAME, WINDOW_STYLE, SCREEN_POS_X, SCREEN_POS_Y, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr);

	//=============================
	// �C���X�^���X�擾�֌W
	//=============================

	CDirect3D* cdirect3d = CSingleton<CDirect3D>::GetInstance();
	CXAudio2* cxaudio = CSingleton<CXAudio2>::GetInstance();

	//=============================
	// �C���X�^���X�Q��������
	//=============================

	// ���۔���p
	HRESULT hr;

	// CDirect3D�̏�����
	hr = cdirect3d->Initialize(this->m_hwnd);
	if (this->ErrorCheck(hr, "Direct3D�̏������Ɏ��s")) return;

	// ��ʃN���A
	FLOAT clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	cdirect3d->GetContext()->ClearRenderTargetView(cdirect3d->GetRenderTargetView(), clear_color);

	// ��ʐ؂�ւ�
	cdirect3d->GetSwapChain()->Present(0, 0);

	// CXAudio2�̏�����
	hr = cxaudio->Initialize();
	if (this->ErrorCheck(hr, "XAudio2�̏������Ɏ��s")) return;

	//=============================
	// �N���C�A���g�̈敝�̊i�[
	//=============================

	RECT rect;
	if (GetClientRect(this->m_hwnd, &rect)) {
		this->m_client_size.width = rect.right;
		this->m_client_size.height = rect.bottom;
	}

	// �E�C���h�E�̕\��
	ShowWindow(this->m_hwnd, nCmdShow);	// �w�肳�ꂽ�E�B���h�E�̕\����Ԃ�ݒ�(�E�B���h�E��\��)
	UpdateWindow(this->m_hwnd);			// �E�B���h�E�̏�Ԃ𒼂��ɔ��f(�E�B���h�E�̃N���C�A���g�̈���X�V)

}

int CMainWindow::MainLoop()
{
	// �G���[���������[�v���s��Ȃ�
	if (this->mb_initialize_error) return 0;

	// ���b�Z�[�W���
	MSG msg;

	CTimer* timer = CSingleton<CTimer>::GetInstance();
	CInput* input = CSingleton<CInput>::GetInstance();
	CXAudio2* cxaudio = CSingleton<CXAudio2>::GetInstance();
	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	scene_manager->Initialize();

	timer->OnResume();

	// ���b�Z�[�W���[�v�@���@�Q�[���̃��C�����[�v
	// ��P�����F���b�Z�[�W���
	// ��Q�����F�E�B���h�E�̃n���h��
	// ��R�����F�ŏ��̃��b�Z�[�W
	// ��S�����F�Ō�̃��b�Z�[�W
	while (this->mb_mainloop_is_continue) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message == WM_QUIT) {
				break;
			}
			else {
				// �Ăяo�����X���b�h�̃��b�Z�[�W�L���[���烁�b�Z�[�W���擾
				TranslateMessage(&msg);					// ���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�֕ϊ�
				DispatchMessage(&msg);					// �E�B���h�E�v���V�[�W���փ��b�Z�[�W�𑗏o
			}
		}
		else {  // ���b�Z�[�W�������������Ă��Ȃ���
			// ���@�Q�[���Ɋւ���v���O�����������ꏊ

			if (timer->ReachedNextFrame() == TRUE) {

				// ���͍X�V
				input->Update();
				input->UpdateCursorPos();
				input->UpdateClientCursorPos(this->m_hwnd);

				// ���y�X�V
				cxaudio->Update();

				scene_manager->Update();

				if (!scene_manager->CheckLoopContinue()) {
					this->mb_mainloop_is_continue = false;
				}
			}

			// ���@�Q�[���Ɋւ���v���O�����������ꏊ
		}
	}

	scene_manager->Finalize();

	return (int)msg.wParam;
}

void CMainWindow::Finalize()
{
	//=============================
	// �C���X�^���X����֌W
	//=============================

}

bool CMainWindow::ErrorCheck(HRESULT hr, LPCSTR lpText)
{
	// �G���[����
	if (FAILED(hr)) {
		// �x�������o��(����҂������[�U�[����)
		MessageBoxA(this->m_hwnd, lpText, "�G���[", MB_OK | MB_ICONERROR);
		this->mb_initialize_error = true;
		this->mb_mainloop_is_continue = false;
		return true;
	}

	return false;
}

bool CMainWindow::TextBox(LPCSTR lpText)
{
	// ���b�Z�[�W�{�b�N�X
	int result = MessageBoxA(this->m_hwnd, lpText, "�m�F", MB_YESNO | MB_ICONWARNING);
	return result == IDYES;
}
