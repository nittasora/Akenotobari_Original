#include "CEditorText.h"

void CEditorText::Initilize()
{
	// �摜����
	this->AddTexture(SPRITE_TEST_CHAR_0);

	// �摜�E�R���C�_�[�T�C�Y�̓K�p
	this->SetDefaultPosAndColl(CHAR_WIDTH, CHAR_HEIGHT);
}

void CEditorText::UnInitilize()
{
}

void CEditorText::SetSprite(int id, UV uv)
{
	// �摜�ύX
	this->ChangeTexture(id);

	// uv�ݒ�
	this->m_anim = uv;
}
