#pragma once

#include "CMapChip.h"

class CGameManager;

class CRB_Block : public CMapChip
{
public:
	// ����������
	void Initilize() override;

	// �I������
	void UnInitilize() override;

	// �X�V����
	void Update() override;

	// �����蔻��
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;

	// �ԃu���b�N�Ȃ̂�
	void SetIsBlue(bool is_blue);

private:
	// �؂�ւ��p
	bool mb_fold_switch;

	// �ԃu���b�N�Ȃ̂��u���b�N�Ȃ̂�
	bool mb_is_blue;

	// �؂�ւ��t���O
	bool mb_is_switch;

	//
	CGameManager* game_manager;
};

