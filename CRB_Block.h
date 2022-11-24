#pragma once

#include "CMapChip.h"

class CGameManager;

class CRB_Block : public CMapChip
{
public:
	// 初期化処理
	void Initilize() override;

	// 終了処理
	void UnInitilize() override;

	// 更新処理
	void Update() override;

	// 当たり判定
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;

	// 赤ブロックなのか
	void SetIsBlue(bool is_blue);

private:
	// 切り替え用
	bool mb_fold_switch;

	// 赤ブロックなのか青ブロックなのか
	bool mb_is_blue;

	// 切り替えフラグ
	bool mb_is_switch;

	//
	CGameManager* game_manager;
};

