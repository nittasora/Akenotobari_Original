#include "CMPause.h"

#include "CDirect3D.h"
#include"CInput.h"

#include "CImage.h"
#include "CButton.h"
#include "CCursor.h"

#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"
#include "CXAudio2.h"

#include "CSingleton.h"

constexpr float BAR_LEFT = -191.0f;
constexpr float BAR_RIGHT = 309.0f;
constexpr float BAR_BOTTOM = -39.0f;
constexpr float BAR_TOP = -9.0f;

void CMPause::Initialize()
{
	this->mi_cursor_line = PB_OPTION;
	this->m_pause_condition = PAUSE_MAIN;

	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_2);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(screen_width, screen_height);

		Color new_color = { 0.0f, 0.0f, 0.0f, 0.5f };
		new_image->SetColor(&new_color);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_fade = new_image;
	}

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_PAUSE_BACK);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(1120.0f, 660.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_back = new_image;
	}

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_PAUSE_HEADER);

		Transform transform = new_image->GetTransform();
		transform.position.x = 560.0f - 226.0f;
		transform.position.y = 330.0f - 33.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(170.0f, 35.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_header = new_image;
	}

	for (size_t i = 0; i < PB_MAX; i++)
	{
		{
			// 画像用オブジェクトの生成
			CImage* new_image = new CImage;
			new_image->Initilize();

			new_image->AddTexture(SPRITE_UI_BUTTON_FRAME);

			UV new_uv = { 0.0f, 0.0f };

			switch (i)
			{
			case PB_BACK:
				new_uv.u = 1.0f;
				break;
			}

			new_image->SetAnim(&new_uv);

			Transform transform = new_image->GetTransform();
			transform.position.x = 64.0f;
			transform.position.y = -13.0f + 150.0f - (100.0f * static_cast<float>(i));
			new_image->SetTransform(&transform);

			// 画像の大きさを変える
			new_image->SetDefaultPosAndColl(350.0f, 90.0f);

			// 画像オブジェクトの追加
			this->mvp_objects.emplace_back(new_image);
			this->mvp_frame.emplace_back(new_image);
		}

		{
			// 画像用オブジェクトの生成
			CImage* new_image = new CImage;
			new_image->Initilize();

			new_image->AddTexture(SPRITE_UI_BUTTON);

			UV new_uv = { 0.0f, 0.0f };

			switch (i)
			{
			case PB_OPTION:
				new_uv.u = 1.0f;
				break;
			case PB_RULE:
				new_uv.u = 2.0f;
				break;
			case PB_SELECT:
				new_uv.u = 3.0f;
				break;
			case PB_BACK:
				new_uv.u = 3.0f;
				new_uv.v = 1.0f;
				break;
			}

			new_image->SetAnim(&new_uv);

			Transform transform = new_image->GetTransform();
			transform.position.x = 64.0f;
			transform.position.y = -13.0f + 150.0f - (100.0f * static_cast<float>(i));
			new_image->SetTransform(&transform);

			// 画像の大きさを変える
			new_image->SetDefaultPosAndColl(345.0f, 75.0f);

			// 画像オブジェクトの追加
			this->mvp_objects.emplace_back(new_image);
			this->mvp_button.emplace_back(new_image);
		}

		{
			// 画像用オブジェクトの生成
			CImage* new_image = new CImage;
			new_image->Initilize();

			new_image->AddTexture(SPRITE_UI_CURSOR);

			Transform transform = new_image->GetTransform();
			switch (i)
			{
			case PB_BACK:
				transform.position.x = 64.0f - 25.0f - 115.0f - 20.0f;
				break;
			default:
				transform.position.x = 64.0f - 25.0f - 175.0f - 20.0f;
				break;
			}
			transform.position.y = -13.0f + 150.0f - (100.0f * static_cast<float>(i));
			new_image->SetTransform(&transform);

			// 画像の大きさを変える
			new_image->SetDefaultPosAndColl(50.0f, 53.0f);

			// 画像オブジェクトの追加
			this->mvp_objects.emplace_back(new_image);
			this->mvp_cursor.emplace_back(new_image);
		}
	}

	// ボタン画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_RETURN);

		Transform transform = new_image->GetTransform();
		transform.position.x = -320.0f;
		transform.position.y = 202.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(66.0f, 66.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_rule_back = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_BUTTON_FRAME);

		UV new_uv = { 1.0f, 0.0f };
		new_image->SetAnim(&new_uv);

		Transform transform = new_image->GetTransform();
		transform.position.x = 64.0f;
		transform.position.y = -13.0f + 150.0f - (100.0f * 3.5f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(350.0f, 90.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_sound_frame = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_BUTTON);

		UV new_uv = { 3.0f, 1.0f };
		new_image->SetAnim(&new_uv);

		Transform transform = new_image->GetTransform();
		transform.position.x = 64.0f;
		transform.position.y = -13.0f + 150.0f - (100.0f * 3.5f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(345.0f, 75.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_sound_back = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_SOUND_BACK);

		Transform transform = new_image->GetTransform();
		transform.position.x = 59.0f;
		transform.position.y = 20.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(750.0f, 325.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_sound_obj.emplace_back(new_image);
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_SOUND_BAR);

		UV new_uv = { 0.0f, 1.0f };
		new_image->SetAnim(&new_uv);

		Transform transform = new_image->GetTransform();
		transform.position.x = 59.0f;
		transform.position.y = -44.0f + 20.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(500.0f, 30.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_sound_obj.emplace_back(new_image);
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_SOUND_BAR);

		Transform transform = new_image->GetTransform();
		transform.position.x = 59.0f;
		transform.position.y = -44.0f + 20.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(500.0f, 30.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_sound_bar = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_SOUND_FRAME);

		Transform transform = new_image->GetTransform();
		transform.position.x = 59.0f;
		transform.position.y = -44.0f + 20.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(665.0f, 50.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_sound_obj.emplace_back(new_image);
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_SOUND_CURSOR);

		Transform transform = new_image->GetTransform();
		transform.position.x = 59.0f;
		transform.position.y = -44.0f + 20.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(65.0f, 65.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_sound_cursor = new_image;
	}

	//{
	//	// 画像用オブジェクトの生成
	//	CImage* new_image = new CImage;
	//	new_image->Initilize();

	//	// テクスチャ追加
	//	new_image->AddTexture(SPRITE_UI_RULE);

	//	Transform transform = new_image->GetTransform();
	//	transform.scale.x = 2.0f;
	//	transform.scale.y = 2.0f;
	//	//transform.position.x = 59.0f;
	//	//transform.position.y = -44.0f + 20.0f;
	//	new_image->SetTransform(&transform);

	//	// 画像の大きさを変える
	//	new_image->SetDefaultPosAndColl(971.0f, 659.0f);

	//	// 画像オブジェクトの追加
	//	this->mvp_objects.emplace_back(new_image);
	//	this->mp_rule = new_image;
	//}

	// カーソル
	this->mp_cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();
}

void CMPause::Finalize()
{
	for (size_t i = 0; i < this->mvp_objects.size(); i++)
	{
		this->mvp_objects[i]->UnInitilize();
		delete this->mvp_objects[i];
	}
	this->mvp_objects.clear();

	this->mp_cursor = nullptr;
}

void CMPause::Update()
{
	if (this->m_current_condition != this->m_next_condition) this->m_current_condition = this->m_next_condition;

	switch (this->m_current_condition)
	{
	case MENU_FADE_IN_PREPARE:
	{
		for (size_t i = 0; i < this->mvp_objects.size(); i++)
		{
			Color color = this->mvp_objects[i]->GetColor();
			color.a = 0.0f;
			this->mvp_objects[i]->SetColor(&color);
		}
		this->mi_cursor_line = PB_OPTION;
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->m_next_condition = MENU_FADE_IN_ACT;
	}
	break;
	case MENU_FADE_IN_ACT:
	{
		Color new_color = this->mp_back->GetColor();
		new_color.a += 0.1f;

		if (new_color.a >= 1.0f) {
			new_color.a = 1.0f;
			this->m_pause_condition = PAUSE_MAIN;
			this->mf_old_axis = 0;
			this->m_next_condition = MENU_ACT;
		}

		for (size_t i = 0; i < this->mvp_button.size(); i++)
		{
			this->mvp_button[i]->SetColor(&new_color);
		}

		this->mp_header->SetColor(&new_color);
		this->mp_back->SetColor(&new_color);
		this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);
		this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);

		new_color.r = 0.0f;
		new_color.g = 0.0f;
		new_color.b = 0.0f;
		new_color.a /= 2.0f;

		this->mp_fade->SetColor(&new_color);
	}
	break;
	case MENU_FADE_IN_END:
	case MENU_PREPARE:
	{
		Color new_color = { 0.0f, 0.0f, 0.0f, 0.0f };
		switch (this->m_pause_condition)
		{
		case PAUSE_MAIN:
		{
			UV new_uv = { 0.0f, 0.0f };
			this->mp_back->SetAnim(&new_uv);
		}

		new_color = this->mvp_button[0]->GetColor();
		new_color.a += 0.1f;

		{
			UV new_uv = { 0.0f, 0.0f };
			this->mp_header->SetAnim(&new_uv);
		}

		if (new_color.a >= 1.0f) {
			new_color.a = 1.0f;
			this->m_next_condition = MENU_ACT;
		}

		for (size_t i = 0; i < this->mvp_button.size(); i++)
		{
			this->mvp_button[i]->SetColor(&new_color);
		}
		this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);
		this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);

		if (this->mp_back->GetColor().a < 1.0f) {
			this->mp_back->SetColor(&new_color);
		}

		new_color.a = 1.0f - new_color.a;

		if (this->mp_rule_back->GetColor().a > 0.0f) {
			this->mp_rule_back->SetColor(&new_color);
		}

		if (this->mvp_sound_obj[0]->GetColor().a > 0.0f) {
			for (size_t i = 0; i < this->mvp_sound_obj.size(); i++)
			{
				this->mvp_sound_obj[i]->SetColor(&new_color);
			}
			this->mp_sound_bar->SetColor(&new_color);
			this->mp_sound_cursor->SetColor(&new_color);
			this->mp_sound_back->SetColor(&new_color);
		}
		//if (this->mp_rule->GetColor().a > 0.0f) {
		//	Transform transform = this->mp_rule->GetTransform();
		//	transform.scale.x = 2.0f - new_color.a;
		//	transform.scale.y = 2.0f - new_color.a;

		//	if (new_color.a <= 0.0f) {
		//		new_color.a = 0.0f;
		//		transform.scale.x = 2.0f;
		//		transform.scale.y = 2.0f;
		//	}

		//	this->mp_rule->SetTransform(&transform);
		//	this->mp_rule->SetColor(&new_color);
		//}

		break;
		case PAUSE_OPTION:
			new_color = this->mp_sound_cursor->GetColor();
			new_color.a += 0.1f;

			{
				UV new_uv = { 0.0f, 3.0f };
				this->mp_header->SetAnim(&new_uv);
			}

			if (new_color.a >= 1.0f) {
				new_color.a = 1.0f;
				this->m_next_condition = MENU_ACT;
				this->mb_cursor_move = false;
			}

			for (size_t i = 0; i < this->mvp_sound_obj.size(); i++)
			{
				this->mvp_sound_obj[i]->SetColor(&new_color);
			}
			this->mp_sound_bar->SetColor(&new_color);
			this->mp_sound_cursor->SetColor(&new_color);
			this->mp_sound_back->SetColor(&new_color);

			{
				float volume = CSingleton<CXAudio2>::GetInstance()->XA_GetCommonVolume();
				float dis = (BAR_RIGHT - BAR_LEFT) * volume;

				Transform transform = this->mp_sound_cursor->GetTransform();
				transform.position.x = BAR_LEFT + dis;
				this->mp_sound_cursor->SetTransform(&transform);

				this->mp_sound_bar->SetImageAndColl(BAR_LEFT, transform.position.x, BAR_BOTTOM, BAR_TOP);
			}

			if (this->mp_back->GetColor().a < 1.0f) {
				this->mp_back->SetColor(&new_color);
			}

			new_color.a = 1.0f - new_color.a;

			if (this->mvp_button[0]->GetColor().a > 0.0f) {
				for (size_t i = 0; i < this->mvp_button.size(); i++)
				{
					this->mvp_button[i]->SetColor(&new_color);
				}
				this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);
				this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);
			}

			break;
		case PAUSE_RULE:
		{
			new_color = this->mvp_button[0]->GetColor();
			new_color.a -= 0.1f;

			{
				UV new_uv = { 0.0f, 2.0f };
				this->mp_header->SetAnim(&new_uv);
			}

			if (new_color.a <= 0.0f) {
				UV new_uv = { 0.0f, 1.0f };
				new_color.a = 0.0f;

				this->m_next_condition = MENU_ACT;

				//if (this->mp_input->CheckConeectedXInput()) {
				//	new_uv.v = 2.0f;
				//}
				//this->mp_back->SetAnim(&new_uv);
			}

			for (size_t i = 0; i < this->mvp_button.size(); i++)
			{
				this->mvp_button[i]->SetColor(&new_color);
			}
			this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);
			this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);

			new_color.a = 1.0f - new_color.a;
			this->mp_rule_back->SetColor(&new_color);

			//new_color = this->mp_rule->GetColor();
			//Transform transform = this->mp_rule->GetTransform();
			//new_color.a += 0.1f;
			//transform.scale.x = 2.0f - new_color.a;
			//transform.scale.y = 2.0f - new_color.a;

			//if (new_color.a >= 1.0f) {
			//	new_color.a = 1.0f;
			//	transform.scale.x = 1.0f;
			//	transform.scale.y = 1.0f;
			//	this->m_next_condition = MENU_ACT;
			//}

			//this->mp_rule->SetTransform(&transform);
			//this->mp_rule->SetColor(&new_color);

			//new_color.a = 1.0f - new_color.a;

			//if (this->mp_back->GetColor().a > 0.0f) {
			//	this->mp_back->SetColor(&new_color);
			//	for (size_t i = 0; i < this->mvp_button.size(); i++)
			//	{
			//		this->mvp_button[i]->SetColor(&new_color);
			//	}
			//	this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);
			//	this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);
			//}
		}
		break;
		}
	}
	break;
	case MENU_ACT:
	{
		switch (this->m_pause_condition)
		{
		case PAUSE_MAIN:
			this->MainUpdate();
			break;
		case PAUSE_OPTION:
			this->OptionUpdate();
			break;
		case PAUSE_RULE:
			this->RuleUpdate();
			break;
		}
	}
	break;
	case MENU_END:
		break;
	case MENU_FADE_OUT_PREPARE:
		break;
	case MENU_FADE_OUT_ACT:
	{
		Color new_color = this->mp_back->GetColor();
		new_color.a -= 0.1f;

		if (new_color.a <= 0.0f) {
			new_color.a = 0.0f;
			this->m_next_condition = MENU_FADE_OUT_END;
		}

		for (size_t i = 0; i < this->mvp_button.size(); i++)
		{
			this->mvp_button[i]->SetColor(&new_color);
		}

		this->mp_header->SetColor(&new_color);
		this->mp_back->SetColor(&new_color);
		this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);
		this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);

		new_color.r = 0.0f;
		new_color.g = 0.0f;
		new_color.b = 0.0f;
		new_color.a /= 2.0f;

		this->mp_fade->SetColor(&new_color);
	}
	break;
	case MENU_FADE_OUT_END:
	{
		CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();
		menu_manager->BackMenuSceneEX(true);
	}
	break;
	}

	this->ColliderTest();
}

void CMPause::Draw()
{
	for (size_t i = 0; i < this->mvp_objects.size(); i++)
	{
		this->mvp_objects[i]->Draw();
	}
}

void CMPause::ColliderTest()
{
}

void CMPause::MainUpdate()
{
	for (size_t i = 0; i < this->mvp_cursor.size(); i++)
	{
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		this->mvp_cursor[i]->SetColor(&new_color);
	}
	for (size_t i = 0; i < this->mvp_frame.size(); i++)
	{
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		this->mvp_frame[i]->SetColor(&new_color);
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	float axis = this->mp_input->GetAxisRawY(true);
	float check_axis = 0.0f;
	if (this->mf_old_axis != axis) check_axis = axis;
	if (this->mp_input->GetKeyTrigger('W') || this->mp_input->GetButtonTrigger(XI_BUTTON_UP) || check_axis > 0.0f) {
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->mi_cursor_line--;
		if (this->mi_cursor_line < PB_OPTION) {
			this->mi_cursor_line = PB_BACK;
		}
	}
	else if (this->mp_input->GetKeyTrigger('S') || this->mp_input->GetButtonTrigger(XI_BUTTON_DOWN) || check_axis < 0.0f) {
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->mi_cursor_line++;
		if (this->mi_cursor_line > PB_BACK) {
			this->mi_cursor_line = PB_OPTION;
		}
	}
	else if (this->mp_input->GetKeyTrigger(VK_LBUTTON) || this->mp_input->GetButtonTrigger(XI_BUTTON_B)) {
		this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
		if (this->mi_cursor_line == PB_OPTION) {
			this->m_pause_condition = PAUSE_OPTION;
			this->m_next_condition = MENU_PREPARE;
		}
		if (this->mi_cursor_line == PB_RULE) {
			//UV new_uv = { 0.0f, 1.0f };
			//if (this->mp_input->CheckConeectedXInput()) {
			//	new_uv.v = 2.0f;
			//}
			//this->mp_back->SetAnim(&new_uv);
			//this->mp_rule->SetAnim(&new_uv);

			this->m_pause_condition = PAUSE_RULE;
			this->m_next_condition = MENU_PREPARE;
		}
		else if (this->mi_cursor_line == PB_SELECT) {
			scene_manager->ChangeScene(SCENE_STAGE_SELECT);
			this->m_next_condition = MENU_END;
		}
		else if (this->mi_cursor_line == PB_BACK) {
			this->m_next_condition = MENU_FADE_OUT_ACT;
		}
	}
	else if (this->mp_input->GetKeyTrigger(VK_ESCAPE) || this->mp_input->GetButtonTrigger(XI_BUTTON_START) || this->mp_input->GetButtonTrigger(XI_BUTTON_BACK) || this->mp_input->GetButtonTrigger(XI_BUTTON_A)) {
		this->m_next_condition = MENU_FADE_OUT_ACT;
	}
	this->mf_old_axis = axis;

	POINT point = this->mp_input->GetCursorClientDefference();
	if (point.x != 0 || point.y != 0) {
		for (size_t i = 0; i < this->mvp_button.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_button[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_line = static_cast<PAUSE_BUTTON>(i);
				break;
			}
		}
	}

	if (this->mi_old_cursor_line != this->mi_cursor_line) {
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_MOVE);
	}

	Color new_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);
	this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);
}

void CMPause::OptionUpdate()
{
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();

	Transform transform = this->mp_sound_cursor->GetTransform();

	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mp_sound_cursor->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mb_cursor_move = true;
		}
	}

	{
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		this->mp_sound_frame->SetColor(&new_color);

		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mp_sound_back->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
				this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
				this->m_pause_condition = PAUSE_MAIN;
				this->m_next_condition = MENU_PREPARE;
			}
			else {
				new_color.a = 1.0f;
				this->mp_sound_frame->SetColor(&new_color);
			}
		}
	}

	if (this->mp_input->GetKeyUp(VK_LBUTTON)) {
		this->mb_cursor_move = false;
	}

	float axis = this->mp_input->GetAxisRawX(true);
	if (this->mb_cursor_move) {
		transform.position.x = this->mp_cursor->GetTransform().position.x;
	}
	if (this->mp_input->GetKeyPress('A') || this->mp_input->GetButtonPress(XI_BUTTON_LEFT) || axis < 0.0f) {
		transform.position.x--;
	}
	else if (this->mp_input->GetKeyPress('D') || this->mp_input->GetButtonPress(XI_BUTTON_RIGHT) || axis > 0.0f) {
		transform.position.x++;
	}
	else if (this->mp_input->GetKeyTrigger(VK_ESCAPE) || this->mp_input->GetButtonTrigger(XI_BUTTON_START) || this->mp_input->GetButtonTrigger(XI_BUTTON_BACK) || this->mp_input->GetButtonTrigger(XI_BUTTON_A) || this->mp_input->GetButtonTrigger(XI_BUTTON_B)) {
		this->m_pause_condition = PAUSE_MAIN;
		this->m_next_condition = MENU_PREPARE;
	}
	this->mf_old_axis = axis;

	if (transform.position.x < BAR_LEFT) transform.position.x = BAR_LEFT;
	if (transform.position.x > BAR_RIGHT) transform.position.x = BAR_RIGHT;

	float new_volume = (transform.position.x - BAR_LEFT) / (BAR_RIGHT - BAR_LEFT);
	audio->XA_SetCommonVolume(new_volume);
	CSingleton<CGameManager>::GetInstance()->SavePiece();

	this->mp_sound_cursor->SetTransform(&transform);

	this->mp_sound_bar->SetImageAndColl(BAR_LEFT, transform.position.x, BAR_BOTTOM, BAR_TOP);
}

void CMPause::RuleUpdate()
{
	UV new_uv = { 0.0f, 1.0f };
	if (this->mp_input->CheckConeectedXInput()) {
		new_uv.v = 2.0f;
	}
	this->mp_back->SetAnim(&new_uv);

	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mp_rule_back->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
				this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
				this->m_pause_condition = PAUSE_MAIN;
				this->m_next_condition = MENU_PREPARE;
			}
		}
	}

	if (this->mp_input->GetKeyTrigger(VK_ESCAPE) || this->mp_input->GetButtonTrigger(XI_BUTTON_START) || this->mp_input->GetButtonTrigger(XI_BUTTON_BACK) || this->mp_input->GetButtonTrigger(XI_BUTTON_A) || this->mp_input->GetButtonTrigger(XI_BUTTON_B)) {
		this->m_pause_condition = PAUSE_MAIN;
		this->m_next_condition = MENU_PREPARE;
	}
}
