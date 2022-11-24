#pragma once

#include "CUnit.h"
#include "StageData.h"

class CStageManager;

class CCannon : public CUnit
{
public:
	void Initilize() override;
	void UnInitilize() override;
	void Update() override;

	// データ反映
	void SetData(CannonData* data, CStageManager* stage_manager, FRONT_BACK fb);

private:
	// 時間
	float mf_time;

	// 向き
	float mf_angle_z;

	// 玉の速さ
	float mf_max_speed;

	// 玉の発射間隔
	float mf_wait;

	// ステージ
	CStageManager* m_stage_manager;

	FRONT_BACK m_front_back;
};

