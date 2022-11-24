#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// エディター用のサンプルシーン
// これ以降通常のサンプルシーンは使用しない。
// 通常に比べ、上部にもどるボタンを追加。

#include "CScene.h"

class CStageManager;
class CButton;

class CEditorSample : public CScene
{
public:
	// コンストラクタ
	CEditorSample();

	// デストラクタ
	~CEditorSample();

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

	// 当たり判定
	void ColliderTest();

private:
	// ステージマネージャーインスタンス
	CStageManager* mp_stage_manager;

	// ボタン
	std::vector<CButton*> mvp_button;
};

