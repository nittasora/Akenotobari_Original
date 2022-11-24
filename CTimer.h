#pragma once

#include <time.h>
#include <Windows.h>

class CTimer
{
public:
	CTimer();

	// m_fpsに値を渡す
	void SetFps(DWORD i_fps);

	// デルタタイムを取得
	float GetDeltaTime();

	// デルタタイムの大きさを設定
	void SetTimeScale(float f_timescale);

	// 設定されたFPSをもとに、次のフレームを実行する時間に到達したらTRUEを返す
	BOOL ReachedNextFrame();

	// ゲームループ停止から復帰する際に呼び出す関数
	void OnResume();

private:
	// fps値
	DWORD m_fps;

	// 1つ前のフレーム実行開始時の時間を保存する変数
	DWORD m_lastframe_starttime;

	// デルタタイム
	float md_deltatime;

	// デルタタイムの大きさ
	float md_timescale;
};

