#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// �O������ݒ肷��\��̒l�B
// �d�l�ύX�Ńt�@�C���ǂݍ��݂̔p�~�B

struct CIP_Input_Data
{
	//========================================
	// �v���C���[����֌W
	//========================================

	// �v���C���[�̉��̍ő呬�x
	float mf_player_horizontal_max_speed;

	// �v���C���[�̏c�̍ő呬�x
	float mf_player_vertical_max_speed;

	// �v���C���[�n��ł̉���
	float mf_player_acceleration_ground;

	// �v���C���[�󒆂ł̉���
	float mf_player_acceleration_air;

	// �v���C���[�n��ł̌���
	float mf_player_deceleration_ground;

	// �v���C���[�󒆂ł̌���
	float mf_player_deceleration_air;

	// �v���C���[�W�����v�̋���
	float mf_player_vertical_jump_power;

	// �v���C���[�W�����v�����u�Ԃ̉��̑��x
	float mf_player_horizontal_jump_power;

	// �v���C���[�̗������A���̍ő呬�x
	float mf_player_horizontal_tornade_speed;

	// �v���C���[�̗������A�c�̍ő呬�x
	float mf_player_vertical_tornade_speed;

	// �㏸���̏d��
	float mf_player_gravity_up;

	// ���~���̏d��
	float mf_player_gravity_down;

	// ���G����
	float mf_player_invincible_time;

	// ���G���Ԓ��̓_�ł̐؂�ւ�����
	float mf_player_flashing_time;

	//========================================
	// �v���C���[�A�j���[�V�����֌W
	//========================================

	// �v���C���[�̑ҋ@���[�V�����̎���
	float mf_player_idle_loop_time;

	// �v���C���[�̑���o�����[�V�����̎���
	float mf_player_idle_to_run_loop_time;

	// �v���C���[�̑��胂�[�V�����̎���
	float mf_player_run_loop_time;

	// �v���C���[�̑���I��胂�[�V�����̎���
	float mf_player_run_to_idle_loop_time;

	// �v���C���[�̃W�����v�J�n���[�V�����̎���
	float mf_player_jump_up_loop_time;

	// �v���C���[�̃W�����v���[�V�����̎���
	float mf_player_jump_loop_time;

	// �v���C���[�̃W�����v�I�����[�V�����̎���
	float mf_player_jump_down_loop_time;

	// �v���C���[�̂��炢���[�V�����̎���
	float mf_player_damage_loop_time;

	// �v���C���[�̂��ꃂ�[�V�����̎���
	float mf_player_down_loop_time;

	//========================================
	// �X�e�[�W�֌W
	//========================================

	// ��]���x
	float mf_stage_rotate_speed;

	// �Ó]�X�s�[�h
	float mf_stage_fade_speed;

	// �S�ĊJ������
	float mf_stage_push_time_limit;

	// �ő像�[���h��
	int mi_world_max;

	// �ő�X�e�[�W��
	int mi_stage_max;

	//========================================
	// �_���}�֌W
	//========================================

	// �ő呬�x
	float mf_dharma_max_speed;

	// ���~���̏d��
	float mf_dharma_gravity;

	//========================================
	// ���t�g�֌W
	//========================================

	// �オ��X�s�[�h
	float mf_lion_up_max_speed;

	// ������X�s�[�h
	float mf_lion_down_max_speed;

	// ��ɂ��鎞�A������܂ł̑҂��b��
	float mf_lion_up_wait;

	// ���ɂ��鎞�A������܂ł̑҂��b��
	float mf_lion_down_wait;

	//========================================
	// ��C�֌W
	//========================================

	// ����
	float mf_cannon_angle_z;

	// �ʂ̑���
	float mf_cannon_max_speed;

	// �ʂ̔��ˊԊu
	float mf_cannon_wait;
};

class CIP_Data
{
public:
	CIP_Data(CIP_Input_Data data);

	// �f�[�^
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


