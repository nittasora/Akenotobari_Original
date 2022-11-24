#pragma once
#include "CUnit.h"
class CLabel : public CUnit
{
public:
	// ‰Šú‰»ˆ—
	void Initilize() override;

	// I—¹ˆ—
	void UnInitilize() override;

	// •`‰æˆ—
	void Draw() override;
	void FakeFoldDraw() override;
};

