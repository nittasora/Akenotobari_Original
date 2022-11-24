#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// エディター選択シーン
// エディターに行くか、サンプル画面に行くか、やめるかの選択

#include "CScene.h" 

class CImage;
class CButton;
class CCursor;

class CEditorSelect : public CScene
{
public:
	// コンストラクタ
	CEditorSelect();

	// デストラクタ
	~CEditorSelect();

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
	// 画像
	std::vector<CImage*> mvp_images;

	// ボタン
	std::vector<CButton*> mvp_buttons;

	// カーソル
	CCursor* mp_cursor;
};

