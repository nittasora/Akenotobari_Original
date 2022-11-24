#pragma once

#include "CUnit.h"
#include "StageData.h"

class CBall : public CUnit
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

	void Destroy();
	void Create(float max_speed, float angle_z, FRONT_BACK fb);

private:
	// 速度
	float mf_max_speed;

	// 飛ぶ方向
	float dir_x;
	float dir_y;
};

