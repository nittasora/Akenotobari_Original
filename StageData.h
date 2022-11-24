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

// エリア数
constexpr int AREA_NUM = 7;

// 1エリア内のマップチップの数・横
constexpr int MAPCHIP_NUM_WIDTH = 4;

// 1エリア内のマップチップの数・縦
constexpr int MAPCHIP_NUM_HEIGHT = 10;

// 存在しないマップチップ
constexpr int MAPCHIP_NONE = -1;

// 存在しない画像
constexpr int MAPCHIP_SPRITE_NONE = -1;

// 既存画像
constexpr int MAPCHIP_SPRITE_FACTORY = -2;

// ステージファイルの最後
constexpr int END_OF_STAGE_FILE = -1;

// 札の最大数
constexpr int LABEL_NUM_MAX = 5;

// カケラの数
constexpr int PIECE_NUM = 3;

// デフォルト値
constexpr float DEFAULT_INPUT_VALUE = -1.0f;

// 下顎
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
	// 背景画像
	int mi_front_back;
	int mi_back_back;

	// プレイヤー位置
	int m_player_pos_x;
	int m_player_pos_y;

	// マップチップデータ
	std::array<std::array<MapChipData, MAPCHIP_NUM_HEIGHT>, MAPCHIP_NUM_WIDTH * AREA_NUM> mv_front_data;
	std::array<std::array<MapChipData, MAPCHIP_NUM_HEIGHT>, MAPCHIP_NUM_WIDTH * AREA_NUM> mv_back_data;

	// ゴールデータ
	std::vector<GoalData> mv_front_goal_obj;
	std::vector<GoalData> mv_back_goal_obj;

	// カケラデータ
	std::vector<PieceData> mv_front_piece_obj;
	std::vector<PieceData> mv_back_piece_obj;

	// 竜巻データ
	std::vector<TornadeData> mv_front_tornade_obj;
	std::vector<TornadeData> mv_back_tornade_obj;

	// ダルマデータ
	std::vector<DharmaData> mv_front_dharma_obj;
	std::vector<DharmaData> mv_back_dharma_obj;

	// 大砲データ
	std::vector<CannonData> mv_front_cannon_obj;
	std::vector<CannonData> mv_back_cannon_obj;

	// リフトデータ
	std::vector<LionData> mv_front_lion_obj;
	std::vector<LionData> mv_back_lion_obj;

};
