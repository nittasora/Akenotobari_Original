#pragma once

#include "CScene.h"
#include "SpriteData.h"
#include "StageData.h"

enum SELECT_CONDITION
{
	SC_WORLD_PREPARE,
	SC_WORLD_ACT,
	SC_WORLD_END,
	SC_STAGE_PREPARE,
	SC_STAGE_ACT,
	SC_STAGE_END,
};

class CGameObject;
class CImage;
class CButton;
class CCursor;

class CStageSelect : public CScene
{
public:
	// コンストラクタ
	CStageSelect();

	// デストラクタ
	~CStageSelect();

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
	std::vector<CGameObject*> mvp_objects;
	std::array<CGameObject*, GAME_WORLD_MAX> map_world_button;
	std::array<CGameObject*, GAME_WORLD_MAX> map_world_frame;
	std::array<CGameObject*, GAME_STAGE_MAX> map_stage_button;
	std::array<CGameObject*, GAME_STAGE_MAX> map_stage_frame;
	std::vector<CGameObject*> mvp_piece;
	std::vector<CGameObject*> mvp_world_piece;
	std::array<std::array<CGameObject*, PIECE_NUM>, PIECE_NUM> maap_stage_piece;
	CGameObject* mp_return_button;
	CGameObject* mp_frame;
	CGameObject* mp_world_back;
	CCursor* mp_cursor;

	std::vector<SpriteData> mv_sprite_data;

	float mf_world;
	float mf_stage;
	int mi_cursor_line;
	int mi_old_cursor_line;
	float mf_old_axis;

	SELECT_CONDITION m_condition;
};

