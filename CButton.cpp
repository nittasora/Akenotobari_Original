#include "CButton.h"
#include "CInput.h"
#include "CSpriteFactory.h"
#include "CSingleton.h"

void CButton::Initilize()
{
	this->AddTexture(SPRITE_TEST_0);
	this->mb_is_on = false;
	this->m_point = { 0.0f, 0.0f };
	this->mi_key = 'W';
	this->mi_button = XI_BUTTON_A;
	this->mb_anim_on = false;
	this->mvp_other_buttons.clear();
}

void CButton::UnInitilize()
{
}

void CButton::Update()
{
	// 画像を切り替えないならいらない
	if (!this->mb_anim_on) return;

	// 画像切り替え
	if (this->mb_is_on) {
		this->m_anim = { this->m_point.u, this->m_point.v + 1.0f };
	}
	else {
		this->m_anim = this->m_point;
	}
}

void CButton::OnCollision(COLLIDER_RESULT * collider_res, CGameObject * other)
{
	ObjectTag other_tag = other->GetTag();

	switch (other_tag)
	{
	case TAG_CURSOR:
	{
		CInput* input = CSingleton<CInput>::GetInstance();
		if (this->mvp_other_buttons.empty()) {
			// ボタンを押したなら入力切替
			if (input->GetKeyTrigger(this->mi_key)) {
				this->mb_is_on = !this->mb_is_on;
			}
			else if (input->GetButtonTrigger(this->mi_button)) {
				this->mb_is_on = !this->mb_is_on;
			}
		}
		else {
			if (input->GetKeyTrigger(this->mi_key) || input->GetButtonTrigger(this->mi_button)) {
				if (!this->mb_is_on) {
					this->mb_is_on = true;
					for (size_t i = 0; i < this->mvp_other_buttons.size(); i++)
					{
						if (this->mvp_other_buttons[i] != nullptr) this->mvp_other_buttons[i]->SetONOFF(false);
					}
				}
			}
		}
	}
	break;
	default:
		break;
	}
}

bool CButton::GetONOFF()
{
	return this->mb_is_on;
}

void CButton::SetONOFF(bool is_on)
{
	this->mb_is_on = is_on;
}

void CButton::SetSprite(SPRITE_INDEX sprite, UV point, bool anim_on)
{
	// 画像を変更する
	CSingleton<CSpriteFactory>::GetInstance()->CreateSprite(&this->m_sprites[this->m_current_sprite], sprite);
	this->m_point = point;
	this->mb_anim_on = anim_on;

	this->m_anim = this->m_point;

	// 画像を切り替えないならいらない
	if (!this->mb_anim_on) return;

	// 画像切り替え
	if (this->mb_is_on) {
		this->m_anim = { this->m_point.u, this->m_point.v + 1.0f };
	}

}

void CButton::SetButton(int key, int button)
{
	// キー・ボタンを変更する
	this->mi_key = key;
	this->mi_button = button;
}

void CButton::SetOtherButtonObject(CButton * other_button)
{
	this->mvp_other_buttons.emplace_back(other_button);
}
