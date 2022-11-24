#include "CTornade.h"
#include "CTimer.h"
#include "CInput.h"

#include "CSingleton.h"

void CTornade::Initilize()
{
	// �摜����
	this->AddTexture(SPRITE_TORNADO);

	// ����^�O�͗���
	this->m_tag = TAG_TORNADO;

	// �A�j���[�V�����Đ����͖���
	this->mi_anim_count = 0;

	// �摜�E�R���C�_�[�T�C�Y�̓K�p
	this->SetDefaultPosAndColl(TORNADE_WIDTH, TORNADE_HEIGHT);
}

void CTornade::UnInitilize()
{
}

void CTornade::Update()
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

	if (!is_hit) {
		this->mf_anim_time = tmp_time;
		this->m_anim.u = 0.0f;
		this->m_anim.v = 0.0f;
	}
}

