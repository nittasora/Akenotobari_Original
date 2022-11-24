#include "CPlayerRunImage.h"
#include "CSingleton.h"
#include "CTimer.h"
#include "CInputParameter.h"

void CPlayerRunImage::Initilize()
{
	// �摜����
	this->AddTexture(SPRITE_PLAYER_RUN00);
	this->AddTexture(SPRITE_PLAYER_RUN01);

	// �����摜�͏u������
	this->m_current_sprite = PA_RUN00;

	// ����^�O�̓v���C���[
	this->m_tag = TAG_PLAYER;

	// �A�j���[�V�����Đ����͖���
	this->mi_anim_count = 0;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	this->m_sprites[PA_RUN00].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN00].anim_max);
	this->m_sprites[PA_RUN01].count_check = cip_data.M_DATA.mf_player_run_loop_time / static_cast<float>(this->m_sprites[PA_RUN01].anim_max);
}

void CPlayerRunImage::UnInitilize()
{
}

void CPlayerRunImage::Update()
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
	case PA_RUN00:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {
			// �o�ߎ��Ԃ��X�V
			this->mf_anim_time = tmp_time;

			// �A�j���[�V�����Đ����𑝂₷�B
			this->mi_anim_count++;

			// count_max����܂���
			if (this->mi_anim_count >= 2) {
				// �A�j���[�V�����Đ��������Z�b�g
				this->mi_anim_count = 0;

				// �摜�ύX
				this->m_current_sprite = PA_RUN01;
			}

			// �A�j���[�V�����̃R�}��0�ɂ���
			this->m_anim.u = 0.0f;
			this->m_anim.v = 0.0f;
		}
		break;
	case PA_RUN01:
		// �A�j���[�V�������Ō�܂ōĐ�����܂���
		if (!is_hit) {
			// �o�ߎ��Ԃ��X�V
			this->mf_anim_time = tmp_time;

			// �A�j���[�V�����Đ��������Z�b�g
			this->mi_anim_count = 0;

			// �摜�ύX
			this->m_current_sprite = PA_RUN00;

			// �A�j���[�V�����̃R�}��0�ɂ���
			this->m_anim.u = 0.0f;
			this->m_anim.v = 0.0f;
		}
		break;
	}
}
