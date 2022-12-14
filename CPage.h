#pragma once

#include "CUnit.h"

class CPage : public CUnit
{
public:
	// 初期化処理
	void Initilize() override;

	// 終了処理
	void UnInitilize() override;

	// 描画処理
	void Draw() override;
	void FakeFoldDraw() override;

	// 画像設定
	void SetSprite(int front_id, int back_id);
	void SetSprite(SpriteData* front, SpriteData* back);

	// 画像のUV座標設定
	void SetDefaultUV(int page_id);

};

