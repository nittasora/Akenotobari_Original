#pragma once

enum ObjectTag {
	// �f�t�@���g�^�O
	TAG_DEFAULT,

	// �J�[�\���p�^�O
	TAG_CURSOR,

	// �v���C���[�p�^�O
	TAG_PLAYER,

	// �}�b�v�`�b�v�p�^�O
	TAG_MAPCHIP,

	// �����Ȃ��}�b�v�`�b�v
	TAG_INVISIBLE_BLOCK,

	// �X�e�[�W���E��
	TAG_BORDER_LEFT,
	TAG_BORDER_RIGHT,
	TAG_BORDER_TOP,
	TAG_BORDER_BOTTOM,

	// �y�[�W�̋��E
	TAG_BORDER_PAGE,

	// �S�[��
	TAG_GOAL,

	// �J�P��
	TAG_PIECE_1,
	TAG_PIECE_2,
	TAG_PIECE_3,

	// �ԃu���b�N
	TAG_RED_BLOCK,

	// �u���b�N
	TAG_BLUE_BLOCK,

	// ����
	TAG_TORNADO,

	// �_���}
	TAG_DHARMA,

	// ��C
	TAG_CANNON,

	// �e
	TAG_BALL,

	// ���t�g
	TAG_LION_TOP,
	TAG_LION_BOTTOM,
};

// �I�u�W�F�N�g�̃t���O�p�ϐ�
using obj_flg = int;

// �I�u�W�F�N�g�������Ă��邩
constexpr obj_flg OBJ_FLG_ACTIVE = (1 << 0);

// �I�u�W�F�N�g���`�悳��邩
constexpr obj_flg OBJ_FLG_DRAW = (1 << 1);
