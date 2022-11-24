#pragma once

#include "CUnit.h"
#include "StageData.h"

class CDharma : public CUnit
{
public:
	// ����������
	void Initilize() override;

	// �I������
	void UnInitilize() override;

	// �X�V����
	void Update() override;

	// �`�揈��
	void Draw() override;
	void FakeFoldDraw() override;

	// �����蔻��
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;

	// �f�[�^���f
	void SetData(DharmaData* data);

private:
	// �ړ��X�s�[�h
	float mf_max_speed;

	// �ǂ����������Ă��邩
	bool mb_is_left;

	// �d��
	float mf_gravity;

	// ��������
	bool mb_is_tornade;
	bool mb_next_is_tornade;

	// ���]����
	bool mb_is_not_turn;
	bool mb_is_hit_border;

	Collider m_body_collider;
	Collider m_wall_checker;
	Collider m_hole_checker;

};

