#pragma once

#include "CGameObject.h"

class CUnit : public CGameObject
{
public:
	CUnit();

	// ‰Šú‰»ˆ—
	virtual void Initilize() override;

	// I—¹ˆ—
	virtual void UnInitilize() override;

	// ÜŠJ‚«—p‚²‚Ü‚©‚µ‚Ì•`‰æ
	virtual void FakeFoldDraw();

	void SetAxis(const Translate* new_axis);
	void SetFoldAngle(float new_angle);
	void SetNotRotate(bool not_rotate);
	void InitilizeFold();

	bool GetGroundFlg() override;

protected:
	// ²
	Translate m_axis;

	// ‰ñ“]Šp“x
	float mf_fold_angle_y;

	// ‰ñ“]ˆÚ“®‰ñ“]‚·‚é‚©
	bool mb_not_rotate;

	// İ’u”»’è
	bool mb_is_ground;
	bool mb_next_is_ground;
};

