#include "CGoal.h"

void CGoal::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_GOAL);

	// 判定タグはゴール
	this->m_tag = TAG_GOAL;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosition(GOAL_WIDTH, GOAL_HEIGHT);
	this->m_collider.width = MAPCHIP_WIDTH;
	this->m_collider.height = MAPCHIP_HEIGHT;
}

void CGoal::UnInitilize()
{
}
