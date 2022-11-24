#pragma once

#include "CUnit.h"

class CPlayer : public CUnit
{
public:
	void Initilize() override;
	void UnInitilize() override;
	void Update() override;
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;
	void Damage() override;
	void StartGameOver();
	bool CheckFinishGameOver();
	void Animation();

private:
	// 画像指定用の定数
	enum PLAYER_ANIM
	{
		PA_IDLE_00,
		PA_IDLE_01,
		PA_IDLETORUN,
		PA_RUN00,
		PA_RUN01,
		PA_RUNTOIDLE,
		//PA_JUMP_UP,
		PA_JUMP,
		PA_JUMP_DOWN,
		PA_HIT,
		PA_OVER,
	};

	// アニメーションの再生数カウント
	int mi_anim_count;

	// 竜巻判定
	bool mb_is_tornade;
	bool mb_next_is_tornade;

	// プレイヤーの横の最大速度
	float mf_horizontal_max_speed;

	// プレイヤーの縦の最大速度
	float mf_vertical_max_speed;

	// プレイヤー地上での加速
	float mf_acceleration_ground;

	// プレイヤー空中での加速
	float mf_acceleration_air;

	// プレイヤー地上での減速
	float mf_deceleration_ground;

	// プレイヤー空中での減速
	float mf_deceleration_air;

	// プレイヤージャンプの強さ
	float mf_vertical_jump_power;

	// プレイヤージャンプした瞬間の横の速度
	float mf_horizontal_jump_power;

	// プレイヤーの竜巻時、横の最大速度
	float mf_horizontal_tornade_speed;

	// プレイヤーの竜巻時、縦の最大速度
	float mf_vertical_tornade_speed;

	// 上昇中の重力
	float mf_gravity_up;

	// 下降中の重力
	float mf_gravity_down;

	// 無敵時間
	float mf_invincible_time;
	float mf_current_invincible_time;

	// 無敵時間中の点滅の切り替え時間
	float mf_flashing_time;

	void SetAnimation(PLAYER_ANIM anim_id, float set_time, float width, float height);
	void SetAnimation(PLAYER_ANIM anim_id, int count_max, float set_time, float width, float height);
};

