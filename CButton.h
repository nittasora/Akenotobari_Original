#pragma once

#include "CImage.h"

class CButton : public CImage
{
public:
	// ������
	void Initilize() override;

	// �I��
	void UnInitilize() override;

	// �X�V
	void Update() override;

	// �����蔻��
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;

	// On��Off�����擾
	bool GetONOFF();

	// On��Off����ݒ�
	void SetONOFF(bool is_on);

	// �X�v���C�g��ݒ�
	void SetSprite(SPRITE_INDEX sprite, UV point, bool anim_on);

	// ���͂���L�[�y�у{�^���̐ݒ�
	void SetButton(int key, int button);

	// ���̃{�^����ݒ�
	void SetOtherButtonObject(CButton* other_button);

private:
	// �������A�j���[�V���������邩
	bool mb_anim_on;

	// on��off��
	bool mb_is_on;

	// �摜�̂ǂ����g����
	UV m_point;

	// �C���v�b�g�p�{�^��
	int mi_key;
	int mi_button;

	// ���̃{�^��
	std::vector<CButton*> mvp_other_buttons;
};

