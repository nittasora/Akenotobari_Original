#pragma once

#include "CUnit.h"
#include "StageData.h"

class CLionTop : public CUnit
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

	// データ反映
	void SetData(LionData* data);

private:
	enum LION_CONDITION
	{
		LION_WAIT_DOWN,
		LION_DOWN,
		LION_WAIT_UP,
		LION_UP
	};

	// 上がるスピード
	float mf_up_max_speed;

	// 下がるスピード
	float mf_down_max_speed;

	// 上にいる時、落ちるまでの待ち秒数
	float mf_up_wait;

	// 下にいる時、落ちるまでの待ち秒数
	float mf_down_wait;

	// 初期位置
	Translate m_start_pos;

	// 時間
	float mf_time;

	// 状態
	LION_CONDITION m_condition;
	LION_CONDITION m_next_condition;
};

