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
	// �摜�w��p�̒萔
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

	// �A�j���[�V�����̍Đ����J�E���g
	int mi_anim_count;

	// ��������
	bool mb_is_tornade;
	bool mb_next_is_tornade;

	// �v���C���[�̉��̍ő呬�x
	float mf_horizontal_max_speed;

	// �v���C���[�̏c�̍ő呬�x
	float mf_vertical_max_speed;

	// �v���C���[�n��ł̉���
	float mf_acceleration_ground;

	// �v���C���[�󒆂ł̉���
	float mf_acceleration_air;

	// �v���C���[�n��ł̌���
	float mf_deceleration_ground;

	// �v���C���[�󒆂ł̌���
	float mf_deceleration_air;

	// �v���C���[�W�����v�̋���
	float mf_vertical_jump_power;

	// �v���C���[�W�����v�����u�Ԃ̉��̑��x
	float mf_horizontal_jump_power;

	// �v���C���[�̗������A���̍ő呬�x
	float mf_horizontal_tornade_speed;

	// �v���C���[�̗������A�c�̍ő呬�x
	float mf_vertical_tornade_speed;

	// �㏸���̏d��
	float mf_gravity_up;

	// ���~���̏d��
	float mf_gravity_down;

	// ���G����
	float mf_invincible_time;
	float mf_current_invincible_time;

	// ���G���Ԓ��̓_�ł̐؂�ւ�����
	float mf_flashing_time;

	void SetAnimation(PLAYER_ANIM anim_id, float set_time, float width, float height);
	void SetAnimation(PLAYER_ANIM anim_id, int count_max, float set_time, float width, float height);
};

