#pragma once

#include "CUnit.h"
#include "StageData.h"

class CBall : public CUnit
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

	void Destroy();
	void Create(float max_speed, float angle_z, FRONT_BACK fb);

private:
	// ���x
	float mf_max_speed;

	// ��ԕ���
	float dir_x;
	float dir_y;
};

