#include "CCursor.h"
#include "CInput.h"
#include "CGameManager.h"
#include "CDirect3D.h"

#include "CSingleton.h"

void CCursor::Initilize()
{
	// �摜����
	this->AddTexture(SPRITE_TEST_0);

	// ����^�O�̓J�[�\��
	this->m_tag = TAG_CURSOR;

	// �摜�E�R���C�_�[�T�C�Y�̓K�p
	this->SetDefaultPosAndColl(4.0f, 4.0f);
}

void CCursor::UnInitilize()
{
}

void CCursor::Update()
{
	// �O�񂩂�̈ړ��ʂ��擾
	POINT cursor_deff = CSingleton<CInput>::GetInstance()->GetCursorClientDefference();

	// �J�[�\�����ړ����Ă���Ȃ�
	if (cursor_deff.x > 1 || cursor_deff.x < -1 || cursor_deff.y > 1 || cursor_deff.y < -1) {

		// �J�������W�擾
		Transform camera_tr = CSingleton<CGameManager>::GetInstance()->GetTransformCamera();

		// �X�N���[���T�C�Y�擾(����W�ύX�̂���)
		UINT screen_width;
		UINT screen_height;
		CSingleton<CDirect3D>::GetInstance()->GetScreenSize(&screen_width, &screen_height);

		// �J�[�\���ʒu�̌v�Z
		POINT cursor_pos = CSingleton<CInput>::GetInstance()->GetCursorClientPosition();

		// (�J�[�\���ʒu X + �J�����ʒu X - �X�N���[���T�C�Y X �̔���)
		float cursor_x = static_cast<float>(cursor_pos.x) + camera_tr.position.x - (static_cast<float>(screen_width) / 2.0f);

		// (-�J�[�\���ʒu Y + �J�����ʒu Y - �X�N���[���T�C�Y Y �̔���)
		float cursor_y = static_cast<float>(-cursor_pos.y) + camera_tr.position.y + (static_cast<float>(screen_height) / 2.0f);

		// ���W�̃Z�b�g
		this->m_transform.position.x = cursor_x;
		this->m_transform.position.y = cursor_y;
	}
}
