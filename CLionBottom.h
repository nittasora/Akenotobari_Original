#pragma once

#include "CMapChip.h"

class CLionBottom : public CMapChip
{
public:
	// ����������
	void Initilize() override;

	// �I������
	void UnInitilize() override;

	// �`�揈��
	void Draw() override;

	// �܊J���p���܂����̕`��
	void FakeFoldDraw() override;

	// �摜��UV���W�ݒ�
	void SetDefaultUV(float left, float right, float bottom, float top);
};

