#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// 外部から設定する予定の値。
// 仕様変更でファイル読み込みの廃止。

struct CIP_Input_Data
{
	//========================================
	// プレイヤー操作関係
	//========================================

	// プレイヤーの横の最大速度
	float mf_player_horizontal_max_speed;

	// プレイヤーの縦の最大速度
	float mf_player_vertical_max_speed;

	// プレイヤー地上での加速
	float mf_player_acceleration_ground;

	// プレイヤー空中での加速
	float mf_player_acceleration_air;

	// プレイヤー地上での減速
	float mf_player_deceleration_ground;

	// プレイヤー空中での減速
	float mf_player_deceleration_air;

	// プレイヤージャンプの強さ
	float mf_player_vertical_jump_power;

	// プレイヤージャンプした瞬間の横の速度
	float mf_player_horizontal_jump_power;

	// プレイヤーの竜巻時、横の最大速度
	float mf_player_horizontal_tornade_speed;

	// プレイヤーの竜巻時、縦の最大速度
	float mf_player_vertical_tornade_speed;

	// 上昇中の重力
	float mf_player_gravity_up;

	// 下降中の重力
	float mf_player_gravity_down;

	// 無敵時間
	float mf_player_invincible_time;

	// 無敵時間中の点滅の切り替え時間
	float mf_player_flashing_time;

	//========================================
	// プレイヤーアニメーション関係
	//========================================

	// プレイヤーの待機モーションの時間
	float mf_player_idle_loop_time;

	// プレイヤーの走り出しモーションの時間
	float mf_player_idle_to_run_loop_time;

	// プレイヤーの走りモーションの時間
	float mf_player_run_loop_time;

	// プレイヤーの走り終わりモーションの時間
	float mf_player_run_to_idle_loop_time;

	// プレイヤーのジャンプ開始モーションの時間
	float mf_player_jump_up_loop_time;

	// プレイヤーのジャンプモーションの時間
	float mf_player_jump_loop_time;

	// プレイヤーのジャンプ終了モーションの時間
	float mf_player_jump_down_loop_time;

	// プレイヤーのくらいモーションの時間
	float mf_player_damage_loop_time;

	// プレイヤーのやられモーションの時間
	float mf_player_down_loop_time;

	//========================================
	// ステージ関係
	//========================================

	// 回転速度
	float mf_stage_rotate_speed;

	// 暗転スピード
	float mf_stage_fade_speed;

	// 全て開く時間
	float mf_stage_push_time_limit;

	// 最大ワールド数
	int mi_world_max;

	// 最大ステージ数
	int mi_stage_max;

	//========================================
	// ダルマ関係
	//========================================

	// 最大速度
	float mf_dharma_max_speed;

	// 下降中の重力
	float mf_dharma_gravity;

	//========================================
	// リフト関係
	//========================================

	// 上がるスピード
	float mf_lion_up_max_speed;

	// 下がるスピード
	float mf_lion_down_max_speed;

	// 上にいる時、落ちるまでの待ち秒数
	float mf_lion_up_wait;

	// 下にいる時、落ちるまでの待ち秒数
	float mf_lion_down_wait;

	//========================================
	// 大砲関係
	//========================================

	// 向き
	float mf_cannon_angle_z;

	// 玉の速さ
	float mf_cannon_max_speed;

	// 玉の発射間隔
	float mf_cannon_wait;
};

class CIP_Data
{
public:
	CIP_Data(CIP_Input_Data data);

	// データ
	const CIP_Input_Data M_DATA;
};

class CInputParameter
{
public:
	CInputParameter();
	~CInputParameter();

	CIP_Data GetCIP_Data();

private:
	CIP_Data* data;
};


