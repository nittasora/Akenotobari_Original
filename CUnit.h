#pragma once

#include "CGameObject.h"

class CUnit : public CGameObject
{
public:
	CUnit();

	// ����������
	virtual void Initilize() override;

	// �I������
	virtual void UnInitilize() override;

	// �܊J���p���܂����̕`��
	virtual void FakeFoldDraw();

	void SetAxis(const Translate* new_axis);
	void SetFoldAngle(float new_angle);
	void SetNotRotate(bool not_rotate);
	void InitilizeFold();

	bool GetGroundFlg() override;

protected:
	// ��
	Translate m_axis;

	// ��]�p�x
	float mf_fold_angle_y;

	// ��]�ړ�����]���邩
	bool mb_not_rotate;

	// �ݒu����
	bool mb_is_ground;
	bool mb_next_is_ground;
};

