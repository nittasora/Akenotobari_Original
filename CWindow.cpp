#include "CWindow.h"

LRESULT CWindow::StaticWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	CWindow* This = (CWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!This) {//�擾�ł��Ȃ�����(�E�B���h�E������)�ꍇ
		if (msg == WM_CREATE) {
			This = (CWindow*)((LPCREATESTRUCT)lp)->lpCreateParams;
			if (This) {
				SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)This);
				return This->WndProc(hWnd, msg, wp, lp);
			}
		}
	}
	else {//�擾�ł����ꍇ(�E�B���h�E������)
		return This->WndProc(hWnd, msg, wp, lp);
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

CWindow::CWindow()
{
}

CWindow::~CWindow()
{
}

bool CWindow::RegistWindow(const char * window_class_name, const HINSTANCE hInst)
{
	strcpy_s(this->window_class_name, window_class_name);
	m_hInstance = hInst;
	WNDCLASSEX wc;

	// �E�B���h�E�N���X�̏���ݒ�
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = CWindow::StaticWndProc;				// �E�B���h�E�v���V�[�W��
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;									// �C���X�^���X�n���h��
	wc.hIcon = (HICON)LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->window_class_name;				// �E�B���h�E�N���X��

	// �E�B���h�E�N���X��o�^����
	if (RegisterClassEx(&wc) == 0) { return false; }

	return true;
}

LRESULT CWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}

HWND CWindow::GetHWnd()
{
	return this->m_hwnd;
}

HINSTANCE CWindow::GetHInstance()
{
	return this->m_hInstance;
}

bool CWindow::nuCreateWindow(DWORD dwExStyle, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
	m_hwnd = CreateWindowEx(
		dwExStyle,					// �g���E�B���h�E�X�^�C��
		this->window_class_name,	// �E�B���h�E�N���X��
		lpWindowName,				// �^�C�g���o�[�ɕ\�����镶����
		dwStyle,					//  �E�B���h�E�̎��
		x,							// �E�B���h�E��\������ʒu�iX���W�j
		y,							// �E�B���h�E��\������ʒu�iY���W�j
		nWidth,						// �E�B���h�E�̕�
		nHeight,					// �E�B���h�E�̍���
		hWndParent,					// �e�E�B���h�E�̃E�B���h�E�n���h��
		hMenu,						// ���j���[�n���h��
		m_hInstance,				// �C���X�^���X�n���h��
		this						// ���̑��̍쐬�f�[�^
	);
	if (m_hwnd == nullptr)
		return false;

	return true;
}
