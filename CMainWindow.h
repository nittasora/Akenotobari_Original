#pragma once

#include "CWindow.h"

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// WindowsAPI�ɂ��E�B���h�E�쐬�����s���N���X
// ��main.cpp���̏���������

class CMainWindow : public CWindow
{
public:
	//=============================
	// �����֌W
	//=============================

	// ���C���E�B���h�E�̃E�B���h�E�v���b�N�֐�
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) override;

	// ����������
	void Initialize(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

	// ���C�����[�v����
	int MainLoop();

	// �I������
	void Finalize();

	// �G���[�`�F�b�N�p
	bool ErrorCheck(HRESULT hr, LPCSTR lpText);
	bool TextBox(LPCSTR lpText);

private:
	// WinMain�ɓn�����ϐ��Q
	HINSTANCE mh_instance;
	HINSTANCE mh_previnstance;
	LPSTR mlp_cmdline;
	int n_cmdshow;

	// �������G���[���ʗp
	bool mb_initialize_error = true;

	// ���C�����[�v���������邩�ǂ���
	bool mb_mainloop_is_continue = false;;

	// �N���C�A���g�T�C�Y�p
	struct {
		LONG width;
		LONG height;
	}m_client_size;

};
