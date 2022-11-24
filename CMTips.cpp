#include "CMTips.h"

#include "CXAudio2.h"
#include "CInput.h"
#include "CGameManager.h"
#include "CMenuManager.h"
#include "CImage.h"

#include "CSingleton.h"

namespace
{
	constexpr UV TIPS_GOAL = { 0.0f, 0.0f };
	constexpr UV TIPS_BACK = { 0.0f, 1.0f };
	constexpr UV TIPS_TURN = { 0.0f, 2.0f };
	constexpr UV TIPS_FOLD_1 = { 1.0f, 0.0f };
	constexpr UV TIPS_FOLD_2 = { 1.0f, 1.0f };
}

void CMTips::Initialize()
{
	this->mb_is_act = false;
	this->mi_count = 0;
	this->m_condition = TC_1_1;

	for (size_t i = 0; i < 2; i++)
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_RULE);

		// 大きさ変更
		Transform transform = new_image->GetTransform();
		transform.scale.x = 2.0f;
		transform.scale.y = 2.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(971.0f, 659.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
	}

	this->mp_game_manager = CSingleton<CGameManager>::GetInstance();

	this->mvp_tips.clear();
}

void CMTips::Finalize()
{
	for (size_t i = 0; i < this->mvp_objects.size(); i++)
	{
		this->mvp_objects[i]->UnInitilize();
		delete this->mvp_objects[i];
	}
	this->mvp_objects.clear();
}

void CMTips::Update()
{
	if (this->m_current_condition != this->m_next_condition) this->m_current_condition = this->m_next_condition;

	switch (this->m_current_condition)
	{
	case MENU_FADE_IN_PREPARE:

		this->mvp_tips.clear();
		this->mi_count = 0;
		this->mb_is_act = false;

		for (size_t i = 0; i < this->mvp_objects.size(); i++)
		{
			Transform transform = this->mvp_objects[i]->GetTransform();
			transform.scale.x = 2.0f;
			transform.scale.y = 2.0f;
			this->mvp_objects[i]->SetTransform(&transform);

			Color color = this->mvp_objects[i]->GetColor();
			color.a = 0.0f;
			this->mvp_objects[i]->SetColor(&color);
		}

		{
			int world = this->mp_game_manager->GetWorldIndex();
			int stage = this->mp_game_manager->GetStageIndex();

			if (world == 0 && stage == 0) {
				this->m_condition = TC_1_1;
				this->mvp_objects[this->mi_count % 2]->SetAnim(&TIPS_GOAL);
				this->mvp_tips.emplace_back(this->mvp_objects[this->mi_count % 2]);
			}
			else if (world == 0 && stage == 1) {
				this->m_condition = TC_1_2;
				this->mvp_objects[this->mi_count % 2]->SetAnim(&TIPS_FOLD_2);
				this->mvp_tips.emplace_back(this->mvp_objects[this->mi_count % 2]);
			}
			else {
				CSingleton<CMenuManager>::GetInstance()->BackMenuSceneEX(true);
			}
		}

		this->m_next_condition = MENU_PREPARE;

		break;
	case MENU_FADE_IN_ACT:
		break;
	case MENU_FADE_IN_END:
		break;
	case MENU_PREPARE:
	{
		Color color = this->mvp_objects[this->mi_count % 2]->GetColor();
		Transform transform = this->mvp_objects[this->mi_count % 2]->GetTransform();

		color.a += 0.05f;
		transform.scale.x = 2.0f - color.a;
		transform.scale.y = 2.0f - color.a;

		if (color.a >= 1.0f) {
			color.a = 1.0f;
			transform.scale.x = 1.0f;
			transform.scale.y = 1.0f;
			this->m_next_condition = MENU_ACT;

			this->mp_xaudio2->Start(SOUND_LABEL_SE_CLEAR);
		}

		this->mvp_objects[this->mi_count % 2]->SetColor(&color);
		this->mvp_objects[this->mi_count % 2]->SetTransform(&transform);
	}
	break;
	case MENU_ACT:
	{
		CInput* input = CSingleton<CInput>::GetInstance();

		bool is_change = false;

		if (input->GetKeyTrigger(VK_LBUTTON) ||
			input->GetKeyTrigger(VK_RIGHT) ||
			input->GetKeyTrigger('D') ||
			input->GetKeyTrigger(VK_RETURN) ||
			input->GetKeyTrigger(VK_SPACE) ||
			input->GetButtonTrigger(XI_BUTTON_RIGHT) ||
			input->GetButtonTrigger(XI_BUTTON_B)) {
			this->mi_count++;
			is_change = true;
		}
		else if (input->GetKeyTrigger(VK_ESCAPE) ||
			input->GetKeyTrigger(VK_LEFT) ||
			input->GetKeyTrigger('A') ||
			input->GetButtonTrigger(XI_BUTTON_LEFT) ||
			input->GetButtonTrigger(XI_BUTTON_A) ||
			input->GetButtonTrigger(XI_BUTTON_BACK) ||
			input->GetButtonTrigger(XI_BUTTON_START)) {
			if (this->mi_count > 0) {
				this->mi_count--;
				is_change = true;
			}
		}

		if (is_change) {
			Transform transform = this->mvp_objects[this->mi_count % 2]->GetTransform();
			transform.scale.x = 1.0f;
			transform.scale.y = 1.0f;
			this->mvp_objects[this->mi_count % 2]->SetTransform(&transform);

			Color color = this->mvp_objects[this->mi_count % 2]->GetColor();
			color.a = 1.0f;
			this->mvp_objects[this->mi_count % 2]->SetColor(&color);

			if (this->mvp_tips.size() >= 2) {
				this->mvp_tips[0] = this->mvp_tips[1];
				this->mvp_tips[1] = this->mvp_objects[this->mi_count % 2];
			}
			else {
				this->mvp_tips.emplace_back(this->mvp_objects[this->mi_count % 2]);
			}

			switch (this->m_condition)
			{
			case TC_1_2:
				if (this->mi_count > 0) {
					this->m_next_condition = MENU_END;
					this->mvp_tips.clear();
					this->mvp_tips.emplace_back(this->mvp_objects[(this->mi_count + 1) % 2]);
				}
				break;
			case TC_1_1:
			default:
				if (this->mi_count > 3) {
					this->m_next_condition = MENU_END;
					this->mvp_tips.clear();
					this->mvp_tips.emplace_back(this->mvp_objects[(this->mi_count + 1) % 2]);
				}
				else {
					switch (this->mi_count)
					{
					case 3:
						this->mvp_objects[this->mi_count % 2]->SetAnim(&TIPS_FOLD_1);
						break;
					case 2:
						this->mvp_objects[this->mi_count % 2]->SetAnim(&TIPS_TURN);
						break;
					case 1:
						this->mvp_objects[this->mi_count % 2]->SetAnim(&TIPS_BACK);
						break;
					case 0:
					default:
						this->mvp_objects[this->mi_count % 2]->SetAnim(&TIPS_GOAL);
						break;
					}
					//this->m_next_condition = MENU_PREPARE;
				}
				break;
			}
		}
	}
	break;
	case MENU_END:
	{
		Color color = this->mvp_objects[(this->mi_count + 1) % 2]->GetColor();

		color.a -= 0.05f;

		if (color.a <= 0.0f) {
			color.a = 0.0f;
			CSingleton<CMenuManager>::GetInstance()->BackMenuSceneEX(true);
		}

		this->mvp_objects[(this->mi_count + 1) % 2]->SetColor(&color);
	}
	break;
	case MENU_FADE_OUT_PREPARE:
		break;
	case MENU_FADE_OUT_ACT:
		break;
	case MENU_FADE_OUT_END:
		break;
	}
}

void CMTips::Draw()
{
	for (size_t i = 0; i < this->mvp_tips.size(); i++)
	{
		this->mvp_tips[i]->Draw();
	}
}

void CMTips::ColliderTest()
{
}
