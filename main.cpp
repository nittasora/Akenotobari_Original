#undef UNICODE  // Unicode�ł͂Ȃ��A�}���`�o�C�g�������g��

#include <Windows.h>
#include "CMainWindow.h"
#include "UserMath.h"

#include "CSingleton.h"

//=============================================================================
// ���C���֐�
//  (WINAPI : Win32API�֐����Ăяo�����̋K��)
//  hInstance     : ���̃A�v���P�[�V�����̃C���X�^���X�n���h��(���ʎq)
//  hPrevInstance : ���NULL(16bit����̖��c��)
//  lpCmdLine     : �R�}���h���C������󂯎���������ւ̃|�C���^
//  nCmdShow      : �A�v���P�[�V�����̏����E�C���h�E�\�����@�̎w��
//=============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// return �ŕԂ��l
	int result = 0;

	//// ���[�U�[��`�̐��w�������֐�
	UserMath::Initialize();

	// ���C���E�B���h�E
	CMainWindow* main_window = CSingleton<CMainWindow>::GetInstance();

	// ���C���E�B���h�E�̏�����
	main_window->Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	// ���C�����[�v
	result = main_window->MainLoop();

	// ���C���E�B���h�E�̉��
	main_window->Finalize();

	// ���[�U�[��`�̐��w����֐�
	UserMath::Finalize();

	// Singleton�I������ 
	CSingletonFinalizer::Finalize();

	return result;
}


