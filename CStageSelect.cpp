#include "CStageSelect.h"

#include "CDirect3D.h"
#include"CInput.h"

#include "CImage.h"
#include "CButton.h"
#include "CCursor.h"

#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"
#include "CMainWindow.h"
#include "CXAudio2.h"

#include "CSingleton.h"

#include <array>

namespace
{
	enum SELECT_BUTTON
	{
		SB_STAGE_1,
		SB_STAGE_2,
		SB_STAGE_3,
		SB_RETURN,
		SB_MAX
	};

	enum SELECT_SPRITE
	{
		SP_BACK,
		SP_W_BACK,
		SP_LOGO,
		SP_W_BUTTON,
		SP_S_BUTTON,
		SP_FRAME,
		SP_MAX
	};

	constexpr float BUTTON_POS_WIDTH = 361.0f;
	constexpr float BUTTON_POS_HEIGHT = 20.0f;

	constexpr float NONE_SELECT_COLOR = 0.5f;

	constexpr float LEFT_PIECE_SIZE = 144.0f;
	constexpr float RIGHT_PIECE_SIZE = 156.0f;
}

CStageSelect::CStageSelect()
{
}

CStageSelect::~CStageSelect()
{
}

void CStageSelect::Initialize()
{
	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	this->mf_world = -1;
	this->mf_stage = -1;

	this->m_condition = SC_WORLD_PREPARE;

	SAVE_DATA save = CSingleton<CGameManager>::GetInstance()->GetSave();

	CMainWindow* cmain = CSingleton<CMainWindow>::GetInstance();

	std::array<IO_SpriteData, SP_MAX> io_data = {
		"assets/Game/Sprites/Common/Select/Back.png",
		1, 1, 1,
		1.0f,
		"assets/Game/Sprites/Common/Select/WorldBack.png",
		3, 1, 3,
		1.0f,
		"assets/Game/Sprites/Common/Select/SelectLogo.png",
		1, 1, 1,
		1.0f,
		"assets/Game/Sprites/Common/Select/WorldButton.png",
		4, 4, 1,
		1.0f,
		"assets/Game/Sprites/Common/Select/StageButton.png",
		12, 4, 3,
		1.0f,
		"assets/Game/Sprites/Common/Select/frame.png",
		1, 1, 1,
		1.0f,
	};

	for (size_t i = 0; i < static_cast<size_t>(SP_MAX); i++)
	{
		SpriteData new_data;
		HRESULT hr = direct3d->LoadTexture(io_data[i].filename.c_str(), &new_data.mSRV);
		std::string error_text = "画像読み込みに失敗しました。\nファイル名 : " + io_data[i].filename;
		if (!cmain->ErrorCheck(hr, error_text.c_str())) {
			new_data.anim_max = io_data[i].anim_max;
			new_data.slice_x = io_data[i].slice_x;
			new_data.slice_y = io_data[i].slice_y;

			new_data.slice.u = 1.0f / static_cast<float>(new_data.slice_x);
			new_data.slice.v = 1.0f / static_cast<float>(new_data.slice_y);

			new_data.count_check = io_data[i].count_check;

			this->mv_sprite_data.emplace_back(new_data);
		};
	}

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_0);
		new_image->SetTexture(&this->mv_sprite_data[SP_BACK]);

		// カラー変更
		Color new_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		new_image->SetColor(&new_color);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(screen_width, screen_height);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
	}

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_0);
		new_image->SetTexture(&this->mv_sprite_data[SP_W_BACK]);

		// カラー変更
		Color new_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		new_image->SetColor(&new_color);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(1120.0f, 695.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_world_back = new_image;
	}

	// ロゴ画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_0);
		new_image->SetTexture(&this->mv_sprite_data[SP_LOGO]);

		// カラー変更
		Color new_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		new_image->SetColor(&new_color);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = -16.0f;
		transform.position.y = (screen_height / 2.0f) - 96.0f - (145.0 / 2.0f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(505.0f, 145.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
	}

	// ボタン画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_UI_RETURN);

		Color new_color = { 0.5f, 0.5f, 0.5f, 1.0f };
		new_image->SetColor(&new_color);

		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width / 2.0f) + 70.0f;
		transform.position.y = (screen_height / 2.0f) - 80.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(87.0f, 88.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_return_button = new_image;
	}

	// ボタン画像の追加
	for (size_t j = 0; j < 2; j++)
	{
		for (size_t i = 0; i < GAME_WORLD_MAX; i++)
		{
			// 画像用オブジェクトの生成
			CImage* new_image = new CImage;
			new_image->Initilize();

			// テクスチャ追加
			new_image->AddTexture(SPRITE_TEST_0);
			new_image->SetTexture(&this->mv_sprite_data[SP_W_BUTTON]);

			// カラー変更
			if (j != 0) {
				Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
				new_image->SetColor(&new_color);
			}
			else {
				Color new_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 0.0f };
				new_image->SetColor(&new_color);
			}

			// UV変更
			if (j != 0) {
				UV new_uv = { 3.0f, 0.0f };
				new_image->SetAnim(&new_uv);
			}
			else {
				UV new_uv = { static_cast<float>(i), 0.0f };
				new_image->SetAnim(&new_uv);
			}


			// 座標変更
			Transform transform = new_image->GetTransform();
			transform.position.x = (305.0f * static_cast<float>(i)) - 305.0f;
			transform.position.y = -62.0f;
			new_image->SetTransform(&transform);

			// 画像の大きさを変える
			new_image->SetDefaultPosAndColl(305.0f, 421.0f);

			// 画像オブジェクトの追加
			this->mvp_objects.emplace_back(new_image);
			if (j != 0) {
				this->map_world_frame[i] = new_image;
			}
			else {
				this->map_world_button[i] = new_image;
			}
		}
	}

	// ボタン画像の追加
	for (size_t j = 0; j < 2; j++)
	{
		for (size_t i = 0; i < GAME_STAGE_MAX; i++)
		{
			// 画像用オブジェクトの生成
			CImage* new_image = new CImage;
			new_image->Initilize();

			// テクスチャ追加
			new_image->AddTexture(SPRITE_TEST_0);
			new_image->SetTexture(&this->mv_sprite_data[SP_S_BUTTON]);

			// カラー変更
			if (j != 0) {
				Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
				new_image->SetColor(&new_color);
			}
			else {
				Color new_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 0.0f };
				new_image->SetColor(&new_color);
			}

			// UV変更
			if (j != 0) {
				UV new_uv = { 3.0f, 2.0f };
				new_image->SetAnim(&new_uv);
			}
			else {
				UV new_uv = { static_cast<float>(i), 0.0f };
				new_image->SetAnim(&new_uv);
			}

			// 座標変更
			Transform transform = new_image->GetTransform();
			transform.position.x = (302.0f * static_cast<float>(i)) - 302.0f;
			transform.position.y = 34.0f;
			new_image->SetTransform(&transform);

			// 画像の大きさを変える
			new_image->SetDefaultPosAndColl(255.0f, 190.0f);

			// 画像オブジェクトの追加
			this->mvp_objects.emplace_back(new_image);
			if (j != 0) {
				this->map_stage_frame[i] = new_image;
			}
			else {
				this->map_stage_button[i] = new_image;
			}
		}
	}

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_0);
		new_image->SetTexture(&this->mv_sprite_data[SP_FRAME]);

		// カラー変更
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		new_image->SetColor(&new_color);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = 8.0f;
		transform.position.y = -202.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(680.0f, 240.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_frame = new_image;
	}

	// カケラ画像の追加
	for (size_t i = 0; i < static_cast<size_t>(PIECE_NUM); i++)
	{
		for (size_t j = 0; j < static_cast<size_t>(PIECE_NUM); j++)
		{
			// 画像用オブジェクトの生成
			CImage* new_image = new CImage;
			new_image->Initilize();

			// テクスチャ追加
			new_image->AddTexture(SPRITE_PIECE_2_com);

			// カラー変更
			Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
			new_image->SetColor(&new_color);

			// UV変更
			UV new_uv = { static_cast<float>(j), static_cast<float>(i) };
			new_image->SetAnim(&new_uv);

			// 座標変更
			Transform transform = this->map_world_button[i]->GetTransform();
			transform.position.x += 92.0f;
			transform.position.y -= (9.0f + (62.0f * static_cast<float>(j)));
			new_image->SetTransform(&transform);

			// 画像の大きさを変える
			new_image->SetDefaultPosAndColl(60.0f, 60.0f);

			// 画像オブジェクトの追加
			this->mvp_objects.emplace_back(new_image);
			this->maap_stage_piece[i][j] = new_image;
		}
	}

	// カケラ画像の追加
	for (size_t i = 0; i < static_cast<size_t>(PIECE_NUM); i++)
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_PIECE_2_com);

		// カラー変更
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		new_image->SetColor(&new_color);

		// UV変更
		UV new_uv = { 3.0f, static_cast<float>(i) };
		new_image->SetAnim(&new_uv);

		// 座標変更
		Transform transform = this->map_world_button[i]->GetTransform();
		transform.position.x += 56.0f;
		transform.position.y += 109.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(130.0f, 130.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_world_piece.emplace_back(new_image);
	}

	// カケラ画像の追加
	for (size_t i = 0; i < static_cast<size_t>(PIECE_NUM); i++)
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_PIECE_1_in);

		// カラー変更
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		new_image->SetColor(&new_color);

		// UV変更
		UV new_uv = { static_cast<float>(i), 0.0f };
		new_image->SetAnim(&new_uv);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = 158.0f;
		transform.position.y = -205.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(RIGHT_PIECE_SIZE, RIGHT_PIECE_SIZE);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_piece.emplace_back(new_image);
	}

	// カケラ画像の追加
	for (size_t i = 0; i < static_cast<size_t>(PIECE_NUM); i++)
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_PIECE_1_in);

		// カラー変更
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		new_image->SetColor(&new_color);

		// UV変更
		UV new_uv = { static_cast<float>(i), 0.0f };
		new_image->SetAnim(&new_uv);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = (57.0f * static_cast<float>(i)) - 154.0f;
		transform.position.y = -212.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(LEFT_PIECE_SIZE, LEFT_PIECE_SIZE);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_piece.emplace_back(new_image);
	}

	// カーソル
	this->mp_cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();

	// カーソル位置決定
	this->mi_cursor_line = SB_STAGE_1;
	this->mi_old_cursor_line = this->mi_cursor_line;
	this->ColliderTest();
}

void CStageSelect::Finalize()
{
	for (size_t i = 0; i < this->mvp_objects.size(); i++)
	{
		this->mvp_objects[i]->UnInitilize();
		delete this->mvp_objects[i];
	}
	this->mvp_objects.clear();

	for (size_t i = 0; i < this->mv_sprite_data.size(); i++)
	{
		COM_SAFE_RELEASE(this->mv_sprite_data[i].mSRV);
	}

	this->mp_cursor = nullptr;
}

void CStageSelect::Update()
{
	if (!this->mb_initialized_flg) {
		this->mb_initialized_flg = true;
		this->mp_xaudio2->Start(SOUND_LABEL_BGM_SELECT);
		this->mp_xaudio2->XA_Setvolume(SOUND_LABEL_BGM_SELECT, 0.0f);
		this->mp_xaudio2->XA_SetFadein(SOUND_LABEL_BGM_SELECT, DEFAULT_MAX_VOLUME, DEFAULT_DIFF_VOLUME);
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();

	if (this->mp_input->GetKeyTrigger(VK_ESCAPE)) {
		menu_manager->ChangeMenuSceneEX(MENU_PAUSE, false);
		return;
	}

	switch (this->m_condition)
	{
	case SC_WORLD_PREPARE:
	{
		Color none_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 1.0f };
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
		color.a = this->map_world_button[0]->GetColor().a;

		color.a += 0.05f;
		none_color.a = color.a;

		if (color.a >= 1.0f) {
			color.a = 1.0f;
			none_color.a = 1.0f;

			this->m_condition = SC_WORLD_ACT;
		}

		for (size_t i = 0; i < this->map_world_button.size(); i++)
		{
			this->map_world_button[i]->SetColor(&none_color);
		}

		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < GAME_WORLD_MAX) {
			UV new_uv = { 0.0f, static_cast<float>(this->mi_cursor_line) };
			this->mp_world_back->SetAnim(&new_uv);
		}

		// セーブデータ
		SAVE_DATA save = CSingleton<CGameManager>::GetInstance()->GetSave();
		for (size_t i = 0; i < static_cast<size_t>(PIECE_NUM); i++)
		{
			for (size_t j = 0; j < static_cast<size_t>(PIECE_NUM); j++)
			{
				bool is_draw = true;
				for (size_t k = 0; k < save[i][j].size(); k++)
				{
					if (!save[i][j][k]) is_draw = false;
				}

				if (!is_draw) continue;

				if (i == this->mi_cursor_line) this->maap_stage_piece[i][j]->SetColor(&color);
				else this->maap_stage_piece[i][j]->SetColor(&none_color);
			}
		}

		for (size_t i = 0; i < this->mvp_world_piece.size(); i++)
		{
			bool is_draw = true;
			for (size_t j = 0; j < save[i].size(); j++)
			{
				for (size_t k = 0; k < save[i][j].size(); k++)
				{
					if (!save[i][j][k]) is_draw = false;
				}
			}

			if (!is_draw) continue;

			if (i == this->mi_cursor_line) this->mvp_world_piece[i]->SetColor(&color);
			else this->mvp_world_piece[i]->SetColor(&none_color);
		}

		none_color.a = 1.0f;
		this->mp_return_button->SetColor(&none_color);
		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			this->map_world_button[this->mi_cursor_line]->SetColor(&color);
			this->map_world_frame[this->mi_cursor_line]->SetColor(&color);
		}
		else {
			this->mp_return_button->SetColor(&color);
		}
	}
	break;
	case SC_WORLD_ACT:
	{
		POINT point = this->mp_input->GetCursorClientDefference();
		if (point.x != 0 || point.y != 0) {
			this->ColliderTest();
		}

		float axis = this->mp_input->GetAxisRawX(true);
		float check_axis = 0.0f;
		if (this->mf_old_axis != axis) check_axis = axis;
		if (this->mp_input->GetKeyTrigger('A') || this->mp_input->GetButtonTrigger(XI_BUTTON_LEFT) || check_axis < 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line--;
			if (this->mi_cursor_line < 0) {
				this->mi_cursor_line = SB_MAX - 1;
			}
		}
		else if (this->mp_input->GetKeyTrigger('D') || this->mp_input->GetButtonTrigger(XI_BUTTON_RIGHT) || check_axis > 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line++;
			this->mi_cursor_line %= SB_MAX;
		}
		else if (this->mp_input->GetKeyTrigger(VK_LBUTTON) || this->mp_input->GetButtonTrigger(XI_BUTTON_B)) {
			this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
			if (this->mi_cursor_line == SB_STAGE_1) {
				this->mf_world = 0;
				this->m_condition = SC_WORLD_END;
			}
			if (this->mi_cursor_line == SB_STAGE_2) {
				this->mf_world = 1;
				this->m_condition = SC_WORLD_END;
			}
			if (this->mi_cursor_line == SB_STAGE_3) {
				this->mf_world = 2;
				this->m_condition = SC_WORLD_END;
			}
			if (this->mi_cursor_line == SB_RETURN) {
				scene_manager->ChangeScene(SCENE_TITLE);
			}
		}
		this->mf_old_axis = axis;

		if (this->mi_old_cursor_line != this->mi_cursor_line) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_MOVE);
		}

		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < GAME_WORLD_MAX) {
			UV new_uv = { 0.0f, static_cast<float>(this->mi_cursor_line) };
			this->mp_world_back->SetAnim(&new_uv);
		}

		Color none_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 1.0f };
		Color color = { 1.0f, 1.0f, 1.0f, 0.0f };

		for (size_t i = 0; i < this->map_world_button.size(); i++)
		{
			this->map_world_button[i]->SetColor(&none_color);
		}
		for (size_t i = 0; i < this->map_world_button.size(); i++)
		{
			this->map_world_frame[i]->SetColor(&color);
		}
		this->mp_return_button->SetColor(&none_color);

		color.a = 1.0f;

		// セーブデータ
		SAVE_DATA save = CSingleton<CGameManager>::GetInstance()->GetSave();
		for (size_t i = 0; i < static_cast<size_t>(PIECE_NUM); i++)
		{
			for (size_t j = 0; j < static_cast<size_t>(PIECE_NUM); j++)
			{
				bool is_draw = true;
				for (size_t k = 0; k < save[i][j].size(); k++)
				{
					if (!save[i][j][k]) is_draw = false;
				}

				if (!is_draw) continue;

				if (i == this->mi_cursor_line) this->maap_stage_piece[i][j]->SetColor(&color);
				else this->maap_stage_piece[i][j]->SetColor(&none_color);
			}
		}

		for (size_t i = 0; i < this->mvp_world_piece.size(); i++)
		{
			bool is_draw = true;
			for (size_t j = 0; j < save[i].size(); j++)
			{
				for (size_t k = 0; k < save[i][j].size(); k++)
				{
					if (!save[i][j][k]) is_draw = false;
				}
			}

			if (!is_draw) continue;

			if (i == this->mi_cursor_line) this->mvp_world_piece[i]->SetColor(&color);
			else this->mvp_world_piece[i]->SetColor(&none_color);
		}

		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			this->map_world_button[this->mi_cursor_line]->SetColor(&color);
			this->map_world_frame[this->mi_cursor_line]->SetColor(&color);
		}
		else {
			this->mp_return_button->SetColor(&color);
		}
	}
	break;
	case SC_WORLD_END:
	{
		Color none_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 1.0f };
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
		color.a = this->map_world_button[0]->GetColor().a;

		color.a -= 0.05f;
		none_color.a = color.a;

		if (color.a <= 0.0f) {
			color.a = 0.0f;
			none_color.a = 0.0f;

			this->m_condition = SC_STAGE_PREPARE;
			// カーソル位置決定
			this->mi_cursor_line = SB_STAGE_1;
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->ColliderTest();
		}

		for (size_t i = 0; i < this->map_world_button.size(); i++)
		{
			this->map_world_button[i]->SetColor(&none_color);
		}

		// セーブデータ
		SAVE_DATA save = CSingleton<CGameManager>::GetInstance()->GetSave();
		for (size_t i = 0; i < static_cast<size_t>(PIECE_NUM); i++)
		{
			for (size_t j = 0; j < static_cast<size_t>(PIECE_NUM); j++)
			{
				bool is_draw = true;
				for (size_t k = 0; k < save[i][j].size(); k++)
				{
					if (!save[i][j][k]) is_draw = false;
				}

				if (!is_draw) continue;

				if (i == this->mi_cursor_line) this->maap_stage_piece[i][j]->SetColor(&color);
				else this->maap_stage_piece[i][j]->SetColor(&none_color);
			}
		}

		for (size_t i = 0; i < this->mvp_world_piece.size(); i++)
		{
			bool is_draw = true;
			for (size_t j = 0; j < save[i].size(); j++)
			{
				for (size_t k = 0; k < save[i][j].size(); k++)
				{
					if (!save[i][j][k]) is_draw = false;
				}
			}

			if (!is_draw) continue;

			if (i == this->mi_cursor_line) this->mvp_world_piece[i]->SetColor(&color);
			else this->mvp_world_piece[i]->SetColor(&none_color);
		}

		none_color.a = 1.0f;
		this->mp_return_button->SetColor(&none_color);
		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			this->map_world_button[this->mi_cursor_line]->SetColor(&color);
			this->map_world_frame[this->mi_cursor_line]->SetColor(&color);
		}
		else {
			this->mp_return_button->SetColor(&color);
		}
	}
	break;
	case SC_STAGE_PREPARE:
	{
		Color none_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 1.0f };
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
		color.a = this->map_stage_button[0]->GetColor().a;

		color.a += 0.05f;
		none_color.a = color.a;

		if (color.a >= 1.0f) {
			color.a = 1.0f;
			none_color.a = 1.0f;

			this->m_condition = SC_STAGE_ACT;
		}

		// セーブデータ
		SAVE_DATA save = CSingleton<CGameManager>::GetInstance()->GetSave();
		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			for (size_t i = 0; i < this->mvp_piece.size(); i++)
			{
				UV new_uv = { static_cast<float>((static_cast<int>(i) % PIECE_NUM) + (this->mi_cursor_line * PIECE_NUM)), static_cast<float>(this->mf_world) };
				this->mvp_piece[i]->SetAnim(&new_uv);

				if (save[this->mf_world][this->mi_cursor_line][i % PIECE_NUM]) {
					this->mvp_piece[i]->ChangeTexture(SPRITE_PIECE_1_com);
				}
				else {
					this->mvp_piece[i]->ChangeTexture(SPRITE_PIECE_1_in);
				}
			}
		}

		for (size_t i = 0; i < this->map_stage_button.size(); i++)
		{
			this->map_stage_button[i]->SetColor(&none_color);
			UV uv = this->map_stage_button[i]->GetAnim();
			uv.v = static_cast<float>(this->mf_world);
			this->map_stage_button[i]->SetAnim(&uv);
		}

		none_color.a = 1.0f;
		this->mp_return_button->SetColor(&none_color);
		this->mp_frame->SetColor(&color);
		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			this->map_stage_button[this->mi_cursor_line]->SetColor(&color);
			this->map_stage_frame[this->mi_cursor_line]->SetColor(&color);
		}
		else {
			this->mp_return_button->SetColor(&color);
		}
		for (size_t i = 0; i < this->mvp_piece.size(); i++)
		{
			this->mvp_piece[i]->SetColor(&color);
		}
	}
	break;
	case SC_STAGE_ACT:
	{
		POINT point = this->mp_input->GetCursorClientDefference();
		if (point.x != 0 || point.y != 0) {
			this->ColliderTest();
		}

		float axis = this->mp_input->GetAxisRawX(true);
		float check_axis = 0.0f;
		if (this->mf_old_axis != axis) check_axis = axis;
		if (this->mp_input->GetKeyTrigger('A') || this->mp_input->GetButtonTrigger(XI_BUTTON_LEFT) || check_axis < 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line--;
			if (this->mi_cursor_line < 0) {
				this->mi_cursor_line = SB_MAX - 1;
			}
		}
		else if (this->mp_input->GetKeyTrigger('D') || this->mp_input->GetButtonTrigger(XI_BUTTON_RIGHT) || check_axis > 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line++;
			this->mi_cursor_line %= SB_MAX;
		}
		else if (this->mp_input->GetKeyTrigger(VK_LBUTTON) || this->mp_input->GetButtonTrigger(XI_BUTTON_B)) {
			if (this->mi_cursor_line == SB_STAGE_1) {
				this->mp_xaudio2->Start(SOUND_LABEL_SE_START);
				this->mf_stage = 0;
				this->m_condition = SC_STAGE_END;
			}
			if (this->mi_cursor_line == SB_STAGE_2) {
				this->mp_xaudio2->Start(SOUND_LABEL_SE_START);
				this->mf_stage = 1;
				this->m_condition = SC_STAGE_END;
			}
			if (this->mi_cursor_line == SB_STAGE_3) {
				this->mp_xaudio2->Start(SOUND_LABEL_SE_START);
				this->mf_stage = 2;
				this->m_condition = SC_STAGE_END;
			}
			if (this->mi_cursor_line == SB_RETURN) {
				this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
				this->mf_world = -1;
				this->m_condition = SC_STAGE_END;
			}
		}
		this->mf_old_axis = axis;

		if (this->mi_old_cursor_line != this->mi_cursor_line) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_MOVE);
		}

		// セーブデータ
		SAVE_DATA save = CSingleton<CGameManager>::GetInstance()->GetSave();
		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			for (size_t i = 0; i < this->mvp_piece.size(); i++)
			{
				UV new_uv = { static_cast<float>((static_cast<int>(i) % PIECE_NUM) + (this->mi_cursor_line * PIECE_NUM)), static_cast<float>(this->mf_world) };
				this->mvp_piece[i]->SetAnim(&new_uv);

				if (save[this->mf_world][this->mi_cursor_line][i % PIECE_NUM]) {
					this->mvp_piece[i]->ChangeTexture(SPRITE_PIECE_1_com);
				}
				else {
					this->mvp_piece[i]->ChangeTexture(SPRITE_PIECE_1_in);
				}
			}
		}

		Color none_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 1.0f };
		Color color = { 1.0f, 1.0f, 1.0f, 0.0f };

		for (size_t i = 0; i < this->map_world_button.size(); i++)
		{
			this->map_stage_button[i]->SetColor(&none_color);
		}
		for (size_t i = 0; i < this->map_world_button.size(); i++)
		{
			this->map_stage_frame[i]->SetColor(&color);
		}
		this->mp_return_button->SetColor(&none_color);

		color.a = 1.0f;
		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			this->map_stage_button[this->mi_cursor_line]->SetColor(&color);
			this->map_stage_frame[this->mi_cursor_line]->SetColor(&color);
		}
		else {
			this->mp_return_button->SetColor(&color);
		}
	}
	break;
	case SC_STAGE_END:
	{
		Color none_color = { NONE_SELECT_COLOR, NONE_SELECT_COLOR, NONE_SELECT_COLOR, 1.0f };
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
		color.a = this->map_stage_button[0]->GetColor().a;

		color.a -= 0.05f;
		none_color.a = color.a;

		if (color.a <= 0.0f) {
			color.a = 0.0f;
			none_color.a = 0.0f;

			if (this->mf_world > -1 && this->mf_stage > -1) {
				game_manager->SetWorldIndex(this->mf_world);
				game_manager->SetStageIndex(this->mf_stage);
				scene_manager->ChangeScene(SCENE_STAGE_PLAY);
			}
			else {
				this->m_condition = SC_WORLD_PREPARE;

				// カーソル位置決定
				this->mi_cursor_line = SB_STAGE_1;
				this->mi_old_cursor_line = this->mi_cursor_line;
				this->ColliderTest();
			}
		}

		for (size_t i = 0; i < this->map_stage_button.size(); i++)
		{
			this->map_stage_button[i]->SetColor(&none_color);
		}
		none_color.a = 1.0f;
		this->mp_return_button->SetColor(&none_color);
		this->mp_frame->SetColor(&color);
		if (this->mi_cursor_line >= 0 && this->mi_cursor_line < SB_RETURN) {
			this->map_stage_button[this->mi_cursor_line]->SetColor(&color);
			this->map_stage_frame[this->mi_cursor_line]->SetColor(&color);
		}
		else {
			this->mp_return_button->SetColor(&color);
		}
		for (size_t i = 0; i < this->mvp_piece.size(); i++)
		{
			this->mvp_piece[i]->SetColor(&color);
		}
	}
	break;
	}
}

void CStageSelect::Draw()
{
	for (size_t i = 0; i < this->mvp_objects.size(); i++)
	{
		this->mvp_objects[i]->Draw();
	}
}

void CStageSelect::PauseUpdate()
{
}

void CStageSelect::PauseDraw()
{
	this->Draw();
}

void CStageSelect::ColliderTest()
{
	switch (this->m_condition)
	{
	case SC_WORLD_PREPARE:
	case SC_WORLD_ACT:
	case SC_WORLD_END:
	{
		for (size_t i = 0; i < this->map_world_button.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->map_world_button[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_line = static_cast<SELECT_BUTTON>(i);
				break;
			}
		}

		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mp_return_button->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_line = SB_RETURN;
				break;
			}
		}
	}
	break;
	case SC_STAGE_PREPARE:
	case SC_STAGE_ACT:
	case SC_STAGE_END:
	{
		for (size_t i = 0; i < this->map_stage_button.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->map_stage_button[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_line = static_cast<SELECT_BUTTON>(i);
				break;
			}
		}

		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mp_return_button->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_line = SB_RETURN;
				break;
			}
		}
	}
	break;
	}
}
