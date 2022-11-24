#pragma once

#include "CMapChip.h"

class CLionBottom : public CMapChip
{
public:
	// 初期化処理
	void Initilize() override;

	// 終了処理
	void UnInitilize() override;

	// 描画処理
	void Draw() override;

	// 折開き用ごまかしの描画
	void FakeFoldDraw() override;

	// 画像のUV座標設定
	void SetDefaultUV(float left, float right, float bottom, float top);
};

