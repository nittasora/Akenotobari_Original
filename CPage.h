#pragma once

#include "CUnit.h"

class CPage : public CUnit
{
public:
	// ����������
	void Initilize() override;

	// �I������
	void UnInitilize() override;

	// �`�揈��
	void Draw() override;
	void FakeFoldDraw() override;

	// �摜�ݒ�
	void SetSprite(int front_id, int back_id);
	void SetSprite(SpriteData* front, SpriteData* back);

	// �摜��UV���W�ݒ�
	void SetDefaultUV(int page_id);

};

