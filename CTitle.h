#pragma once

#include "CScene.h"
#include "SpriteData.h"

class CGameObject;
class CImage;
class CButton;
class CCursor;
class CEditorManager;

class CTitle : public CScene
{
public:
	// コンストラクタ
	CTitle();

	// デストラクタ
	~CTitle();

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
	void CommonUpdate();

	std::vector<CGameObject*> mvp_objects;
	CCursor* mp_cursor;

	int mi_world_num;

	int mi_cursor_line;
	int mi_old_cursor_line;
	float mf_old_axis;

	std::vector<CGameObject*> mvp_back;
	std::vector<CGameObject*> mvp_chip;
	std::vector<CGameObject*> mvp_button;
	std::vector<CGameObject*> mvp_cursor;
	std::vector<CGameObject*> mvp_frame;
	CGameObject* mp_player;

	std::vector<SpriteData> mv_sprite_data;

	CEditorManager* mp_editor_manager;
};

