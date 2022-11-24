#pragma once

#include "CUnit.h"
#include "StageData.h"

class CDharma : public CUnit
{
public:
	// 初期化処理
	void Initilize() override;

	// 終了処理
	void UnInitilize() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Draw() override;
	void FakeFoldDraw() override;

	// 当たり判定
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;

	// データ反映
	void SetData(DharmaData* data);

private:
	// 移動スピード
	float mf_max_speed;

	// どっちを向いているか
	bool mb_is_left;

	// 重力
	float mf_gravity;

	// 竜巻判定
	bool mb_is_tornade;
	bool mb_next_is_tornade;

	// 反転判定
	bool mb_is_not_turn;
	bool mb_is_hit_border;

	Collider m_body_collider;
	Collider m_wall_checker;
	Collider m_hole_checker;

};

