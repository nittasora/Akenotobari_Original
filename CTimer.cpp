#include "CTimer.h"
#include <mmsystem.h>  // timeGetTime�֐����g���̂ɕK�v

#pragma comment (lib, "winmm.lib")  // timeGetTime�֐����g���̂ɕK�v

CTimer::CTimer()
{
	this->m_fps = 60;
	this->m_lastframe_starttime = 0;
	this->md_deltatime = 0.0f;
	this->md_timescale = 1.0f;
}

void CTimer::SetFps(DWORD i_fps)
{
	this->m_fps = i_fps;
	this->ReachedNextFrame();
}

float CTimer::GetDeltaTime()
{
	return this->md_deltatime;
}

void CTimer::SetTimeScale(float f_timescale)
{
	if (f_timescale >= 0.0f) {
		this->md_timescale = f_timescale;
	}
}

BOOL CTimer::ReachedNextFrame()
{
	timeBeginPeriod(1);  // timeGetTime�֐���1�~���b�P�ʂŒl��Ԃ��悤�ɐݒ肷��
	DWORD currentTime = timeGetTime();  // Windows�N������̎��Ԃ��~���b�P�ʂŕԂ�
	timeEndPeriod(1);  // timeBeginPeriod�֐��Őݒ肵����Ԃ��f�t�H���g�ɖ߂�

	BOOL b_reached = currentTime - this->m_lastframe_starttime >= 1000 / this->m_fps;

	if (b_reached) {  // ���̃t���[�������s���ׂ����ԂɒB���Ă����
		this->md_deltatime = static_cast<float>(currentTime - this->m_lastframe_starttime) / 1000.0f;
		this->md_deltatime *= this->md_timescale;
		this->m_lastframe_starttime = currentTime;
	}

	return b_reached;
}

void CTimer::OnResume()
{
	this->ReachedNextFrame();
	this->md_deltatime = 16.0f / 1000.0f;
}
