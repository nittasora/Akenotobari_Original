#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// ���j���[�̊Ǘ�

#include <stack>

#include "MenuData.h"
#include "CMenuScene.h"

class CMenuManager
{
public:
	CMenuManager();
	~CMenuManager();

	// ����������
	void Initialize();

	// �I������
	void Finalize();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// ���j���[�ύX
	void ChangeMenuScene(MENU_INDEX index);

	// ���j���[�ύX���APause��Ԃ�ς���
	void ChangeMenuSceneEX(MENU_INDEX index, bool is_not_pause);

	// ���j���[���ЂƂ߂�
	void BackMenuScene();

	// ���j���[����߂�A���j���[�����Ȃ�Pause��Ԃ�ς���
	void BackMenuSceneEX(bool is_not_pause);

	// �ЂƂO�̃��j���[�����ׂď���
	void EraseOldIndex();

private:
	//=============================
	// �V�[���Ǘ��p
	//=============================

	// �V�[�����܂Ƃ߂�p
	CMenuScene* mpa_scenes[MENU_INDEX_MAX];

	// �O�̃V�[���C���f�b�N�X
	std::stack<MENU_INDEX> m_old_scene_index;

	// ���݂̃V�[���C���f�b�N�X
	MENU_INDEX m_current_scene_index;
};

