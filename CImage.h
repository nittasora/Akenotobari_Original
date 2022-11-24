#pragma once

#include "CGameObject.h"

class CImage : public CGameObject
{
public:
	// 初期化処理
	virtual void Initilize() override;

	// 終了処理
	virtual void UnInitilize() override;

	// 描画処理
	void Draw() override;

	// 場所の更新
	void UpdateTransform() override;

	// 場所で大きさ設定
	void SetImage(float left_x, float right_x, float bottom_y, float top_y);
	void SetImageAndColl(float left_x, float right_x, float bottom_y, float top_y);

	// 画像のUV座標設定
	void SetDefaultUV(float left, float right, float bottom, float top);
};

