#pragma once

#include "CUnit.h"
#include "StageData.h"

class CLionTop : public CUnit
{
public:
	// ����������
	void Initilize() override;

	// �I������
	void UnInitilize() override;

	// �X�V����
	void Update() override;

	// �����蔻��
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;

	// �f�[�^���f
	void SetData(LionData* data);

private:
	enum LION_CONDITION
	{
		LION_WAIT_DOWN,
		LION_DOWN,
		LION_WAIT_UP,
		LION_UP
	};

	// �オ��X�s�[�h
	float mf_up_max_speed;

	// ������X�s�[�h
	float mf_down_max_speed;

	// ��ɂ��鎞�A������܂ł̑҂��b��
	float mf_up_wait;

	// ���ɂ��鎞�A������܂ł̑҂��b��
	float mf_down_wait;

	// �����ʒu
	Translate m_start_pos;

	// ����
	float mf_time;

	// ���
	LION_CONDITION m_condition;
	LION_CONDITION m_next_condition;
};

