#pragma once
#include "CUnit.h"
class CLabel : public CUnit
{
public:
	// 初期化処理
	void Initilize() override;

	// 終了処理
	void UnInitilize() override;

	// 描画処理
	void Draw() override;
	void FakeFoldDraw() override;
};

