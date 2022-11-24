#pragma once

#include "CImage.h"

class CPlayerRunImage : public CImage
{
public:
	void Initilize() override;
	void UnInitilize() override;
	void Update() override;

private:
	// 画像指定用の定数
	enum PLAYER_ANIM
	{
		PA_RUN00,
		PA_RUN01,
	};

	// アニメーションの再生数カウント
	int mi_anim_count;
};

