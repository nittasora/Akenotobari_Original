#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// �G�f�B�^�[�Ŏg�p���镶���̉摜�B
// ���ߑł���uv��ύX����
//
// ���Q�[���{�̂Ŏg���ꍇ�́A�ŐV��CImage�𐄏�

#include "CImage.h"

class CEditorText : public CImage
{
public:
	// ����������
	virtual void Initilize() override;

	// �I������
	virtual void UnInitilize() override;

	// �摜�ݒ�
	void SetSprite(int id, UV uv);
};

