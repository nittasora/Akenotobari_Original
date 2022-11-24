#pragma once

#include <vector>
#include <array>
#include "ObjectData.h"
#include "ModelData.h"

enum FRONT_BACK
{
	IS_FRONT,
	IS_BACK,
	FRONT_BACK_MAX
};

enum LEFT_RIGHT
{
	IS_LEFT,
	IS_RIGHT,
	LEFT_RIGHT_MAX
};

// �G���A��
constexpr int AREA_NUM = 7;

// 1�G���A���̃}�b�v�`�b�v�̐��E��
constexpr int MAPCHIP_NUM_WIDTH = 4;

// 1�G���A���̃}�b�v�`�b�v�̐��E�c
constexpr int MAPCHIP_NUM_HEIGHT = 10;

// ���݂��Ȃ��}�b�v�`�b�v
constexpr int MAPCHIP_NONE = -1;

// ���݂��Ȃ��摜
constexpr int MAPCHIP_SPRITE_NONE = -1;

// �����摜
constexpr int MAPCHIP_SPRITE_FACTORY = -2;

// �X�e�[�W�t�@�C���̍Ō�
constexpr int END_OF_STAGE_FILE = -1;

// �D�̍ő吔
constexpr int LABEL_NUM_MAX = 5;

// �J�P���̐�
constexpr int PIECE_NUM = 3;

// �f�t�H���g�l
constexpr float DEFAULT_INPUT_VALUE = -1.0f;

// ���{
constexpr int LEFT_LION_BOTTOM = 0;
constexpr int CENTER_LION_BOTTOM = 1;
constexpr int RIGHT_LION_BOTTOM = 2;

constexpr int HP_MAX = 3;
constexpr int HP_MIN = 0;

constexpr int GAME_WORLD_MAX = 3;
constexpr int GAME_STAGE_MAX = 3;

struct MapChipData
{
	int mi_chip_id;
	float mf_angle_z;
	ObjectTag m_tag;
};

struct GoalData
{
	Translate position;
};

struct PieceData
{
	Translate position;
	int i_piece_num;
};

struct TornadeData
{
	Translate position;
};

struct DharmaData
{
	Translate position;
	float mf_max_speed;
	bool mb_is_left;
};

struct CannonData
{
	Translate position;
	float mf_angle_z;
	float mf_max_speed;
	float mf_wait;
};

struct LionData
{
	Translate position;
	float mf_up_max_speed;
	float mf_down_max_speed;
	float mf_up_wait;
	float mf_down_wait;
};

struct StageData
{
	// �w�i�摜
	int mi_front_back;
	int mi_back_back;

	// �v���C���[�ʒu
	int m_player_pos_x;
	int m_player_pos_y;

	// �}�b�v�`�b�v�f�[�^
	std::array<std::array<MapChipData, MAPCHIP_NUM_HEIGHT>, MAPCHIP_NUM_WIDTH * AREA_NUM> mv_front_data;
	std::array<std::array<MapChipData, MAPCHIP_NUM_HEIGHT>, MAPCHIP_NUM_WIDTH * AREA_NUM> mv_back_data;

	// �S�[���f�[�^
	std::vector<GoalData> mv_front_goal_obj;
	std::vector<GoalData> mv_back_goal_obj;

	// �J�P���f�[�^
	std::vector<PieceData> mv_front_piece_obj;
	std::vector<PieceData> mv_back_piece_obj;

	// �����f�[�^
	std::vector<TornadeData> mv_front_tornade_obj;
	std::vector<TornadeData> mv_back_tornade_obj;

	// �_���}�f�[�^
	std::vector<DharmaData> mv_front_dharma_obj;
	std::vector<DharmaData> mv_back_dharma_obj;

	// ��C�f�[�^
	std::vector<CannonData> mv_front_cannon_obj;
	std::vector<CannonData> mv_back_cannon_obj;

	// ���t�g�f�[�^
	std::vector<LionData> mv_front_lion_obj;
	std::vector<LionData> mv_back_lion_obj;

};
