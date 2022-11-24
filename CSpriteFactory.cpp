#include "CSpriteFactory.h"
#include "CDirect3D.h"
#include "CMainWindow.h"

#include "CSingleton.h"

CSpriteFactory::CSpriteFactory()
{
}

CSpriteFactory::~CSpriteFactory()
{
	for (size_t i = 0; i < this->m_sprites.size(); i++)
	{
		COM_SAFE_RELEASE(this->m_sprites[i].mSRV);
	}
}

void CSpriteFactory::CreateSprite(SpriteData * sprite, int sprite_id)
{
	if (sprite_id < 0 || sprite_id >= this->m_sprites.size()) {
		sprite_id = 0;
	}

	sprite->mSRV = this->m_sprites[sprite_id].mSRV;
	sprite->anim_max = this->m_sprites[sprite_id].anim_max;
	sprite->slice_x = this->m_sprites[sprite_id].slice_x;
	sprite->slice_y = this->m_sprites[sprite_id].slice_y;
	sprite->slice.u = this->m_sprites[sprite_id].slice.u;
	sprite->slice.v = this->m_sprites[sprite_id].slice.v;
	sprite->count_check = this->m_sprites[sprite_id].count_check;
}

void CSpriteFactory::LoadSprites(std::vector<IO_SpriteData>& sprite_data)
{
	for (size_t i = 0; i < this->m_sprites.size(); i++)
	{
		COM_SAFE_RELEASE(this->m_sprites[i].mSRV);
	}
	this->m_sprites.clear();

	CDirect3D* directx = CSingleton<CDirect3D>::GetInstance();
	CMainWindow* cmain = CSingleton<CMainWindow>::GetInstance();

	for (int i = 0; i < SPRITE_MAX; i++)
	{
		SpriteData new_data;

		HRESULT hr = directx->LoadTexture(sprite_data[i].filename.c_str(), &new_data.mSRV);
		std::string error_text = "画像読み込みに失敗しました。\nファイル名 : " + sprite_data[i].filename;
		if (cmain->ErrorCheck(hr, error_text.c_str())) break;

		new_data.anim_max = sprite_data[i].anim_max;
		new_data.slice_x = sprite_data[i].slice_x;
		new_data.slice_y = sprite_data[i].slice_y;

		new_data.slice.u = 1.0f / static_cast<float>(new_data.slice_x);
		new_data.slice.v = 1.0f / static_cast<float>(new_data.slice_y);

		new_data.count_check = sprite_data[i].count_check;

		this->m_sprites.emplace_back(new_data);
	}
}

void CSpriteFactory::LoadDefaultSprites()
{
	//IO_SpriteData sprite_data[SPRITE_MAX];

	std::vector<IO_SpriteData> sprite_data(SPRITE_MAX);

	std::string editor_file = "assets/Editor/Sprites/";
	std::string game_file = "assets/Game/Sprites/";

	sprite_data[SPRITE_TEST_0] = {
			editor_file + "test_Back.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_TEST_1] = {
			editor_file + "test_Back2.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_TEST_2] = {
			editor_file + "test_Back3.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_TEST_3] = {
			editor_file + "test_Back4.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_TEST_4] = {
			editor_file + "test_Back5.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_TEST_BUTTON_0] = {
			editor_file + "button_typeA.png",
			60, 10, 6,
			1.0f
	};
	sprite_data[SPRITE_TEST_BUTTON_W_0] = {
			editor_file + "button_typeA.png",
			60, 5, 6,
			1.0f
	};
	sprite_data[SPRITE_TEST_BUTTON_1] = {
			editor_file + "button_typeB.png",
			60, 10, 6,
			1.0f
	};
	sprite_data[SPRITE_TEST_BUTTON_W_1] = {
			editor_file + "button_typeB.png",
			60, 5, 6,
			1.0f
	};
	sprite_data[SPRITE_TEST_BUTTON_2] = {
			editor_file + "button_typeC.png",
			60, 10, 6,
			1.0f
	};
	sprite_data[SPRITE_TEST_BUTTON_W_2] = {
			editor_file + "button_typeC.png",
			60, 5, 6,
			1.0f
	};
	sprite_data[SPRITE_TEST_CHAR_0] = {
			editor_file + "char_typeA.png",
			120, 20, 6,
			1.0f
	};
	sprite_data[SPRITE_PLAYER_IDLE_00] = {
			game_file + "Player/Super_Low_Tobari_Idle00.png",
			60, 8, 8,
			2.0f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_IDLE_01] = {
			game_file + "Player/Super_Low_Tobari_Idle01.png",
			60, 8, 8,
			2.0f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_RUNTOIDLE] = {
			game_file + "Player/Super_Low_tobari_IdletoRun.png",
			16, 4, 4,
			0.5f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_RUN00] = {
			game_file + "Player/Super_Low_tobari_Run00.png",
			28, 5, 6,
			2.0f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_RUN01] = {
			game_file + "Player/Super_Low_tobari_Run01.png",
			28, 5, 6,
			2.0f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_IDLETORUN] = {
			game_file + "Player/Super_Low_tobari_Runtoidle.png",
			19, 4, 5,
			0.5f / 60.0f
	};
	//sprite_data[SPRITE_PLAYER_JUMP_UP] = {
	//		game_file + "Player/Super_Low_Tobari_JumpUp.png",
	//		11, 3, 4,
	//		0.5f / 60.0f
	//};
	sprite_data[SPRITE_PLAYER_JUMP] = {
			game_file + "Player/Super_Low_Tobari_Jump.png",
			13, 3, 5,
			0.5f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_JUMP_DOWN] = {
			game_file + "Player/Super_Low_Tobari_JumpDown.png",
			10, 3, 4,
			0.5f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_HIT] = {
			game_file + "Player/Tobari_Damage.png",
			12, 3, 4,
			0.5f / 60.0f
	};
	sprite_data[SPRITE_PLAYER_GAMEOVER] = {
			game_file + "Player/Tobari_GameOver.png",
			72, 8, 9,
			0.5f / 60.0f
	};
	sprite_data[SPRITE_FRAME] = {
			game_file + "cursor_frame.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_LABEL] = {
			game_file + "Label/label.png",
			5, 5, 1,
			1.0f
	};
	sprite_data[SPRITE_GOAL] = {
			game_file + "Common/torii.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_PIECE_0] = {
			game_file + "Common/piece_1.png",
			30, 10, 4,
			1.0f
	};
	sprite_data[SPRITE_PIECE_1_in] = {
			game_file + "Common/piece_2_incomplete.png",
			30, 10, 4,
			1.0f
	};
	sprite_data[SPRITE_PIECE_1_com] = {
			game_file + "Common/piece_2_complete.png",
			30, 10, 4,
			1.0f
	};
	sprite_data[SPRITE_PIECE_2_com] = {
			game_file + "Common/piece_3_complete.png",
			12, 4, 4,
			1.0f
	};
	sprite_data[SPRITE_RED_BULE] = {
			game_file + "Common/red_blue.png",
			4, 2, 2,
			1.0f
	};
	sprite_data[SPRITE_TORNADO] = {
			game_file + "Common/Tornade.png",
			34, 4, 9,
			1.0f / 20.0f
	};
	sprite_data[SPRITE_DHARMA] = {
			game_file + "Common/dharma.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_CANNON] = {
			game_file + "Common/cannon.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_BALL] = {
			game_file + "Common/ball.png",
			2, 2, 1,
			1.0f
	};
	sprite_data[SPRITE_LION_TOP] = {
			game_file + "Common/lion_top.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_LION_BOTTOM] = {
			game_file + "Common/lion_bottom.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_HP] = {
			game_file + "Common/HP.png",
			4, 1, 4,
			1.0f
	};
	sprite_data[SPRITE_UI_FRAME] = {
			game_file + "Common/kakera_waku.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_TITLE_LOGO] = {
			game_file + "Common/Title/title_logo.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_CURSOR] = {
			game_file + "Common/Cursor.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_BUTTON_FRAME] = {
			game_file + "Common/Line.png",
			2, 2, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_RETURN] = {
			game_file + "Common/Return.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_PAGE_FRAME] = {
			game_file + "Common/page_frame.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_BOOK_FRAME] = {
			game_file + "Common/book_frame.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_BUTTON] = {
			game_file + "Common/UI_Button.png",
			7, 4, 4,
			1.0f
	};
	sprite_data[SPRITE_UI_PAUSE_BACK] = {
			game_file + "Common/Pause.png",
			3, 1, 3,
			1.0f
	};
	sprite_data[SPRITE_UI_PAUSE_HEADER] = {
			game_file + "Common/PauseText.png",
			4, 1, 4,
			1.0f
	};
	sprite_data[SPRITE_UI_SOUND_BACK] = {
			game_file + "Common/Sound/back.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_SOUND_BAR] = {
			game_file + "Common/Sound/bar.png",
			2, 1, 2,
			1.0f
	};
	sprite_data[SPRITE_UI_SOUND_CURSOR] = {
			game_file + "Common/Sound/cursor.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_SOUND_FRAME] = {
			game_file + "Common/Sound/frame.png",
			1, 1, 1,
			1.0f
	};
	sprite_data[SPRITE_UI_GAME_OVER] = {
		game_file + "Common/GameOver.png",
		1, 1, 1,
		1.0f
	};
	sprite_data[SPRITE_UI_RULE] = {
		game_file + "Common/Rule.png",
		5, 2, 3,
		1.0f
	};

	this->LoadSprites(sprite_data);
}
