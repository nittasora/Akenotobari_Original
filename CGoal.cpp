#include "CGoal.h"

void CGoal::Initilize()
{
	// �摜����
	this->AddTexture(SPRITE_GOAL);

	// ����^�O�̓S�[��
	this->m_tag = TAG_GOAL;

	// �摜�E�R���C�_�[�T�C�Y�̓K�p
	this->SetDefaultPosition(GOAL_WIDTH, GOAL_HEIGHT);
	this->m_collider.width = MAPCHIP_WIDTH;
	this->m_collider.height = MAPCHIP_HEIGHT;
}

void CGoal::UnInitilize()
{
}
