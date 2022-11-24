#pragma once

#include "CGameObject.h"

class CImage : public CGameObject
{
public:
	// ����������
	virtual void Initilize() override;

	// �I������
	virtual void UnInitilize() override;

	// �`�揈��
	void Draw() override;

	// �ꏊ�̍X�V
	void UpdateTransform() override;

	// �ꏊ�ő傫���ݒ�
	void SetImage(float left_x, float right_x, float bottom_y, float top_y);
	void SetImageAndColl(float left_x, float right_x, float bottom_y, float top_y);

	// �摜��UV���W�ݒ�
	void SetDefaultUV(float left, float right, float bottom, float top);
};

