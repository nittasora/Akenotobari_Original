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

	// �f�[�^���f
	void SetData(CannonData* data, CStageManager* stage_manager, FRONT_BACK fb);

private:
	// ����
	float mf_time;

	// ����
	float mf_angle_z;

	// �ʂ̑���
	float mf_max_speed;

	// �ʂ̔��ˊԊu
	float mf_wait;

	// �X�e�[�W
	CStageManager* m_stage_manager;

	FRONT_BACK m_front_back;
};

