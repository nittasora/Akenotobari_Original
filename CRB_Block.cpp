#include "CRB_Block.h"

#include "CGameManager.h"
#include "CXAudio2.h"

#include "CSingleton.h"

void CRB_Block::Initilize()
{
	this->m_tag = TAG_RED_BLOCK;
	this->AddTexture(SPRITE_RED_BULE);
	this->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

	this->game_manager = CSingleton<CGameManager>::GetInstance();
	this->mb_is_blue = false;
	this->mb_fold_switch = this->game_manager->GetFoldSwitch();
	this->mb_is_switch = false;
}

void CRB_Block::UnInitilize()
{

}

void CRB_Block::Update()
{
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();
	this->mb_fold_switch = this->game_manager->GetFoldSwitch();

	if (this->mb_is_switch) {
		if (this->mb_fold_switch == this->mb_is_blue) {
			audio->Start(SOUND_LABEL_SE_SWITCH_RB);
			this->m_anim.v = 0.0f;
			this->m_flg = OBJ_FLG_DRAW | OBJ_FLG_ACTIVE;
			//this->m_collider.is_active = true;
		}
		else {
			audio->Start(SOUND_LABEL_SE_SWITCH_RB);
			this->m_anim.v = 1.0f;
			this->m_flg = OBJ_FLG_DRAW;
			//this->m_collider.is_active = false;
		}

		this->mb_is_switch = false;
	}

	if (this->mb_fold_switch == this->mb_is_blue) {
		if (!this->IsActive()) {
			this->mb_is_switch = true;
			//audio->Start(SOUND_LABEL_SE_SWITCH_RB);
			//this->m_anim.v = 0.0f;
			//this->m_collider.is_active = true;
		}
	}
	else {
		if (this->IsActive()) {
			this->mb_is_switch = true;
			//audio->Start(SOUND_LABEL_SE_SWITCH_RB);
			//this->m_anim.v = 1.0f;
			//this->m_collider.is_active = false;
		}
	}
}

void CRB_Block::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
	ObjectTag other_tag = other->GetTag();

	switch (other_tag)
	{
	case TAG_PLAYER:
		if (!this->IsActive()) {
			this->mb_is_switch = false;
		}
		break;
	}
}

void CRB_Block::SetIsBlue(bool is_blue)
{
	this->mb_is_blue = is_blue;

	if (this->mb_is_blue) {
		this->m_anim.u = 1.0f;
		this->m_anim.v = 1.0f;
		this->m_tag = TAG_BLUE_BLOCK;
	}
	else {
		this->m_anim.u = 0.0f;
		this->m_tag = TAG_RED_BLOCK;
	}

	this->Update();
}
