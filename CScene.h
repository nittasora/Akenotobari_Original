#pragma once

#include <vector>
#include <mutex>

class CDirect3D;
class CXAudio2;
class CInput;
class CTimer;

class CScene
{
public:
	// コンストラクタ
	CScene();

	// デストラクタ
	~CScene();

	// 初期化処理
	// シーン開始時の初期化
	virtual void Initialize() = 0;

	// 終了処理
	// シーン終了時の初期化
	virtual void Finalize() = 0;

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Draw() = 0;

	// ポーズ中更新処理
	virtual void PauseUpdate() = 0;

	// ポーズ中描画処理
	virtual void PauseDraw() = 0;

	//bool GetInitializedFlag();
	//void SetInitializedFlag(bool flg);

protected:
	// 初期化済みフラグ
	bool mb_initialized_flg;

	// CDirectXのインスタンス
	CDirect3D* mp_directx;

	// CXAudio2のインスタンス
	CXAudio2* mp_xaudio2;

	// CInputのインスタンス
	CInput* mp_input;

	// CTimerのインスタンス
	CTimer* mp_timer;
};

