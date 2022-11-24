#pragma once

#include <vector>

class CDirect3D;
class CXAudio2;
class CInput;
class CTimer;

enum MENU_CONDITION
{
	MENU_FADE_IN_PREPARE,
	MENU_FADE_IN_ACT,
	MENU_FADE_IN_END,
	MENU_PREPARE,
	MENU_ACT,
	MENU_END,
	MENU_FADE_OUT_PREPARE,
	MENU_FADE_OUT_ACT,
	MENU_FADE_OUT_END,
};

class CMenuScene
{
public:
	// コンストラクタ
	CMenuScene();

	// デストラクタ
	~CMenuScene();

	// 初期化処理
	// シーン開始時の初期化
	virtual void Initialize();

	// 終了処理
	// シーン終了時の初期化
	virtual void Finalize();

	// 更新処理
	virtual void Update();

	// 描画処理
	virtual void Draw();

	void SetCondition(MENU_CONDITION next);

protected:
	// CDirectXのインスタンス
	CDirect3D* mp_directx;

	// CXAudio2のインスタンス
	CXAudio2* mp_xaudio2;

	// CInputのインスタンス
	CInput* mp_input;

	// CTimerのインスタンス
	CTimer* mp_timer;

	// 状況
	MENU_CONDITION m_current_condition;
	MENU_CONDITION m_next_condition;
};

