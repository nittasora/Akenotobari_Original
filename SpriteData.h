#pragma once

#include <d3d11.h>
#include <string>
#include "ModelData.h"

// �v���C���[�摜�T�C�Y
constexpr float PLAYER_IDLE_WIDTH = 55.9f;
constexpr float PLAYER_IDLE_HEIGHT = 89.0f;
constexpr float PLAYER_IDLE_TO_RUN_WIDTH = 69.5f;
constexpr float PLAYER_IDLE_TO_RUN_HEIGHT = 89.3f;
constexpr float PLAYER_RUN_WIDTH = 60.9f;
constexpr float PLAYER_RUN_HEIGHT = 86.1f;
constexpr float PLAYER_RUN_TO_IDLE_WIDTH = 69.5f;
constexpr float PLAYER_RUN_TO_IDLE_HEIGHT = 89.2f;
constexpr float PLAYER_JUMP_UP_WIDTH = 62.4f;
constexpr float PLAYER_JUMP_UP_HEIGHT = 90.4f;
constexpr float PLAYER_JUMP_WIDTH = 55.5f;
constexpr float PLAYER_JUMP_HEIGHT = 86.4f;
constexpr float PLAYER_JUMP_DOWN_WIDTH = 62.4f;
constexpr float PLAYER_JUMP_DOWN_HEIGHT = 90.7f;
constexpr float PLAYER_HIT_WIDTH = 70.2f;
constexpr float PLAYER_HIT_HEIGHT = 91.9f;
constexpr float PLAYER_OVER_WIDTH = 97.6f;
constexpr float PLAYER_OVER_HEIGHT = 109.5f;

// �v���C���[�R���C�_�[�T�C�Y
constexpr float PLAYER_COLLIDER_WIDTH = 30.0f;
constexpr float PLAYER_COLLIDER_HEIGHT = 80.0f;

// �}�b�v�`�b�v�T�C�Y
constexpr float MAPCHIP_WIDTH = 48.0f;
constexpr float MAPCHIP_HEIGHT = 48.0f;

// �{�^���T�C�Y
constexpr float BUTTON_WIDTH = 48.0f;
constexpr float BUTTON_HEIGHT = 48.0f;

// ���C�h�{�^���T�C�Y
constexpr float BUTTON_W_WIDTH = 96.0f;
constexpr float BUTTON_W_HEIGHT = 48.0f;

// �����T�C�Y
constexpr float CHAR_WIDTH = 24.0f;
constexpr float CHAR_HEIGHT = 48.0f;

// �t���[���T�C�Y
constexpr float FRAME_WIDTH = 196.0f;
constexpr float FRAME_HEIGHT = 490.0f;

// �D�T�C�Y
constexpr float LABEL_WIDTH = 21.0f;
constexpr float LABEL_HEIGHT = 60.0f;

// �S�[���T�C�Y
constexpr float GOAL_WIDTH = 48.0f * 3.0f;
constexpr float GOAL_HEIGHT = 48.0f * 3.0f;

// �J�P���T�C�Y
constexpr float PIECE_0_WIDTH = 48.0f;
constexpr float PIECE_0_HEIGHT = 48.0f;

// �����T�C�Y
constexpr float TORNADE_WIDTH = 48.0f;
constexpr float TORNADE_HEIGHT = 144.0f;

// �_���}�T�C�Y
constexpr float DHARMA_WIDTH = 36.0f;
constexpr float DHARMA_HEIGHT = 36.0f;
constexpr float DHARMA_COLLIDER_WIDTH = MAPCHIP_WIDTH / 4.0f;
constexpr float DHARMA_COLLIDER_HEIGHT = MAPCHIP_HEIGHT / 4.0f;

// ��C�T�C�Y
constexpr float CANNON_WIDTH = 48.0f;
constexpr float CANNON_HEIGHT = 48.0f;

// �e�T�C�Y
constexpr float BALL_WIDTH = 36.0f;
constexpr float BALL_HEIGHT = 36.0f;

// ���t�g�T�C�Y
constexpr float LION_WIDTH = 144.0f;
constexpr float LION_HEIGHT = 48.0f;

// �J�P���摜��
constexpr int PIECE_NUM_WORLD = 3;
constexpr int PIECE_NUM_STAGE = 3;

// �摜�w��p
enum SPRITE_INDEX
{
	SPRITE_TEST_0,
	SPRITE_TEST_1,
	SPRITE_TEST_2,
	SPRITE_TEST_3,
	SPRITE_TEST_4,
	SPRITE_TEST_BUTTON_0,
	SPRITE_TEST_BUTTON_W_0,
	SPRITE_TEST_BUTTON_1,
	SPRITE_TEST_BUTTON_W_1,
	SPRITE_TEST_BUTTON_2,
	SPRITE_TEST_BUTTON_W_2,
	SPRITE_TEST_CHAR_0,
	SPRITE_PLAYER_IDLE_00,
	SPRITE_PLAYER_IDLE_01,
	SPRITE_PLAYER_IDLETORUN,
	SPRITE_PLAYER_RUN00,
	SPRITE_PLAYER_RUN01,
	SPRITE_PLAYER_RUNTOIDLE,
	SPRITE_PLAYER_JUMP,
	SPRITE_PLAYER_JUMP_DOWN,
	SPRITE_PLAYER_HIT,
	SPRITE_PLAYER_GAMEOVER,
	SPRITE_FRAME,
	SPRITE_LABEL,
	SPRITE_GOAL,
	SPRITE_PIECE_0,
	SPRITE_PIECE_1_in,
	SPRITE_PIECE_1_com,
	SPRITE_PIECE_2_com,
	SPRITE_RED_BULE,
	SPRITE_TORNADO,
	SPRITE_DHARMA,
	SPRITE_CANNON,
	SPRITE_BALL,
	SPRITE_LION_TOP,
	SPRITE_LION_BOTTOM,
	SPRITE_UI_HP,
	SPRITE_UI_FRAME,
	SPRITE_UI_TITLE_LOGO,
	SPRITE_UI_CURSOR,
	SPRITE_UI_BUTTON_FRAME,
	SPRITE_UI_RETURN,
	SPRITE_UI_PAGE_FRAME,
	SPRITE_UI_BOOK_FRAME,
	SPRITE_UI_BUTTON,
	SPRITE_UI_PAUSE_BACK,
	SPRITE_UI_PAUSE_HEADER,
	SPRITE_UI_SOUND_BACK,
	SPRITE_UI_SOUND_BAR,
	SPRITE_UI_SOUND_CURSOR,
	SPRITE_UI_SOUND_FRAME,
	SPRITE_UI_GAME_OVER,
	SPRITE_UI_RULE,
	SPRITE_MAX
};

// �X�v���C�g�摜��\���\����
struct SpriteData
{
	ID3D11ShaderResourceView* mSRV;	// �e�N�X�`����SRV�@�iSRV��ID�݂����Ȃ��́j
	int anim_max;					// �A�j���[�V������
	int slice_x, slice_y;			// ���c������
	UV slice;						// �ꖇ����uv
	float count_check;				// �A�j���[�V�����̑҂�����
};

// �X�v���C�g�摜���̓��o�͗p
struct IO_SpriteData
{
	std::string filename;			// �摜��
	int anim_max;					// �A�j���[�V������
	int slice_x, slice_y;			// ���c������
	float count_check;				// �A�j���[�V�����̑҂�����
};

// �X�v���C�g�摜���̓��o�͗p(�摜�T�C�Y����)
struct IO_SpriteDataEX
{
	IO_SpriteData m_io_sprite_data;		// �X�v���C�g�摜���̓��o�͗p
	float m_width;						// ����
	float m_height;						// �c��
};
