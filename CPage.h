#pragma once

#include "CUnit.h"

class CPage : public CUnit
{
public:
	// ‰Šú‰»ˆ—
	void Initilize() override;

	// I—¹ˆ—
	void UnInitilize() override;

	// •`‰æˆ—
	void Draw() override;
	void FakeFoldDraw() override;

	// ‰æ‘œİ’è
	void SetSprite(int front_id, int back_id);
	void SetSprite(SpriteData* front, SpriteData* back);

	// ‰æ‘œ‚ÌUVÀ•Wİ’è
	void SetDefaultUV(int page_id);

};

