#pragma once
#include "CUnit.h"
class CLabel : public CUnit
{
public:
	// ����������
	void Initilize() override;

	// �I������
	void UnInitilize() override;

	// �`�揈��
	void Draw() override;
	void FakeFoldDraw() override;
};

