#pragma once

// �V�[���̗v�f�ԍ�
enum SCENE_INDEX
{
	// �Q�[���V�[��

	//SCENE_SAMPLE,							// �T���v���V�[���C���f�b�N�X
	SCENE_TITLE,							// �^�C�g���V�[���C���f�b�N�X
	SCENE_STAGE_SELECT,						// �X�e�[�W�I���V�[���C���f�b�N�X
	SCENE_STAGE_PLAY,						// �Q�[���V�[���C���f�b�N�X

	// �G�f�B�^�[�V�[��

	SCENE_EDITOR_SELECT,					// �G�f�B�^�[�I���V�[��
	SCENE_STAGE_EDITOR,						// �X�e�[�W�G�f�B�^�[�V�[��
	SCENE_EDITOR_SAMPLE,					// �G�f�B�^�[�̃T���v���V�[��

	SCENE_INDEX_MAX,						// �V�[���̐�
	SCENE_INDEX_NONE = SCENE_INDEX_MAX		// ���̃V�[�����Ȃ����
};

// �ŏ��̃V�[��
constexpr SCENE_INDEX START_SCENE_INDEX = SCENE_TITLE;