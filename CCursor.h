#pragma once

#include "CImage.h"

class CCursor : public CImage
{
public:
	// 初期化処理
	void Initilize() override;

	// 終了処理
	void UnInitilize() override;

	// 更新処理
	void Update() override;
};

