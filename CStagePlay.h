#pragma once

#include "CScene.h"

class CStageManager;
class CButton;

class CStagePlay : public CScene
{
public:
	// コンストラクタ
	CStagePlay();

	// デストラクタ
	~CStagePlay();

	// 初期化処理
	// シーン開始時の初期化
	void Initialize() override final;

	// 終了処理
	// シーン終了時の初期化
	void Finalize() override final;

	// 更新処理
	void Update() override final;

	// 描画処理
	void Draw() override final;

	// ポーズ中更新処理
	void PauseUpdate() override final;

	// ポーズ中描画処理
	void PauseDraw() override final;

private:
	CStageManager* mp_stage_manager;

	float mf_time;
};

