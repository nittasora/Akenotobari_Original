#pragma once

#include "CUnit.h"

class CTornade : public CUnit
{
public:
	void Initilize() override;
	void UnInitilize() override;
	void Update() override;

private:
	// �A�j���[�V�����̍Đ����J�E���g
	int mi_anim_count;
};

