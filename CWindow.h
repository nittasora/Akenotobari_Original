#pragma once

#include <Windows.h>

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// WindowsAPI�ɂ��E�B���h�E�쐬�����s���N���X
// ��{�I�ɐG�邱�Ƃ͂Ȃ�

class CWindow
{
private:
	char window_class_name[1024];

	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

protected:

	HWND m_hwnd;

	HINSTANCE m_hInstance;

public:
	CWindow();
	virtual ~CWindow();

	// �E�B���h�E��`�p�����o�֐�
	bool RegistWindow(const char* window_class_name, const HINSTANCE hInst);

	// �E�B���h�E�v���V�[�W��
	virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	// �E�B���h�E�n���h���擾
	HWND GetHWnd();

	// �C���X�^���X�擾
	HINSTANCE GetHInstance();

	// �E�B���h�E�쐬�p�����o�֐�
	bool nuCreateWindow(
		DWORD dwExStyle,		// �g���E�B���h�E�X�^�C��
		LPCTSTR lpWindowName,	// �E�B���h�E��
		DWORD dwStyle,			// �E�B���h�E�X�^�C��
		int x,					// �E�B���h�E�̉������̈ʒu
		int y,					// �E�B���h�E�̏c�����̈ʒu
		int nWidth,				// �E�B���h�E�̕�
		int nHeight,			// �E�B���h�E�̍���
		HWND hWndParent,		// �e�E�B���h�E�܂��̓I�[�i�[�E�B���h�E�̃n���h��
		HMENU hMenu				// ���j���[�n���h���܂��͎q�E�B���h�E ID
	);
};

