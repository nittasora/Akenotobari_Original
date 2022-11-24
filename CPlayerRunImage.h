#pragma once

#include "CImage.h"

class CPlayerRunImage : public CImage
{
public:
	void Initilize() override;
	void UnInitilize() override;
	void Update() override;

private:
	// �摜�w��p�̒萔
	enum PLAYER_ANIM
	{
		PA_RUN00,
		PA_RUN01,
	};

	// �A�j���[�V�����̍Đ����J�E���g
	int mi_anim_count;
};

