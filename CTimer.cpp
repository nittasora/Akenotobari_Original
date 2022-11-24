#include "CTimer.h"
#include <mmsystem.h>  // timeGetTime関数を使うのに必要

#pragma comment (lib, "winmm.lib")  // timeGetTime関数を使うのに必要

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
	timeBeginPeriod(1);  // timeGetTime関数を1ミリ秒単位で値を返すように設定する
	DWORD currentTime = timeGetTime();  // Windows起動からの時間をミリ秒単位で返す
	timeEndPeriod(1);  // timeBeginPeriod関数で設定した状態をデフォルトに戻す

	BOOL b_reached = currentTime - this->m_lastframe_starttime >= 1000 / this->m_fps;

	if (b_reached) {  // 次のフレームを実行すべき時間に達していれば
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
