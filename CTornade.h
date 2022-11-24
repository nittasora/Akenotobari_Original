#pragma once

#include "CUnit.h"

class CTornade : public CUnit
{
public:
	void Initilize() override;
	void UnInitilize() override;
	void Update() override;

private:
	// アニメーションの再生数カウント
	int mi_anim_count;
};

