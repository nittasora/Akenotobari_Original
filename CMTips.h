#pragma once

#include "CMenuScene.h"

enum TIPS_CONDITION
{
	TC_1_1,
	TC_1_2,
	TC_MAX
};

class CImage;
class CGameObject;
class CGameManager;

class CMTips : public CMenuScene
{
public:
	// 初期化処理
	// シーン開始時の初期化
	void Initialize() override;

	// 終了処理
	// シーン終了時の初期化
	void Finalize() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Draw() override;

	// 当たり判定
	void ColliderTest();

private:
	std::vector<CGameObject*> mvp_objects;
	std::vector<CGameObject*> mvp_tips;

	TIPS_CONDITION m_condition;
	bool mb_is_act;

	int mi_count;

	CGameManager* mp_game_manager;
};

