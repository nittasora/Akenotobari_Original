#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// ステージエディタ―

#include "CScene.h"

#include "StageData.h"

class CImage;
class CButton;
class CCursor;
class CPage;
class CPlayer;
class CEditorText;

class CStageEditor : public CScene
{
public:
	// コンストラクタ
	CStageEditor();

	// デストラクタ
	~CStageEditor();

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
	// ステージの読み込み
	void LoadStage();

	// ステージの保存
	void SaveStage();

	// ステージの更新
	void UpdateStage();

	// 画像オブジェクトの生成
	void CreateImage();

	// 画像の追加
	void AddSprite();

	// 画像の変更
	void ChangeSprite(int id);

	// プレイヤーの移動
	void MovePlayer();

	// ギミックの更新
	void UpdateGoal(std::vector<GoalData> data);
	void UpdatePiece(std::vector<PieceData> data);
	void UpdateTornade(std::vector<TornadeData> data);
	void UpdateDharma(std::vector<DharmaData> data);
	void UpdateCannon(std::vector<CannonData> data);
	void UpdateLion(std::vector<LionData> data);

	// ギミックの情報の入力
	void InputDharmaData(DharmaData* data);
	void InputCannonData(CannonData* data);
	void InputLionTopData(LionData* data);

	// 通常時の更新・描画処理
	void NormalUpdate();
	void NormalColliderTest();
	void NormalDraw();

	// 画像設定変更の更新・描画処理
	void SpriteOptionUpdate();
	void SpriteOptionColliderTest();
	void SpriteOptionDraw();

	// マップチップ選択の更新・描画処理
	void MapChipSelectUpdate();
	void MapChipSelectColliderTest();

	// マップチップ設定の更新・描画処理
	void MapChipUpdate();
	void MapChipColliderTest();

	// 背景設定の更新・描画処理
	void BackSelectUpdate();
	void BackSelectColliderTest();

	// ギミック選択の更新・描画処理
	void GimmikSelectUpdate();
	void GimmikSelectColliderTest();
	void GimmikSelectDraw();

	// ギミック設定の更新・描画処理
	void GimmikUpdate();
	void GimmikColliderTest();

	// 画像
	std::vector<CImage*> mvp_images;

	// マップチップの表示用と選択用
	std::vector<CImage*> mvp_mapchip;
	std::vector<CImage*> mvp_mapchip_image;

	// 文字
	std::vector<CEditorText*> mvp_text;

	// 数字
	std::vector<CEditorText*> mvp_value_text;

	// ステージの背景
	std::vector<CPage*> mvp_pages;

	// プレイヤー
	std::vector<CPlayer*> mvp_player;

	// メイン画面のボタン
	std::vector<CButton*> mvp_main_buttons;

	// 選択画面のボタン
	std::vector<CButton*> mvp_select_buttons;

	// カーソルの画像
	std::vector<CCursor*> mvp_cursor_image;

	// ギミックの表示用と選択用
	std::vector<CImage*> mvp_gimmik_image;
	std::vector<CImage*> mvp_goal;
	std::vector<CImage*> mvp_piece;
	std::vector<CImage*> mvp_tornado;
	std::vector<CImage*> mvp_dharma;
	std::vector<CImage*> mvp_cannon;
	std::vector<CImage*> mvp_lion;

	// カーソル
	CCursor* mp_cursor;
};

