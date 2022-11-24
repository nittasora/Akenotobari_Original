#pragma once

#include <time.h>
#include <Windows.h>

class CTimer
{
public:
	CTimer();

	// m_fps�ɒl��n��
	void SetFps(DWORD i_fps);

	// �f���^�^�C�����擾
	float GetDeltaTime();

	// �f���^�^�C���̑傫����ݒ�
	void SetTimeScale(float f_timescale);

	// �ݒ肳�ꂽFPS�����ƂɁA���̃t���[�������s���鎞�Ԃɓ��B������TRUE��Ԃ�
	BOOL ReachedNextFrame();

	// �Q�[�����[�v��~���畜�A����ۂɌĂяo���֐�
	void OnResume();

private:
	// fps�l
	DWORD m_fps;

	// 1�O�̃t���[�����s�J�n���̎��Ԃ�ۑ�����ϐ�
	DWORD m_lastframe_starttime;

	// �f���^�^�C��
	float md_deltatime;

	// �f���^�^�C���̑傫��
	float md_timescale;
};

