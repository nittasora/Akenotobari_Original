#pragma once

#include "CImage.h"

class CCursor : public CImage
{
public:
	// ����������
	void Initilize() override;

	// �I������
	void UnInitilize() override;

	// �X�V����
	void Update() override;
};

