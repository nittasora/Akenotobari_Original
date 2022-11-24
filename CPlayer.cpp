#include "CPlayer.h"
#include "CTimer.h"
#include "CInput.h"
#include "CInputParameter.h"
#include "CGameManager.h"
#include "CXAudio2.h"

#include "CSingleton.h"

void CPlayer::Initilize()
{
	// �摜����
	this->AddTexture(SPRITE_PLAYER_IDLE_00);
	this->AddTexture(SPRITE_PLAYER_IDLE_01);
	this->AddTexture(SPRITE_PLAYER_IDLETORUN);
	this->AddTexture(SPRITE_PLAYER_RUN00);
	this->AddTexture(SPRITE_PLAYER_RUN01);
	this->AddTexture(SPRITE_PLAYER_RUNTOIDLE);
	//this->AddTexture(SPRITE_PLAYER_JUMP_UP);
	this->AddTexture(SPRITE_PLAYER_JUMP);
	this->AddTexture(SPRITE_PLAYER_JUMP_DOWN);
	this->AddTexture(SPRITE_PLAYER_HIT);
	this->AddTexture(SPRITE_PLAYER_GAMEOVER);

	// �����摜�͑ҋ@
	this->m_current_sprite = PA_IDLE_00;

	// ����^�O�̓v���C���[
	this->m_tag = TAG_PLAYER;

	// �A�j���[�V�����Đ����͖���
	this->mi_anim_count = 0;

	// �ݒu�����؂�
	this->mb_is_ground = false;
	this->mb_next_is_ground = false;
	this->mb_is_tornade = false;
	this->mb_next_is_tornade = false;

	// �摜�E�R���C�_�[�T�C�Y�̓K�p
	this->SetDefaultPosition(PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);
	this->m_collider.width = PLAYER_COLLIDER_WIDTH;
	this->m_collider.height = PLAYER_COLLIDER_HEIGHT;
	this->m_collider.center_y = -4.0f;

	this->mf_current_invincible_time = 0.0f;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	this->mf_horizontal_max_speed = cip_data.M_DATA.mf_player_horizontal_max_speed;
	this->mf_vertical_max_speed = cip_data.M_DATA.mf_player_vertical_max_speed;
	this->mf_acceleration_ground = cip_data.M_DATA.mf_player_acceleration_ground;
	this->mf_acceleration_air = cip_data.M_DATA.mf_player_acceleration_air;
	this->mf_deceleration_ground = cip_data.M_DATA.mf_player_deceleration_ground;
	this->mf_deceleration_air = cip_data.M_DATA.mf_player_deceleration_air;
	this->mf_vertical_jump_power = cip_data.M_DATA.mf_player_vertical_jump_power;
	this->mf_horizontal_jump_power = cip_data.M_DATA.mf_player_horizontal_jump_power;
	this->mf_horizontal_tornade_speed = cip_data.M_DATA.mf_player_horizontal_tornade_speed;
	this->mf_vertical_tornade_speed = cip_data.M_DATA.mf_player_vertical_tornade_speed;
	this->mf_gravity_up = cip_data.M_DATA.mf_player_gravity_up;
	this->mf_gravity_down = cip_data.M_DATA.mf_player_gravity_down;
	this->mf_invincible_time = cip_data.M_DATA.mf_player_invincible_time;
	this->mf_flashing_time = cip_data.M_DATA.mf_player_flashing_time;

	this->m_sprites[PA_IDLE_00].count_check = cip_data.M_DATA.mf_player_idle_loop_time / static_cast<float>(this->m_sprites[PA_IDLE_00].anim_max);
	this->m_sprites[PA_IDLE_01].count_check = cip_data.M_DATA.mf_player_idle_loop_time / static_cast<float>(this->m_sprites[PA_IDLE_01].anim_max);
	this->m_sprites[PA_IDLETORUN].count_check = cip_data.M_DATA.mf_player_idle_to_run_loop_time / static_cast<float>(this->m_sprites[PA_IDLETORUN].anim_max);
	this->m_sprites[PA_RUN00].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN00].anim_max);
	this->m_sprites[PA_RUN01].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN01].anim_max);
	this->m_sprites[PA_RUNTOIDLE].count_check = cip_data.M_DATA.mf_player_run_to_idle_loop_time / static_cast<float>(this->m_sprites[PA_RUNTOIDLE].anim_max);
	//this->m_sprites[PA_JUMP_UP].count_check = cip_data.M_DATA.mf_player_jump_up_loop_time / static_cast<float>(this->m_sprites[PA_JUMP_UP].anim_max);
	this->m_sprites[PA_JUMP].count_check = cip_data.M_DATA.mf_player_jump_loop_time / static_cast<float>(this->m_sprites[PA_JUMP].anim_max);
	this->m_sprites[PA_JUMP_DOWN].count_check = cip_data.M_DATA.mf_player_jump_down_loop_time / static_cast<float>(this->m_sprites[PA_JUMP_DOWN].anim_max);
	this->m_sprites[PA_HIT].count_check = cip_data.M_DATA.mf_player_damage_loop_time / static_cast<float>(this->m_sprites[PA_HIT].anim_max);
	this->m_sprites[PA_OVER].count_check = cip_data.M_DATA.mf_player_down_loop_time / static_cast<float>(this->m_sprites[PA_OVER].anim_max);
}

void CPlayer::UnInitilize()
{
}

void CPlayer::Update()
{
	// Sound
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();

	// ����
	CInput* controller = CSingleton<CInput>::GetInstance();

	this->Animation();

	this->mb_is_ground = this->mb_next_is_ground;
	this->mb_is_tornade = this->mb_next_is_tornade;

	// �d�͌v�Z
	if (this->m_velocity.y > 0.0f) this->m_velocity.y -= this->mf_gravity_up;
	else this->m_velocity.y -= this->mf_gravity_down;

	//�v�Z����
	float input_x = controller->GetAxisRawX(true);

	float acceleration_x = this->mf_acceleration_ground;
	float deceleration_x = this->mf_deceleration_ground;
	if (!this->mb_is_ground) {
		acceleration_x = this->mf_acceleration_air;
		deceleration_x = this->mf_deceleration_air;
	}

	if (input_x == 0.0f) {
		if (controller->GetKeyPress('A') || controller->GetButtonPress(XI_BUTTON_LEFT)) {
			this->m_velocity.x -= acceleration_x;
		}
		if (controller->GetKeyPress('D') || controller->GetButtonPress(XI_BUTTON_RIGHT)) {
			this->m_velocity.x += acceleration_x;
		}
	}
	else {
		this->m_velocity.x += input_x * acceleration_x;
	}

	if (controller->GetButtonTrigger(XI_BUTTON_A) && this->mb_is_ground) {
		audio->Start(SOUND_LABEL_SE_JUMP);
		this->m_velocity.y = this->mf_vertical_jump_power;
		//this->m_velocity.x /= this->mf_horizontal_jump_power;
		this->mb_is_ground = false;

		//this->SetAnimation(PA_JUMP_UP, 0.0f, PLAYER_JUMP_UP_WIDTH, PLAYER_JUMP_UP_HEIGHT);

		//this->m_velocity.y = 4.2f;
		//this->m_velocity.x /= 2.6f;
		//this->mb_is_ground = false;
	}
	if (controller->GetKeyTrigger(VK_SPACE) && this->mb_is_ground) {
		audio->Start(SOUND_LABEL_SE_JUMP);
		this->m_velocity.y = this->mf_vertical_jump_power;
		//this->m_velocity.x /= this->mf_horizontal_jump_power;
		this->mb_is_ground = false;

		//this->SetAnimation(PA_JUMP_UP, 0.0f, PLAYER_JUMP_UP_WIDTH, PLAYER_JUMP_UP_HEIGHT);

		//this->m_velocity.y = 4.2f;
		//this->m_velocity.x /= 2.6f;
		//this->mb_is_ground = false;
	}

	//�ő呬�x
	float max_horizontal_velocity = this->mf_horizontal_max_speed;
	float max_vertical_velocity = this->mf_vertical_max_speed;
	if (this->mb_is_tornade) {
		if (this->m_velocity.y > 0.0f) this->m_velocity.y += 0.6f;
		else this->m_velocity.y += 1.8f;

		if (controller->GetKeyPress(VK_SPACE)) {
			this->m_velocity.y = this->mf_vertical_jump_power;
		}
		else if (controller->GetButtonPress(XI_BUTTON_A)) {
			this->m_velocity.y = this->mf_vertical_jump_power;
		}
		else {
			max_horizontal_velocity = this->mf_horizontal_tornade_speed;
			max_vertical_velocity = this->mf_vertical_tornade_speed;
		}
	}
	else if (!this->mb_is_ground) {
		max_horizontal_velocity = this->mf_horizontal_jump_power;
	}

	// ���]����
	if (this->m_velocity.x > 0.0f) {
		this->m_transform.angle.y = 0.0f;
	}
	if (this->m_velocity.x < 0.0f) {
		this->m_transform.angle.y = 180.0f;
	}

	// ��������
	if (this->m_velocity.x > 0.0f) {  // �{�̒l�̎�
		this->m_velocity.x -= deceleration_x;

		if (this->m_velocity.x < 0.0f) { // �����������ă}�C�i�X�ɂȂ���
			this->m_velocity.x = 0.0f;  // ��~������
		}
	}
	else if (this->m_velocity.x < 0.0f) { // �[�̒l�̎�
		this->m_velocity.x += deceleration_x;

		if (this->m_velocity.x > 0.0f) { // �����������ăv���X�ɂȂ���
			this->m_velocity.x = 0.0f;  // ��~������
		}
	}

	//�ő呬�x�������Ȃ��悤�ɂ���
	if (this->m_velocity.x > max_horizontal_velocity)this->m_velocity.x = max_horizontal_velocity;
	if (this->m_velocity.x < -max_horizontal_velocity)this->m_velocity.x = -max_horizontal_velocity;
	if (this->m_velocity.y > max_vertical_velocity)this->m_velocity.y = max_vertical_velocity;
	if (this->m_velocity.y < -max_vertical_velocity)this->m_velocity.y = -max_vertical_velocity;

	// �ړ��X�V
	this->m_change.y += this->m_velocity.y;
	//if (this->m_current_sprite != PA_JUMP_UP && this->m_current_sprite != PA_JUMP_DOWN) {
	//	this->m_change.x += this->m_velocity.x;
	//}
	this->m_change.x += this->m_velocity.x;

	// �ݒu�����؂�
	this->mb_next_is_ground = false;
	this->mb_next_is_tornade = false;

	if (this->mf_current_invincible_time > 0) {
		int flash_count = static_cast<int>(this->mf_current_invincible_time / this->mf_flashing_time) % 2;
		if (flash_count == 0) {
			this->m_color.a = 0.0f;
		}
		else {
			this->m_color.a = 1.0f;
		}
		this->mf_current_invincible_time -= CSingleton <CTimer>::GetInstance()->GetDeltaTime();
	}
	else {
		this->m_color.a = 1.0f;
	}
}

void CPlayer::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
	ObjectTag other_tag = other->GetTag();
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();

	switch (other_tag)
	{
	case TAG_RED_BLOCK:
	case TAG_BLUE_BLOCK:
		// �ݒu����
		if (!other->IsActive()) {
			break;
		}

		if (collider_res->distance_y > 0) {
			this->mb_next_is_ground = true;
			this->m_velocity.y = 0.0f;
		}
		if (collider_res->distance_x != 0.0f && !this->mb_is_ground) {
			this->m_velocity.x = 0.0f;
		}
		this->m_change.x += collider_res->distance_x;
		this->m_change.y += collider_res->distance_y;
		break;
	case TAG_LION_BOTTOM:
	case TAG_BORDER_PAGE:
	case TAG_MAPCHIP:
	case TAG_CANNON:
	case TAG_BORDER_TOP:
		// �ݒu����
		if (collider_res->distance_y > 0) {
			this->mb_next_is_ground = true;
			this->m_velocity.y = 0.0f;
		}
		if (collider_res->distance_x != 0.0f && !this->mb_is_ground) {
			this->m_velocity.x = 0.0f;
		}
		this->m_change.x += collider_res->distance_x;
		this->m_change.y += collider_res->distance_y;
		break;
	case TAG_TORNADO:
		this->mb_next_is_tornade = true;
		break;
	case TAG_LION_TOP:
		// �ݒu����
		if (collider_res->distance_y > 0) {
			this->mb_next_is_ground = true;
			this->m_velocity.y = 0.0f;
		}
		else if (collider_res->distance_y < 0) {
			this->Damage();
		}
		if (collider_res->distance_x != 0.0f && !this->mb_is_ground) {
			this->m_velocity.x = 0.0f;
		}
		this->m_change.x += collider_res->distance_x;
		if (collider_res->distance_y > 0) this->m_change.y += collider_res->distance_y;
		else if (!this->mb_next_is_ground) this->m_change.y += collider_res->distance_y;
		break;
	case TAG_DHARMA:
		//this->Damage();
		break;
	case TAG_BALL:
		this->Damage();
		break;
	case TAG_PIECE_1:
	{
		game_manager->SetGetPiece(0, true);
		Collider collider = other->GetCollider();
		collider.is_active = false;
		other->SetCollider(&collider);
		obj_flg flg = 0;
		other->SetFlg(&flg);
		if (game_manager->CheckGetPiece(PIECE_NUM)) audio->Start(SOUND_LABEL_SE_COMPLETE_PIECE);
		else audio->Start(SOUND_LABEL_SE_GET_PIECE);
	}
	break;
	case TAG_PIECE_2:
	{
		game_manager->SetGetPiece(1, true);
		Collider collider = other->GetCollider();
		collider.is_active = false;
		other->SetCollider(&collider);
		obj_flg flg = 0;
		other->SetFlg(&flg);
		if (game_manager->CheckGetPiece(PIECE_NUM)) audio->Start(SOUND_LABEL_SE_COMPLETE_PIECE);
		else audio->Start(SOUND_LABEL_SE_GET_PIECE);
	}
	break;
	case TAG_PIECE_3:
	{
		game_manager->SetGetPiece(2, true);
		Collider collider = other->GetCollider();
		collider.is_active = false;
		other->SetCollider(&collider);
		obj_flg flg = 0;
		other->SetFlg(&flg);
		if (game_manager->CheckGetPiece(PIECE_NUM)) audio->Start(SOUND_LABEL_SE_COMPLETE_PIECE);
		else audio->Start(SOUND_LABEL_SE_GET_PIECE);
	}
	break;
	case TAG_GOAL:
		game_manager->SetGoal(true);
		break;
	default:
		break;
	}
}

void CPlayer::Animation()
{
	// �o�ߎ��Ԃ̌v�Z
	this->mf_anim_time += CSingleton <CTimer>::GetInstance()->GetDeltaTime();

	// ���Ԃ̃R�s�[
	float tmp_time = this->mf_anim_time;

	// �A�j���[�V�����̃R�}�����肵�����ǂ���
	bool is_hit = false;

	// �o�ߎ��Ԃ���A�j���[�V�����̃R�}������o��
	for (size_t i = 0; i < this->m_sprites[this->m_current_sprite].anim_max; i++)
	{
		// �o�ߎ��Ԃ����̃R�}�܂ł̎��Ԃ�菬���� -> ���̃R�}�Ō���
		if (tmp_time < this->m_sprites[this->m_current_sprite].count_check) {

			// �A�j���[�V�����̃R�}�����肵�܂���
			is_hit = true;

			// �A�j���[�V�����̃R�}�v�Z
			this->m_anim.u = static_cast<float>(static_cast<int>(i) % this->m_sprites[this->m_current_sprite].slice_x);
			this->m_anim.v = static_cast<float>(static_cast<int>(i) / this->m_sprites[this->m_current_sprite].slice_x);
			break;
		}
		else {
			// �o�ߎ��Ԃ����̃R�}�܂ł̎��Ԃ��傫��
			tmp_time -= this->m_sprites[this->m_current_sprite].count_check;
		}
	}

	// ���݂̉摜���番�򂵂܂�
	switch (this->m_current_sprite)
	{
	case PA_IDLE_00:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {

			// �u���Z�b�g
			this->SetAnimation(PA_IDLE_01, 2, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);

		}

		// ���x��0����Ȃ� = ���郂�[�V������
		if (this->m_velocity.x != 0.0f) {

			// ����o���Z�b�g
			this->SetAnimation(PA_IDLETORUN, 0.0f, PLAYER_IDLE_TO_RUN_WIDTH, PLAYER_IDLE_TO_RUN_HEIGHT);
		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_IDLE_01:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {

			// �u�������Z�b�g
			this->SetAnimation(PA_IDLE_00, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);

		}

		// ���x��0����Ȃ� = ���郂�[�V������
		if (this->m_velocity.x != 0.0f) {

			// ����o���Z�b�g
			this->SetAnimation(PA_IDLETORUN, 0.0f, PLAYER_IDLE_TO_RUN_WIDTH, PLAYER_IDLE_TO_RUN_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_IDLETORUN:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {

			// ����Z�b�g
			this->SetAnimation(PA_RUN00, tmp_time, PLAYER_RUN_WIDTH, PLAYER_RUN_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_RUN00:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {

			// �u���Z�b�g
			this->SetAnimation(PA_RUN01, 2, tmp_time, PLAYER_RUN_WIDTH, PLAYER_RUN_HEIGHT);

		}

		// ���x��0 = �~�܂郂�[�V������
		if (this->m_velocity.x == 0.0f) {

			// ����I���Z�b�g
			this->SetAnimation(PA_RUNTOIDLE, 0.0f, PLAYER_RUN_TO_IDLE_WIDTH, PLAYER_RUN_TO_IDLE_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_RUN01:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {

			// �u�������Z�b�g
			this->SetAnimation(PA_RUN00, tmp_time, PLAYER_RUN_WIDTH, PLAYER_RUN_HEIGHT);

		}

		// ���x��0 = �~�܂郂�[�V������
		if (this->m_velocity.x == 0.0f) {

			// ����I���Z�b�g
			this->SetAnimation(PA_RUNTOIDLE, 0.0f, PLAYER_RUN_TO_IDLE_WIDTH, PLAYER_RUN_TO_IDLE_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
	case PA_RUNTOIDLE:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {

			// �u�������Z�b�g
			this->SetAnimation(PA_IDLE_00, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);

		}

		if (!this->mb_is_ground) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		break;
		//case PA_JUMP_UP:
		//	// �A�j���[�V�������Ō�܂ōĐ�����܂���
		//	if (!is_hit) {
		//		this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		//	}

		//	if (this->m_anim.u == 2.0f && this->m_anim.v == 2.0f) {
		//		this->m_velocity.y = this->mf_vertical_jump_power;
		//		//this->m_velocity.x /= this->mf_horizontal_jump_power;
		//		this->mb_is_ground = false;
		//	}
		//	break;
	case PA_JUMP:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {
			this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
		}

		// ���n����
		if (this->mb_is_ground) {
			this->SetAnimation(PA_JUMP_DOWN, 0.0f, PLAYER_JUMP_DOWN_WIDTH, PLAYER_JUMP_DOWN_HEIGHT);
			CSingleton<CXAudio2>::GetInstance()->Start(SOUND_LABEL_SE_FALL);
		}
		break;
	case PA_JUMP_DOWN:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {
			this->SetAnimation(PA_IDLE_00, 0.0f, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);
		}
		break;
	case PA_HIT:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {
			if (this->mb_is_ground) {
				// ���x��0����Ȃ� = ���郂�[�V������
				if (this->m_velocity.x != 0.0f) {

					// ����o���Z�b�g
					this->SetAnimation(PA_IDLETORUN, 0.0f, PLAYER_IDLE_TO_RUN_WIDTH, PLAYER_IDLE_TO_RUN_HEIGHT);
				}
				else {
					// �u�������Z�b�g
					this->SetAnimation(PA_IDLE_00, tmp_time, PLAYER_IDLE_WIDTH, PLAYER_IDLE_HEIGHT);
				}
			}
			else {
				this->SetAnimation(PA_JUMP, 0.0f, PLAYER_JUMP_WIDTH, PLAYER_JUMP_HEIGHT);
			}
		}
		break;
	case PA_OVER:
		// ���G���Ԃ�؂�
		this->mf_current_invincible_time = 0.0f;

		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {
			this->mf_anim_time = this->m_sprites[this->m_current_sprite].count_check * static_cast<float>(this->m_sprites[this->m_current_sprite].anim_max + 1);

			// �A�j���[�V�����̃R�}�v�Z
			this->m_anim.u = static_cast<float>(this->m_sprites[this->m_current_sprite].slice_x - 1);
			this->m_anim.v = static_cast<float>(this->m_sprites[this->m_current_sprite].slice_y - 1);
		}
		break;
	default:
		break;
	}
}

void CPlayer::SetAnimation(PLAYER_ANIM anim_id, float set_time, float width, float height)
{
	// �o�ߎ��Ԃ��X�V
	this->mf_anim_time = set_time;

	// �A�j���[�V�����Đ��������Z�b�g
	this->mi_anim_count = 0;

	// �摜�ύX
	this->m_current_sprite = anim_id;

	// �A�j���[�V�����̃R�}��0�ɂ���
	this->m_anim.u = 0.0f;
	this->m_anim.v = 0.0f;

	// �摜�T�C�Y�ύX
	this->SetDefaultPosition(width, height);
}

void CPlayer::SetAnimation(PLAYER_ANIM anim_id, int count_max, float set_time, float width, float height)
{
	// �o�ߎ��Ԃ��X�V
	this->mf_anim_time = set_time;

	// �A�j���[�V�����Đ����𑝂₷�B
	this->mi_anim_count++;

	// count_max����܂���
	if (this->mi_anim_count >= count_max) {
		// �A�j���[�V�����Đ��������Z�b�g
		this->mi_anim_count = 0;

		// �摜�ύX
		this->m_current_sprite = anim_id;
	}

	// �A�j���[�V�����̃R�}��0�ɂ���
	this->m_anim.u = 0.0f;
	this->m_anim.v = 0.0f;

	// �摜�T�C�Y�ύX
	this->SetDefaultPosition(width, height);
}

void CPlayer::Damage()
{
	if (this->mf_current_invincible_time <= 0.0f) {
		CSingleton<CXAudio2>::GetInstance()->Start(SOUND_LABEL_SE_DAMAGE);
		this->mf_current_invincible_time = this->mf_invincible_time;
		this->SetAnimation(PA_HIT, 0.0f, PLAYER_HIT_WIDTH, PLAYER_HIT_HEIGHT);
		CSingleton<CGameManager>::GetInstance()->PullHP();
	}
}

void CPlayer::StartGameOver()
{
	// �o�ߎ��Ԃ��X�V
	this->mf_anim_time = 0.0f;

	// �A�j���[�V�����Đ��������Z�b�g
	this->mi_anim_count = 0;

	// �摜�ύX
	this->m_current_sprite = PA_OVER;

	// �A�j���[�V�����̃R�}��0�ɂ���
	this->m_anim.u = 0.0f;
	this->m_anim.v = 0.0f;

	// �摜�T�C�Y�ύX
	this->SetDefaultPosition(PLAYER_OVER_WIDTH, PLAYER_OVER_HEIGHT);
}

bool CPlayer::CheckFinishGameOver()
{
	return this->mf_anim_time >= this->m_sprites[PA_OVER].count_check * static_cast<float>(this->m_sprites[PA_OVER].anim_max + 1);
}
