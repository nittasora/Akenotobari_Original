#pragma once

#include "CGameObject.h"

class CUnit : public CGameObject
{
public:
	CUnit();

	// 初期化処理
	virtual void Initilize() override;

	// 終了処理
	virtual void UnInitilize() override;

	// 折開き用ごまかしの描画
	virtual void FakeFoldDraw();

	void SetAxis(const Translate* new_axis);
	void SetFoldAngle(float new_angle);
	void SetNotRotate(bool not_rotate);
	void InitilizeFold();

	bool GetGroundFlg() override;

protected:
	// 軸
	Translate m_axis;

	// 回転角度
	float mf_fold_angle_y;

	// 回転移動時回転するか
	bool mb_not_rotate;

	// 設置判定
	bool mb_is_ground;
	bool mb_next_is_ground;
};

