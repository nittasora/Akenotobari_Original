#include "CStageEditor.h"

#include <array>
#include <fstream>
#include <iostream>
#include <string>

#include "CDirect3D.h"
#include "CInput.h"
#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"

#include "StageData.h"

#include "CImage.h"
#include "CButton.h"
#include "CCursor.h"
#include "CPage.h"
#include "CPlayer.h"
#include "CEditorText.h"
#include "CInputParameter.h"

#include "CSingleton.h"

namespace
{
	// メイン画面のボタンの役割
	enum STAGE_EDITOR_BUTTON
	{
		SEB_SAVE,				// ステージ保存
		SEB_BACK,				// 選択画面へ
		SEB_WORLD_LEFT,			// ワールド番号--
		SEB_WORLD_MAX_LEFT,		// 最大ワールド番号--
		SEB_WORLD_RIGHT,		// ワールド番号++
		SEB_WORLD_MAX_RIGHT,	// 最大ワールド番号++
		SEB_STAGE_LEFT,			// ステージ番号--
		SEB_STAGE_MAX_LEFT,		// 最大ステージ番号--
		SEB_STAGE_RIGHT,		// ステージ番号++
		SEB_STAGE_MAX_RIGHT,	// 最大ステージ番号++
		SEB_ADD_SPRITE,			// 画像追加
		SEB_SPRITE_OPTION,		// 画像設定
		SEB_MAPCHIP,			// マップチップ選択
		SEB_GOAL,				// ゴール選択 -> ギミック統合により廃止
		SEB_BACK_PAGE,			// ステージの背景設定
		SEB_BACK_OBJECT,		// ステージの背景オブジェクト選択 -> 仕様変更により廃止
		SEB_PLAYER,				// プレイヤーの設定
		SEB_GIMMICK,			// ギミックの選択
		SEB_PIECE,				// カケラの設定 -> ギミック統合により廃止
		SEB_FRONT_BACK,			// ステージを裏返す
		SEB_ROTATE_RIGHT,		// アイテムを右に90°回転
		SEB_ROTATE_LEFT,		// アイテムを左に90°回転
		SEB_PAINT,				// 追加モード
		SEB_ERASE,				// 削除モード
		SEB_MAX
	};

	// 選択画面のボタンの役割
	enum SPRITE_SELECT_BUTTON
	{
		SSB_BACK,				// メイン画面に戻る
		SSB_MAX
	};

	// 数字の役割
	enum VELUE_TEXT
	{
		VT_WORLD_10,			// ワールド番号の10の位
		VT_WORLD_01,			// ワールド番号の1の位
		VT_WORLD_MAX_10,		// 最大ワールド番号の10の位
		VT_WORLD_MAX_01,		// 最大ワールド番号の1の位
		VT_STAGE_10,			// ステージ番号の10の位
		VT_STAGE_01,			// ステージ番号の1の位
		VT_STAGE_MAX_10,		// 最大ステージ番号の10の位
		VT_STAGE_MAX_01,		// 最大ステージ番号の1の位
		VT_MAX
	};

	// 描画モード
	enum EDIT_VIEW
	{
		EV_NONE = 0,			// 通常
		EV_SPRITE_SELECT,		// 画像選択
		EV_GIMMIK_SELECT,		// ギミック選択
		EV_MAX
	};

	// エディットモード
	enum EDIT_MODE
	{
		EM_NONE = 0,			// 無し
		EM_SPRITE_OPTION,		// 画像設定
		EM_MAPCHIP_SELECT,		// マップチップ選択
		EM_MAPCHIP,				// マップチップ配置
		EM_BACK_SELECT,			// 背景選択
		EM_GIMMIK,				// ギミック配置
		EM_GIMMIK_SELECT,		// ギミック選択
		EM_MAX
	};

	// ギミック選択用
	enum GIMMIK_SELECT_IMAGE
	{
		GSI_GOAL,				// ゴール
		GSI_PIECE_1,			// カケラ1つめ
		GSI_PIECE_2,			// カケラ2つめ
		GSI_PIECE_3,			// カケラ3つめ
		GSI_RED_BLOCK,			// 赤ブロック
		GSI_BLUE_BLOCK,			// 青ブロック
		GSI_TORNADO,			// 竜巻
		GSI_DHARMA,				// ダルマ
		GSI_CANNON,				// 大砲
		GSI_LION_TOP,			// リフト上
		GSI_LION_BOTTOM,		//リフト下
		GSI_MAX
	};

	// ワールド・ステージ最大数
	constexpr int WORLD_MAX = 25;
	constexpr int STAGE_MAX = 25;

	// デフォルトのマップチップなどの画像番号
	constexpr int DEFAULT_CHIP_SPRITE = SPRITE_TEST_3;
	constexpr int DEFAULT_FRONT_SPRITE = SPRITE_TEST_2;
	constexpr int DEFAULT_BACK_SPRITE = SPRITE_TEST_1;

	// デフォルトのマップチップなどの画像
	SpriteData g_default_chip;
	SpriteData g_default_front;
	SpriteData g_default_back;

	// 画像データ
	std::vector <IO_SpriteDataEX> gv_io_sprite_data;
	std::vector <SpriteData> gv_sprite_data;

	// ステージのデータ
	std::array<std::array<StageData, STAGE_MAX>, WORLD_MAX> ga_stage_data;

	// "WORLD"になるUV
	std::array<UV, 5> ga_world_text = {
		2.0f, 2.0f,
		14.0f, 1.0f,
		17.0f, 1.0f,
		11.0f, 1.0f,
		3.0f, 1.0f
	};

	// "STAGE"になるUV
	std::array<UV, 5> ga_stage_text = {
		18.0f, 1.0f,
		19.0f, 1.0f,
		0.0f, 1.0f,
		6.0f, 1.0f,
		4.0f, 1.0f
	};

	// " MAX "になるUV
	std::array<UV, 5> ga_max_text = {
		19.0f, 5.0f,
		12.0f, 1.0f,
		0.0f, 1.0f,
		3.0f, 2.0f,
		19.0f, 5.0f
	};

	// 現在のワールド・ステージ番号
	int gi_world_index;
	int gi_stage_index;

	// 現在のワールド・ステージ最大値
	int gi_world_max;
	int gi_stage_max;

	// 表向きか裏向きか
	bool gb_is_front;

	// 描画・エディットモード
	EDIT_VIEW g_view_mode;
	EDIT_MODE g_edit_mode;

	// 配置するマップチップのデータ
	MapChipData g_chip_brush;
}

CStageEditor::CStageEditor()
{
}

CStageEditor::~CStageEditor()
{
}

void CStageEditor::Initialize()
{
	// モードは通常
	g_view_mode = EV_NONE;
	g_edit_mode = EM_NONE;

	// 配置はなし
	g_chip_brush.mi_chip_id = MAPCHIP_SPRITE_NONE;
	g_chip_brush.mf_angle_z = 0.0f;
	g_chip_brush.m_tag = TAG_MAPCHIP;

	// 一応
	this->mvp_images.clear();
	this->mvp_main_buttons.clear();

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
		Color image_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		new_image->SetColor(&image_color);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(screen_width, screen_height);

		// 画像オブジェクトの追加
		this->mvp_images.emplace_back(new_image);
	}

	// マップチップ画像オブジェクトの生成
	for (int y = 0; y < MAPCHIP_NUM_HEIGHT; y++) {
		for (int x = 0; x < MAPCHIP_NUM_WIDTH * AREA_NUM; x++) {
			// 画像用オブジェクトの生成
			CImage* new_image = new CImage;
			new_image->Initilize();

			// テクスチャ追加
			new_image->AddTexture(SPRITE_TEST_3);

			// 画像の大きさを変える
			new_image->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

			// 座標変更
			Transform transform = new_image->GetTransform();
			transform.position.x = MAPCHIP_WIDTH * (-(static_cast<float>(MAPCHIP_NUM_WIDTH) * static_cast<float>(AREA_NUM)) / 2.0f + 0.5f + static_cast<float>(x));
			transform.position.y = MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f + 0.5f + static_cast<float>(y));
			new_image->SetTransform(&transform);

			// 画像オブジェクトの追加
			this->mvp_mapchip.emplace_back(new_image);
		}
	}

	// "WORLD"文字生成
	for (size_t i = 0; i < ga_world_text.size(); i++)
	{
		// 画像用オブジェクトの生成
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// テクスチャ追加
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_world_text[i]);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - 10.0f;
		new_image->SetTransform(&transform);

		// 画像オブジェクトの追加
		this->mvp_text.emplace_back(new_image);
	}

	// " MAX "文字生成
	for (size_t i = 0; i < ga_max_text.size(); i++)
	{
		// 画像用オブジェクトの生成
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// テクスチャ追加
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_max_text[i]);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 1.5f) - 20.0f;
		new_image->SetTransform(&transform);

		// 画像オブジェクトの追加
		this->mvp_text.emplace_back(new_image);
	}

	// "STAGE"文字生成
	for (size_t i = 0; i < ga_stage_text.size(); i++)
	{
		// 画像用オブジェクトの生成
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// テクスチャ追加
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_stage_text[i]);

		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 5.0f) + (BUTTON_WIDTH * 4.0f) + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - 10.0f;
		new_image->SetTransform(&transform);

		// 画像オブジェクトの追加
		this->mvp_text.emplace_back(new_image);
	}

	// " MAX "文字生成
	for (size_t i = 0; i < ga_max_text.size(); i++)
	{
		// 画像用オブジェクトの生成
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// テクスチャ追加
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_max_text[i]);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 5.0f) + (BUTTON_WIDTH * 4.0f) + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 1.5f) - 20.0f;
		new_image->SetTransform(&transform);

		// 画像オブジェクトの追加
		this->mvp_text.emplace_back(new_image);
	}

	// ステージ背景画像の追加
	for (int i = 0; i < AREA_NUM; i++)
	{
		// 画像用オブジェクトの生成
		CPage* new_image = new CPage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->SetSprite(SPRITE_TEST_2, SPRITE_TEST_1);
		new_image->SetDefaultUV(i);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
		new_image->SetTransform(&transform);

		// 画像オブジェクトの追加
		this->mvp_pages.emplace_back(new_image);
	}

	// ボタンをあらかじめ用意
	this->mvp_main_buttons.resize(SEB_MAX);
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		// ボタンオブジェクト生成
		this->mvp_main_buttons[i] = new CButton;
		this->mvp_main_buttons[i]->Initilize();

		// ボタンのサイズ
		float width = BUTTON_W_WIDTH;
		float height = BUTTON_W_HEIGHT;

		// ボタンの大きさを変える
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(width, height);

		// 画像オブジェクトの追加
		this->mvp_main_buttons[i]->SetButton(VK_LBUTTON, XI_BUTTON_B);
	}

	// ボタン専用のテクスチャ設定関数(保存、やめる)
	this->mvp_main_buttons[SEB_SAVE]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 4.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_BACK]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 3.0f, 0.0f }, true);

	// ボタン専用のテクスチャ設定関数(ワールド関係・左右)
	this->mvp_main_buttons[SEB_WORLD_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_WORLD_MAX_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_WORLD_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_WORLD_MAX_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);

	// ボタン専用のテクスチャ設定関数(ステージ関係・左右)
	this->mvp_main_buttons[SEB_STAGE_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_STAGE_MAX_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_STAGE_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_STAGE_MAX_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);

	// ボタン専用のテクスチャ設定関数(モード変更関係)
	this->mvp_main_buttons[SEB_ADD_SPRITE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 3.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_SPRITE_OPTION]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 0.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_MAPCHIP]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 0.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_GOAL]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 3.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_BACK_PAGE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 0.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_BACK_OBJECT]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 1.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_PLAYER]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 4.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_GIMMICK]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 2.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_PIECE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 2.0f, 0.0f }, true);

	// ボタン専用のテクスチャ設定関数(エディット補助関係)
	this->mvp_main_buttons[SEB_FRONT_BACK]->SetSprite(SPRITE_TEST_BUTTON_0, { 9.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_ROTATE_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_1, { 0.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_ROTATE_LEFT]->SetSprite(SPRITE_TEST_BUTTON_1, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_PAINT]->SetSprite(SPRITE_TEST_BUTTON_0, { 6.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_ERASE]->SetSprite(SPRITE_TEST_BUTTON_0, { 7.0f, 4.0f }, true);

	// SEB_PAINT と SEB_ERASE はラジオボタンに
	this->mvp_main_buttons[SEB_PAINT]->SetOtherButtonObject(this->mvp_main_buttons[SEB_ERASE]);
	this->mvp_main_buttons[SEB_ERASE]->SetOtherButtonObject(this->mvp_main_buttons[SEB_PAINT]);

	// 初期はPAINTモード
	this->mvp_main_buttons[SEB_PAINT]->SetONOFF(true);

	// ボタンの位置合わせ
	for (int i = SEB_SAVE; i <= SEB_BACK; i++)
	{
		// 座標変更
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 0.5f) + 10.0f; //((10.0f) * static_cast<float>(i + 1)) + (BUTTON_W_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// ボタンの位置合わせ
	for (int i = SEB_WORLD_LEFT; i <= SEB_WORLD_MAX_LEFT; i++)
	{
		// 大きさを変える
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// 座標変更
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 4.0f) + (BUTTON_WIDTH);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_WORLD_LEFT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_WORLD_LEFT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// ボタンの位置合わせ
	for (int i = SEB_WORLD_RIGHT; i <= SEB_WORLD_MAX_RIGHT; i++)
	{
		// 大きさを変える
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// 座標変更
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 5.0f) + (BUTTON_WIDTH * 3.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_WORLD_RIGHT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_WORLD_RIGHT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// ボタンの位置合わせ
	for (int i = SEB_STAGE_LEFT; i <= SEB_STAGE_MAX_LEFT; i++)
	{
		// 大きさを変える
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// 座標変更
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 9.0f) + (BUTTON_WIDTH * 5.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_STAGE_LEFT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_STAGE_LEFT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// ボタンの位置合わせ
	for (int i = SEB_STAGE_RIGHT; i <= SEB_STAGE_MAX_RIGHT; i++)
	{
		// 大きさを変える
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// 座標変更
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 10.0f) + (BUTTON_WIDTH * 7.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_STAGE_RIGHT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_STAGE_RIGHT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// ボタンの位置合わせ
	for (int i = SEB_ADD_SPRITE; i <= SEB_PIECE; i++)
	{
		float width = static_cast<float>((i - SEB_ADD_SPRITE) % 6);
		float height = static_cast<float>((i - SEB_ADD_SPRITE) / 6);

		// 座標変更
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * (2.5f + width)) + (10.0f * (width + 1.0f)) + (CHAR_WIDTH * 10.0f) + (BUTTON_WIDTH * 7.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * height);
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// ボタンの位置合わせ
	for (int i = SEB_FRONT_BACK; i <= SEB_ERASE; i++)
	{
		float width = static_cast<float>((i - SEB_FRONT_BACK) % 10);
		float height = static_cast<float>((i - SEB_FRONT_BACK) / 10);

		// 大きさを変える
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// 座標変更
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (BUTTON_WIDTH * (width + 1.0f));
		transform.position.y = (-screen_height * 0.5f) + (10.0f * (height + 1.0f)) + (BUTTON_HEIGHT * (height + 1.0f));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// 文字の作成
	this->mvp_value_text.resize(VT_MAX);
	for (size_t i = 0; i < mvp_value_text.size(); i++)
	{
		// 画像用オブジェクトの生成
		mvp_value_text[i] = new CEditorText;
		mvp_value_text[i]->Initilize();
	}

	// 文字の位置合わせ
	for (int i = VT_WORLD_10; i <= VT_WORLD_MAX_01; i++)
	{
		float width = static_cast<float>((i - VT_WORLD_10) % 2);
		float height = static_cast<float>((i - VT_WORLD_10) / 2);

		// 座標変更
		Transform transform = this->mvp_value_text[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 4.0f) + (BUTTON_WIDTH * 2.0f) + (CHAR_WIDTH * width);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * height);
		this->mvp_value_text[i]->SetTransform(&transform);
	}

	// 文字の位置合わせ
	for (int i = VT_STAGE_10; i <= VT_STAGE_MAX_01; i++)
	{
		float width = static_cast<float>((i - VT_STAGE_10) % 2);
		float height = static_cast<float>((i - VT_STAGE_10) / 2);

		// 座標変更
		Transform transform = this->mvp_value_text[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 9.0f) + (BUTTON_WIDTH * 6.0f) + (CHAR_WIDTH * width);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * height);
		this->mvp_value_text[i]->SetTransform(&transform);
	}

	// ボタンをあらかじめ用意
	this->mvp_select_buttons.resize(SSB_MAX);
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// ボタンオブジェクト生成
		this->mvp_select_buttons[i] = new CButton;
		this->mvp_select_buttons[i]->Initilize();

		// ボタンの大きさを変える
		this->mvp_select_buttons[i]->SetDefaultPosAndColl(BUTTON_W_WIDTH, BUTTON_W_HEIGHT);

		float width = static_cast<float>(i % 10);
		float height = static_cast<float>(i / 10);

		// 座標変更
		Transform transform = this->mvp_select_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (BUTTON_W_WIDTH * (width + 1.0f));
		transform.position.y = (screen_height * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * (height + 1.0f));
		this->mvp_select_buttons[i]->SetTransform(&transform);

		// ボタンの設定
		this->mvp_select_buttons[i]->SetButton(VK_LBUTTON, XI_BUTTON_B);
	}
	this->mvp_select_buttons[SSB_BACK]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 3.0f, 0.0f }, true);

	this->mvp_gimmik_image.resize(GSI_MAX);
	for (size_t i = 0; i < this->mvp_gimmik_image.size(); i++)
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

		float width = static_cast<float>(i % 30);
		float height = static_cast<float>(i / 30);

		// 座標変更
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (MAPCHIP_WIDTH * (width + 1.0f));
		transform.position.y = (screen_height * 0.5f) - (10.0f * (height + 2.0f)) - (BUTTON_W_HEIGHT)-(MAPCHIP_HEIGHT * (height + 1.0f));
		new_image->SetTransform(&transform);

		// 画像オブジェクトの追加
		this->mvp_gimmik_image[i] = new_image;
	}

	// 画像設定
	this->mvp_gimmik_image[GSI_GOAL]->AddTexture(SPRITE_GOAL);
	this->mvp_gimmik_image[GSI_PIECE_1]->AddTexture(SPRITE_PIECE_0);
	this->mvp_gimmik_image[GSI_PIECE_2]->AddTexture(SPRITE_PIECE_0);
	this->mvp_gimmik_image[GSI_PIECE_3]->AddTexture(SPRITE_PIECE_0);
	this->mvp_gimmik_image[GSI_RED_BLOCK]->AddTexture(SPRITE_RED_BULE);
	this->mvp_gimmik_image[GSI_BLUE_BLOCK]->AddTexture(SPRITE_RED_BULE);
	this->mvp_gimmik_image[GSI_TORNADO]->AddTexture(SPRITE_TORNADO);
	this->mvp_gimmik_image[GSI_DHARMA]->AddTexture(SPRITE_DHARMA);
	this->mvp_gimmik_image[GSI_CANNON]->AddTexture(SPRITE_CANNON);
	this->mvp_gimmik_image[GSI_LION_TOP]->AddTexture(SPRITE_LION_TOP);
	this->mvp_gimmik_image[GSI_LION_BOTTOM]->AddTexture(SPRITE_LION_BOTTOM);

	// UV設定
	{
		UV new_uv = { 1.0f, 0.0f };
		this->mvp_gimmik_image[GSI_PIECE_2]->SetAnim(&new_uv);
	}
	{
		UV new_uv = { 2.0f, 0.0f };
		this->mvp_gimmik_image[GSI_PIECE_3]->SetAnim(&new_uv);
	}
	{
		UV new_uv = { 1.0f, 1.0f };
		this->mvp_gimmik_image[GSI_BLUE_BLOCK]->SetAnim(&new_uv);
	}

	// プレイヤーの作成
	{
		// 画像用オブジェクトの生成
		CPlayer* new_image = new CPlayer;
		new_image->Initilize();

		// 画像オブジェクトの追加
		this->mvp_player.emplace_back(new_image);
	}

	// カーソル先の画像用オブジェクト作成
	{
		// 画像用オブジェクトの生成
		CCursor* new_image = new CCursor;
		new_image->Initilize();

		Color color = { 1.0f, 1.0f, 1.0f, 0.5f };
		new_image->SetColor(&color);

		// 画像オブジェクトの追加
		this->mvp_cursor_image.emplace_back(new_image);
	}

	// ステージ読み込み
	this->LoadStage();

	// カーソル
	this->mp_cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();
}

void CStageEditor::Finalize()
{
	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->UnInitilize();
		delete this->mvp_images[i];
	}
	this->mvp_images.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_mapchip.size(); i++)
	{
		this->mvp_mapchip[i]->UnInitilize();
		delete this->mvp_mapchip[i];
	}
	this->mvp_mapchip.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
	{
		this->mvp_mapchip_image[i]->UnInitilize();
		delete this->mvp_mapchip_image[i];
	}
	this->mvp_mapchip_image.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_text.size(); i++)
	{
		this->mvp_text[i]->UnInitilize();
		delete this->mvp_text[i];
	}
	this->mvp_text.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_value_text.size(); i++)
	{
		this->mvp_value_text[i]->UnInitilize();
		delete this->mvp_value_text[i];
	}
	this->mvp_value_text.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_pages.size(); i++)
	{
		this->mvp_pages[i]->UnInitilize();
		delete this->mvp_pages[i];
	}
	this->mvp_pages.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_player.size(); i++)
	{
		this->mvp_player[i]->UnInitilize();
		delete this->mvp_player[i];
	}
	this->mvp_player.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->UnInitilize();
		delete this->mvp_main_buttons[i];
	}
	this->mvp_main_buttons.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->UnInitilize();
		delete this->mvp_select_buttons[i];
	}
	this->mvp_select_buttons.clear();

	// オブジェクト解放
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->UnInitilize();
		delete this->mvp_cursor_image[i];
	}
	this->mvp_cursor_image.clear();

	// ギミックをまとめて解放
	std::vector<CImage*>* vp_ap_gimmik[6];
	vp_ap_gimmik[0] = &this->mvp_goal;
	vp_ap_gimmik[1] = &this->mvp_piece;
	vp_ap_gimmik[2] = &this->mvp_tornado;
	vp_ap_gimmik[3] = &this->mvp_dharma;
	vp_ap_gimmik[4] = &this->mvp_cannon;
	vp_ap_gimmik[5] = &this->mvp_lion;
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < vp_ap_gimmik[i]->size(); j++)
		{
			(*vp_ap_gimmik[i])[j]->UnInitilize();
			delete (*vp_ap_gimmik[i])[j];
		}
		vp_ap_gimmik[i]->clear();
	}

	// 画像リソースの開放
	for (size_t i = 0; i < gv_sprite_data.size(); i++)
	{
		COM_SAFE_RELEASE(gv_sprite_data[i].mSRV);
	}

	this->mp_cursor = nullptr;
}

void CStageEditor::Update()
{
	// 各種更新処理へ
	switch (g_edit_mode)
	{
	case EM_NONE:
		// カーソルの画像は初期状態
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		this->NormalUpdate();
		break;
	case EM_SPRITE_OPTION:
		this->SpriteOptionUpdate();
		break;
	case EM_MAPCHIP_SELECT:
		this->MapChipSelectUpdate();
		break;
	case EM_MAPCHIP:
		this->MapChipUpdate();
		break;
	case EM_GIMMIK_SELECT:
		this->GimmikSelectUpdate();
		break;
	case EM_GIMMIK:
		this->GimmikUpdate();
		break;
	case EM_BACK_SELECT:
		// カーソルの画像は初期状態
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		this->BackSelectUpdate();
		break;
	}
}

void CStageEditor::Draw()
{
	// 各種描画処理へ
	switch (g_view_mode)
	{
	case EV_NONE:
		this->NormalDraw();
		break;
	case EV_SPRITE_SELECT:
		this->SpriteOptionDraw();
		break;
	case EV_GIMMIK_SELECT:
		this->GimmikSelectDraw();
		break;
	}
}

void CStageEditor::PauseUpdate()
{
	// Pause中にボタンを戻す
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->SetONOFF(false);
	}
}

void CStageEditor::PauseDraw()
{
	this->Draw();
}

void CStageEditor::ColliderTest()
{
	// 各種当たり判定処理へ
	switch (g_edit_mode)
	{
	case EM_NONE:
		this->NormalColliderTest();
		break;
	case EM_SPRITE_OPTION:
		this->SpriteOptionColliderTest();
		break;
	case EM_MAPCHIP_SELECT:
		this->MapChipSelectColliderTest();
		break;
	case EM_MAPCHIP:
		this->MapChipColliderTest();
		break;
	case EM_GIMMIK_SELECT:
		this->GimmikSelectColliderTest();
		break;
	case EM_GIMMIK:
		this->GimmikColliderTest();
		break;
	case EM_BACK_SELECT:
		this->BackSelectColliderTest();
		break;
	}
}

void CStageEditor::LoadStage()
{
	// とりあえず設定を最低値にする
	gi_world_index = 0;
	gi_stage_index = 0;
	gi_world_max = 1;
	gi_stage_max = 1;
	gb_is_front = true;

	// スプライトファクトリーをじかに使う
	CSpriteFactory* sprite_factory = CSingleton<CSpriteFactory>::GetInstance();

	// デフォルトスプライトを生成
	sprite_factory->CreateSprite(&g_default_chip, DEFAULT_CHIP_SPRITE);
	sprite_factory->CreateSprite(&g_default_front, DEFAULT_FRONT_SPRITE);
	sprite_factory->CreateSprite(&g_default_back, DEFAULT_BACK_SPRITE);

	// スプライトの初期化
	gv_io_sprite_data.clear();
	gv_sprite_data.clear();

	// ステージ初期化
	for (size_t w = 0; w < ga_stage_data.size(); w++)
	{
		for (size_t s = 0; s < ga_stage_data[w].size(); s++)
		{
			// スプライト無し
			ga_stage_data[w][s].mi_front_back = MAPCHIP_SPRITE_NONE;
			ga_stage_data[w][s].mi_back_back = MAPCHIP_SPRITE_NONE;

			// プレイヤーは左下
			ga_stage_data[w][s].m_player_pos_x = 0;
			ga_stage_data[w][s].m_player_pos_y = 1;

			// ゴールデータ初期化
			ga_stage_data[w][s].mv_front_goal_obj.clear();
			ga_stage_data[w][s].mv_back_goal_obj.clear();

			// カケラデータ初期化
			ga_stage_data[w][s].mv_front_piece_obj.clear();
			ga_stage_data[w][s].mv_back_piece_obj.clear();

			// 竜巻データ初期化
			ga_stage_data[w][s].mv_front_tornade_obj.clear();
			ga_stage_data[w][s].mv_back_tornade_obj.clear();

			// ダルマデータ初期化
			ga_stage_data[w][s].mv_front_dharma_obj.clear();
			ga_stage_data[w][s].mv_back_dharma_obj.clear();

			// 大砲データ初期化
			ga_stage_data[w][s].mv_front_cannon_obj.clear();
			ga_stage_data[w][s].mv_back_cannon_obj.clear();

			// リフトデータ初期化
			ga_stage_data[w][s].mv_front_lion_obj.clear();
			ga_stage_data[w][s].mv_back_lion_obj.clear();

			// マップチップデータ初期化
			for (size_t x = 0; x < ga_stage_data[w][s].mv_front_data.size(); x++)
			{
				for (size_t y = 0; y < ga_stage_data[w][s].mv_front_data[x].size(); y++)
				{
					ga_stage_data[w][s].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
					ga_stage_data[w][s].mv_front_data[x][y].mf_angle_z = 0.0f;
					ga_stage_data[w][s].mv_front_data[x][y].m_tag = TAG_MAPCHIP;
				}
			}
			// マップチップデータ初期化
			for (size_t x = 0; x < ga_stage_data[w][s].mv_back_data.size(); x++)
			{
				for (size_t y = 0; y < ga_stage_data[w][s].mv_back_data[x].size(); y++)
				{
					ga_stage_data[w][s].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
					ga_stage_data[w][s].mv_back_data[x][y].mf_angle_z = 0.0f;
					ga_stage_data[w][s].mv_back_data[x][y].m_tag = TAG_MAPCHIP;
				}
			}
		}
	}

	{
		std::ifstream ifs("assets/Game/Stage/StageData.dat");

		if (!ifs.fail()) {
			size_t size;

			// 画像・ステージ数読み込み
			ifs >> size;
			ifs >> gi_world_max;
			ifs >> gi_stage_max;

			gv_io_sprite_data.resize(size);

			// 画像データ読み込み
			for (size_t i = 0; i < gv_io_sprite_data.size(); i++)
			{
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.filename;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.anim_max;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.slice_x;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.slice_y;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.count_check;
				ifs >> gv_io_sprite_data[i].m_width;
				ifs >> gv_io_sprite_data[i].m_height;
			}

			// 画像生成
			for (size_t i = 0; i < gv_io_sprite_data.size(); i++)
			{
				SpriteData new_sprite;

				// CDirect3D取得
				CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

				// 画像読み込み
				HRESULT hr = direct3d->LoadTexture(gv_io_sprite_data[i].m_io_sprite_data.filename.c_str(), &new_sprite.mSRV);

				if (FAILED(hr)) {
					// 一応 nullptr に
					new_sprite.mSRV = nullptr;
					gv_sprite_data.emplace_back(new_sprite);
				}
				else {
					// 画像の設定をし、登録
					new_sprite.anim_max = gv_io_sprite_data[i].m_io_sprite_data.anim_max;
					new_sprite.slice_x = gv_io_sprite_data[i].m_io_sprite_data.slice_x;
					new_sprite.slice_y = gv_io_sprite_data[i].m_io_sprite_data.slice_y;

					new_sprite.slice.u = 1.0f / static_cast<float>(new_sprite.slice_x);
					new_sprite.slice.v = 1.0f / static_cast<float>(new_sprite.slice_y);

					new_sprite.count_check = gv_io_sprite_data[i].m_io_sprite_data.count_check;

					gv_sprite_data.emplace_back(new_sprite);
				}
			}
		}
	}

	// 画像オブジェクト生成
	this->CreateImage();

	{
		for (size_t w = 0; w < static_cast<size_t>(gi_world_max); w++)
		{
			for (size_t s = 0; s < static_cast<size_t>(gi_stage_max); s++)
			{
				// ディレクトリ名
				std::string dir_name = "assets/Game/Stage/Stage_";

				// 拡張子名
				std::string ex_name = ".dat";

				// ファイル名
				std::string file_name = dir_name + std::to_string(w) + "_" + std::to_string(s) + ex_name;

				std::ifstream ifs(file_name);

				if (!ifs.fail()) {


					// マップチップデータ読み込み
					for (size_t x = 0; x < ga_stage_data[w][s].mv_front_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_front_data[x].size(); y++)
						{
							int tag;
							ifs >> ga_stage_data[w][s].mv_front_data[x][y].mi_chip_id;
							ifs >> ga_stage_data[w][s].mv_front_data[x][y].mf_angle_z;
							ifs >> tag;
							ga_stage_data[w][s].mv_front_data[x][y].m_tag = static_cast<ObjectTag>(tag);
						}
					}
					// マップチップデータ読み込み
					for (size_t x = 0; x < ga_stage_data[w][s].mv_back_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_back_data[x].size(); y++)
						{
							int tag;
							ifs >> ga_stage_data[w][s].mv_back_data[x][y].mi_chip_id;
							ifs >> ga_stage_data[w][s].mv_back_data[x][y].mf_angle_z;
							ifs >> tag;
							ga_stage_data[w][s].mv_back_data[x][y].m_tag = static_cast<ObjectTag>(tag);
						}
					}

					// 仕様変更でいらなくなったデータを格納する
					int trash;

					// 背景画像番号読み込み
					ifs >> ga_stage_data[w][s].mi_front_back;
					ifs >> ga_stage_data[w][s].mi_back_back;

					// プレイヤー位置読み込み
					ifs >> ga_stage_data[w][s].m_player_pos_x;
					ifs >> ga_stage_data[w][s].m_player_pos_y;

					// 残ったゴミ
					ifs >> trash;
					ifs >> trash;

					// ゴールデータ読み込み
					int size;
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						GoalData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_front_goal_obj.emplace_back(data);
					}

					// ゴールデータ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						GoalData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_back_goal_obj.emplace_back(data);
					}

					// カケラデータ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						PieceData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.i_piece_num;
						ga_stage_data[w][s].mv_front_piece_obj.emplace_back(data);
					}

					// カケラデータ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						PieceData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.i_piece_num;
						ga_stage_data[w][s].mv_back_piece_obj.emplace_back(data);
					}

					// 竜巻データ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						TornadeData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_front_tornade_obj.emplace_back(data);
					}

					// 竜巻データ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						TornadeData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_back_tornade_obj.emplace_back(data);
					}

					// ダルマデータ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						DharmaData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_max_speed;
						ifs >> data.mb_is_left;
						ga_stage_data[w][s].mv_front_dharma_obj.emplace_back(data);
					}

					// ダルマデータ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						DharmaData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_max_speed;
						ifs >> data.mb_is_left;
						ga_stage_data[w][s].mv_back_dharma_obj.emplace_back(data);
					}

					// 大砲データ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						CannonData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_angle_z;
						ifs >> data.mf_max_speed;
						ifs >> data.mf_wait;
						ga_stage_data[w][s].mv_front_cannon_obj.emplace_back(data);
					}

					// 大砲データ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						CannonData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_angle_z;
						ifs >> data.mf_max_speed;
						ifs >> data.mf_wait;
						ga_stage_data[w][s].mv_back_cannon_obj.emplace_back(data);
					}

					// リフトデータ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						LionData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_up_max_speed;
						ifs >> data.mf_down_max_speed;
						ifs >> data.mf_up_wait;
						ifs >> data.mf_down_wait;
						ga_stage_data[w][s].mv_front_lion_obj.emplace_back(data);
					}

					// リフトデータ読み込み
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						LionData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_up_max_speed;
						ifs >> data.mf_down_max_speed;
						ifs >> data.mf_up_wait;
						ifs >> data.mf_down_wait;
						ga_stage_data[w][s].mv_back_lion_obj.emplace_back(data);
					}
				}
			}
		}
	}
}

void CStageEditor::SaveStage()
{
	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::cout << "セーブします。何もしないでください。" << std::endl;

	{
		std::ofstream ofs("assets/Game/Stage/StageData.dat");

		// 画像・ステージ数書き出し
		ofs << gv_io_sprite_data.size() << " "
			<< gi_world_max << " "
			<< gi_stage_max << std::endl;

		// 画像設定書き出し
		for (size_t i = 0; i < gv_io_sprite_data.size(); i++)
		{
			ofs << gv_io_sprite_data[i].m_io_sprite_data.filename << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.anim_max << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.slice_x << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.slice_y << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.count_check << " "
				<< gv_io_sprite_data[i].m_width << " "
				<< gv_io_sprite_data[i].m_height << std::endl;
		}
	}

	{
		// 全ステージ書き出す
		for (size_t w = 0; w < static_cast<size_t>(gi_world_max); w++)
		{
			for (size_t s = 0; s < static_cast<size_t>(gi_stage_max); s++)
			{
				std::string dir_name = "assets/Game/Stage/Stage_";
				std::string ex_name = ".dat";

				std::string file_name = dir_name + std::to_string(w) + "_" + std::to_string(s) + ex_name;

				std::ofstream ofs(file_name);

				if (!ofs.fail()) {

					// マップチップデータ書き出し
					for (size_t x = 0; x < ga_stage_data[w][s].mv_front_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_front_data[x].size(); y++)
						{
							ofs << ga_stage_data[w][s].mv_front_data[x][y].mi_chip_id << " "
								<< ga_stage_data[w][s].mv_front_data[x][y].mf_angle_z << " "
								<< static_cast<int>(ga_stage_data[w][s].mv_front_data[x][y].m_tag) << std::endl;
						}
					}

					// マップチップデータ書き出し
					for (size_t x = 0; x < ga_stage_data[w][s].mv_back_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_back_data[x].size(); y++)
						{
							ofs << ga_stage_data[w][s].mv_back_data[x][y].mi_chip_id << " "
								<< ga_stage_data[w][s].mv_back_data[x][y].mf_angle_z << " "
								<< static_cast<int>(ga_stage_data[w][s].mv_back_data[x][y].m_tag) << std::endl;
						}
					}

					// 背景番号・プレイヤー位置・昔の名残を書き出し
					ofs << ga_stage_data[w][s].mi_front_back << " "
						<< ga_stage_data[w][s].mi_back_back << " "
						<< ga_stage_data[w][s].m_player_pos_x << " "
						<< ga_stage_data[w][s].m_player_pos_y << " "
						<< 0 << " "
						<< 0 << std::endl;

					// ゴールデータ書き出し
					ofs << ga_stage_data[w][s].mv_front_goal_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_goal_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_goal_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_goal_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_goal_obj[i].position.z << std::endl;
					}

					// ゴールデータ書き出し
					ofs << ga_stage_data[w][s].mv_back_goal_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_goal_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_goal_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_goal_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_goal_obj[i].position.z << std::endl;
					}

					// カケラデータ書き出し
					ofs << ga_stage_data[w][s].mv_front_piece_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_piece_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_piece_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_piece_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_piece_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_piece_obj[i].i_piece_num << std::endl;
					}

					// カケラデータ書き出し
					ofs << ga_stage_data[w][s].mv_back_piece_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_piece_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_piece_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_piece_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_piece_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_piece_obj[i].i_piece_num << std::endl;
					}

					// 竜巻データ書き出し
					ofs << ga_stage_data[w][s].mv_front_tornade_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_tornade_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_tornade_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_tornade_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_tornade_obj[i].position.z << std::endl;
					}

					// 竜巻データ書き出し
					ofs << ga_stage_data[w][s].mv_back_tornade_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_tornade_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_tornade_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_tornade_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_tornade_obj[i].position.z << std::endl;
					}

					// ダルマデータ書き出し
					ofs << ga_stage_data[w][s].mv_front_dharma_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_dharma_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_dharma_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].mb_is_left << std::endl;
					}

					// ダルマデータ書き出し
					ofs << ga_stage_data[w][s].mv_back_dharma_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_dharma_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_dharma_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].mb_is_left << std::endl;
					}

					// 大砲データ書き出し
					ofs << ga_stage_data[w][s].mv_front_cannon_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_cannon_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_cannon_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].mf_angle_z << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].mf_wait << std::endl;
					}

					// 大砲データ書き出し
					ofs << ga_stage_data[w][s].mv_back_cannon_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_cannon_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_cannon_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].mf_angle_z << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].mf_wait << std::endl;
					}

					// リフトデータ書き出し
					ofs << ga_stage_data[w][s].mv_front_lion_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_lion_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_lion_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_up_max_speed << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_down_max_speed << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_up_wait << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_down_wait << std::endl;
					}

					// リフトデータ書き出し
					ofs << ga_stage_data[w][s].mv_back_lion_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_lion_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_lion_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_up_max_speed << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_down_max_speed << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_up_wait << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_down_wait << std::endl;
					}


					ofs << END_OF_STAGE_FILE << std::endl;

				}
			}
		}
	}

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

}

void CStageEditor::UpdateStage()
{
	// スプライトファクトリーをじかに使う
	CSpriteFactory* sprite_factory = CSingleton<CSpriteFactory>::GetInstance();

	// 表向き
	if (gb_is_front) {
		// 背景番号
		int front_id = ga_stage_data[gi_world_index][gi_stage_index].mi_front_back;
		int back_id = ga_stage_data[gi_world_index][gi_stage_index].mi_back_back;

		// 画像を初期状態のものに
		SpriteData front_sprite = g_default_front;
		SpriteData back_sprite = g_default_back;

		// 画像が指定され存在するなら交換
		if (front_id > MAPCHIP_SPRITE_NONE && front_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[front_id].mSRV != nullptr) front_sprite = gv_sprite_data[front_id];
		}
		if (back_id > MAPCHIP_SPRITE_NONE && back_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[back_id].mSRV != nullptr) back_sprite = gv_sprite_data[back_id];
		}

		// 背景画像の追加
		for (size_t i = 0; i < this->mvp_pages.size(); i++)
		{
			// テクスチャ追加
			this->mvp_pages[i]->SetSprite(&front_sprite, &back_sprite);
		}

		// マップチップ初期化
		for (size_t x = 0; x < ga_stage_data[gi_world_index][gi_stage_index].mv_front_data.size(); x++)
		{
			for (size_t y = 0; y < ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x].size(); y++)
			{
				UV default_uv = { 0.0f, 0.0f };
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&default_uv);
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);

				if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag == TAG_RED_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag == TAG_BLUE_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
					UV new_uv = { 1.0f, 1.0f };
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&new_uv);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag == TAG_LION_BOTTOM)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_LION_BOTTOM);

					switch (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id)
					{
					case LEFT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f / 3.0f, 1.0f, 0.0f);
						break;
					case CENTER_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(1.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f);
						break;
					case RIGHT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(2.0f / 3.0f, 1.0f, 1.0f, 0.0f);
						break;
					default:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);
						break;
					}
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id <= MAPCHIP_SPRITE_NONE) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id >= static_cast<int>(gv_sprite_data.size()))
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id].mSRV == nullptr) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id]);
				}
				Transform tr = this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->GetTransform();
				tr.angle.z = ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z;
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTransform(&tr);
			}
		}

		// ゴール
		this->UpdateGoal(ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj);

		// カケラ
		this->UpdatePiece(ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj);

		// 竜巻
		this->UpdateTornade(ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj);

		// ダルマ
		this->UpdateDharma(ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj);

		// 大砲
		this->UpdateCannon(ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj);

		// リフト
		this->UpdateLion(ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj);

		for (size_t i = 0; i < this->mvp_player.size(); i++)
		{
			Transform tr = this->mvp_player[i]->GetTransform();
			Collider collider = this->mvp_player[i]->GetCollider();
			tr.position.x = (MAPCHIP_WIDTH * (-(static_cast<float>(MAPCHIP_NUM_WIDTH) * static_cast<float>(AREA_NUM)) / 2.0f + 0.5f) + (MAPCHIP_WIDTH * static_cast<float>(ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_x))) - (collider.center_x * 2.0f);
			tr.position.y = (MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f - 1.0f) + (PLAYER_COLLIDER_HEIGHT / 2.0f) + (PLAYER_COLLIDER_HEIGHT / 2.0f) + (MAPCHIP_HEIGHT * static_cast<float>(ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_y))) - (collider.center_y * 2.0f);
			this->mvp_player[i]->SetTransform(&tr);
		}
	}
	else {
		int front_id = ga_stage_data[gi_world_index][gi_stage_index].mi_back_back;
		int back_id = ga_stage_data[gi_world_index][gi_stage_index].mi_front_back;

		SpriteData front_sprite = g_default_back;
		SpriteData back_sprite = g_default_front;

		if (front_id > MAPCHIP_SPRITE_NONE && front_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[front_id].mSRV != nullptr) front_sprite = gv_sprite_data[front_id];
		}
		if (back_id > MAPCHIP_SPRITE_NONE && back_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[back_id].mSRV != nullptr) back_sprite = gv_sprite_data[back_id];
		}

		// 背景画像の追加
		for (size_t i = 0; i < this->mvp_pages.size(); i++)
		{
			// テクスチャ追加
			this->mvp_pages[i]->SetSprite(&front_sprite, &back_sprite);
		}

		// マップチップ初期化
		for (size_t x = 0; x < ga_stage_data[gi_world_index][gi_stage_index].mv_back_data.size(); x++)
		{
			for (size_t y = 0; y < ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x].size(); y++)
			{
				UV default_uv = { 0.0f, 0.0f };
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&default_uv);
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);

				if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag == TAG_RED_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag == TAG_BLUE_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
					UV new_uv = { 1.0f, 1.0f };
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&new_uv);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag == TAG_LION_BOTTOM)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_LION_BOTTOM);

					switch (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id)
					{
					case LEFT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f / 3.0f, 1.0f, 0.0f);
						break;
					case CENTER_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(1.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f);
						break;
					case RIGHT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(2.0f / 3.0f, 1.0f, 1.0f, 0.0f);
						break;
					default:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);
						break;
					}
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id <= MAPCHIP_SPRITE_NONE) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id >= static_cast<int>(gv_sprite_data.size()))
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id].mSRV == nullptr) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id]);
				}

				Transform tr = this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->GetTransform();
				tr.angle.z = ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z;
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTransform(&tr);
			}
		}

		// ゴール
		this->UpdateGoal(ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj);

		// カケラ
		this->UpdatePiece(ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj);

		// 竜巻
		this->UpdateTornade(ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj);

		// ダルマ
		this->UpdateDharma(ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj);

		// 大砲
		this->UpdateCannon(ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj);

		// リフト
		this->UpdateLion(ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj);

	}
}

void CStageEditor::CreateImage()
{
	for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
	{
		this->mvp_mapchip_image[i]->UnInitilize();
		delete this->mvp_mapchip_image[i];
	}
	this->mvp_mapchip_image.clear();

	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	for (size_t i = 0; i < gv_sprite_data.size(); i++)
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_2);
		if (gv_sprite_data[i].mSRV != nullptr) new_image->SetTexture(&gv_sprite_data[i]);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

		float width = static_cast<float>(i % 30);
		float height = static_cast<float>(i / 30);

		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (MAPCHIP_WIDTH * (width + 1.0f));
		transform.position.y = (screen_height * 0.5f) - (10.0f * (height + 2.0f)) - (BUTTON_W_HEIGHT)-(MAPCHIP_HEIGHT * (height + 1.0f));
		new_image->SetTransform(&transform);

		// 画像オブジェクトの追加
		this->mvp_mapchip_image.emplace_back(new_image);
	}
}

void CStageEditor::AddSprite()
{
	IO_SpriteDataEX new_data;

	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::string dir_name = "assets/Game/Sprites/";
	std::string file_name;

	std::cout << "追加する画像のパスを入力してください : \n" << dir_name;
	std::cin >> file_name;
	std::cout << std::endl;

	new_data.m_io_sprite_data.filename = dir_name + file_name;

	std::cout << "画像の横幅を入力してください : \n";
	std::cin >> new_data.m_width;
	std::cout << std::endl;

	std::cout << "画像の縦幅を入力してください : \n";
	std::cin >> new_data.m_height;
	std::cout << std::endl;

	std::cout << "画像の横の分割数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.slice_x;
	std::cout << std::endl;

	std::cout << "画像の縦の分割数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.slice_y;
	std::cout << std::endl;

	std::cout << "使用する枚数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.anim_max;
	std::cout << std::endl;

	std::cout << "切り替えの待ち秒数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.count_check;
	std::cout << std::endl;

	if (new_data.m_width < 1.0f) {
		new_data.m_width = 1.0f;
	}
	if (new_data.m_height < 1.0f) {
		new_data.m_height = 1.0f;
	}

	if (new_data.m_io_sprite_data.slice_x < 1) {
		new_data.m_io_sprite_data.slice_x = 1;
	}
	if (new_data.m_io_sprite_data.slice_y < 1) {
		new_data.m_io_sprite_data.slice_y = 1;
	}
	if (new_data.m_io_sprite_data.anim_max < 1) {
		new_data.m_io_sprite_data.anim_max = 1;
	}
	if (new_data.m_io_sprite_data.count_check < 0) {
		new_data.m_io_sprite_data.count_check = 1;
	}

	{
		SpriteData new_sprite;

		// CDirect3D取得
		CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

		HRESULT hr = direct3d->LoadTexture(new_data.m_io_sprite_data.filename.c_str(), &new_sprite.mSRV);

		if (FAILED(hr)) {
			std::cout << "ファイルが見つからないです" << std::endl;
		}
		else {
			new_sprite.anim_max = new_data.m_io_sprite_data.anim_max;
			new_sprite.slice_x = new_data.m_io_sprite_data.slice_x;
			new_sprite.slice_y = new_data.m_io_sprite_data.slice_y;

			new_sprite.slice.u = 1.0f / static_cast<float>(new_sprite.slice_x);
			new_sprite.slice.v = 1.0f / static_cast<float>(new_sprite.slice_y);

			new_sprite.count_check = new_data.m_io_sprite_data.count_check;

			gv_io_sprite_data.emplace_back(new_data);
			gv_sprite_data.emplace_back(new_sprite);
		}
	}

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

	this->CreateImage();
}

void CStageEditor::ChangeSprite(int id)
{
	if (id < 0) return;
	if (static_cast<size_t>(id) >= gv_io_sprite_data.size()) return;

	IO_SpriteDataEX new_data;

	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::string dir_name = "assets/Game/Sprites/";
	std::string file_name;

	std::cout << gv_io_sprite_data[id].m_io_sprite_data.filename << "を設定しなおします。\n" << std::endl;

	int check = 0;
	std::cout << "画像を変えるなら 1 を入力してください : \n";
	std::cin >> check;
	std::cout << std::endl;

	new_data.m_io_sprite_data.filename = gv_io_sprite_data[id].m_io_sprite_data.filename;

	if (check == 1) {
		std::string dir_name = "assets/Game/Sprites/";
		std::string file_name;
		std::string final_file_name;

		std::cout << "変更する画像のパスを入力してください : \n" << dir_name;
		std::cin >> file_name;
		std::cout << std::endl;

		final_file_name = dir_name + file_name;

		SpriteData new_sprite;

		// CDirect3D取得
		CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

		HRESULT hr = direct3d->LoadTexture(final_file_name.c_str(), &new_sprite.mSRV);

		if (FAILED(hr)) {
			std::cout << "ファイルが見つからないです" << std::endl;
		}
		else {
			new_sprite.anim_max = new_data.m_io_sprite_data.anim_max;
			new_sprite.slice_x = new_data.m_io_sprite_data.slice_x;
			new_sprite.slice_y = new_data.m_io_sprite_data.slice_y;

			new_sprite.slice.u = 1.0f / static_cast<float>(new_sprite.slice_x);
			new_sprite.slice.v = 1.0f / static_cast<float>(new_sprite.slice_y);

			new_sprite.count_check = new_data.m_io_sprite_data.count_check;

			COM_SAFE_RELEASE(gv_sprite_data[id].mSRV);

			gv_sprite_data[id] = new_sprite;

			new_data.m_io_sprite_data.filename = final_file_name;
		}
	}

	std::cout << "画像の横幅を入力してください : \n";
	std::cin >> new_data.m_width;
	std::cout << std::endl;

	std::cout << "画像の縦幅を入力してください : \n";
	std::cin >> new_data.m_height;
	std::cout << std::endl;

	std::cout << "画像の横の分割数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.slice_x;
	std::cout << std::endl;

	std::cout << "画像の縦の分割数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.slice_y;
	std::cout << std::endl;

	std::cout << "使用する枚数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.anim_max;
	std::cout << std::endl;

	std::cout << "切り替えの待ち秒数を入力してください : \n";
	std::cin >> new_data.m_io_sprite_data.count_check;
	std::cout << std::endl;

	if (new_data.m_width < 1.0f) {
		new_data.m_width = 1.0f;
	}
	if (new_data.m_height < 1.0f) {
		new_data.m_height = 1.0f;
	}

	if (new_data.m_io_sprite_data.slice_x < 1) {
		new_data.m_io_sprite_data.slice_x = 1;
	}
	if (new_data.m_io_sprite_data.slice_y < 1) {
		new_data.m_io_sprite_data.slice_y = 1;
	}
	if (new_data.m_io_sprite_data.anim_max < 1) {
		new_data.m_io_sprite_data.anim_max = 1;
	}
	if (new_data.m_io_sprite_data.count_check < 0) {
		new_data.m_io_sprite_data.count_check = 1;
	}

	{
		gv_sprite_data[id].anim_max = new_data.m_io_sprite_data.anim_max;
		gv_sprite_data[id].slice_x = new_data.m_io_sprite_data.slice_x;
		gv_sprite_data[id].slice_y = new_data.m_io_sprite_data.slice_y;

		gv_sprite_data[id].slice.u = 1.0f / static_cast<float>(gv_sprite_data[id].slice_x);
		gv_sprite_data[id].slice.v = 1.0f / static_cast<float>(gv_sprite_data[id].slice_y);

		gv_sprite_data[id].count_check = new_data.m_io_sprite_data.count_check;
	}

	gv_io_sprite_data[id] = new_data;

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

	this->CreateImage();
}

void CStageEditor::MovePlayer()
{
	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::cout << "プレイヤーの位置を設定しなおします。\n" << std::endl;

	int new_x;
	std::cout << "左端を 0 として、横の位置を入力してください : \n";
	std::cin >> new_x;
	std::cout << std::endl;

	int new_y;
	std::cout << "下端を 0 として、縦の位置を入力してください : \n";
	std::cin >> new_y;
	std::cout << std::endl;

	if (0 <= new_x && new_x < AREA_NUM * MAPCHIP_NUM_WIDTH) {
		ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_x = new_x;
	}

	if (0 <= new_y && new_y < MAPCHIP_NUM_HEIGHT) {
		ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_y = new_y;
	}

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::UpdateGoal(std::vector<GoalData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_goal.size()) {
			Transform transform = this->mvp_goal[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_goal[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_GOAL);

			new_image->SetDefaultPosAndColl(GOAL_WIDTH, GOAL_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_goal.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_goal.size(); )
	{
		this->mvp_goal[i]->UnInitilize();
		delete this->mvp_goal[i];
		this->mvp_goal.erase(this->mvp_goal.begin() + i);
	}
}

void CStageEditor::UpdatePiece(std::vector<PieceData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_piece.size()) {
			float new_u = static_cast<float>(data[i].i_piece_num % 3);
			if (new_u < 0.0f) new_u *= -1.0f;

			UV new_uv = { new_u, 0.0f };
			this->mvp_piece[i]->SetAnim(&new_uv);

			Transform transform = this->mvp_piece[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_piece[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_PIECE_0);

			new_image->SetDefaultPosAndColl(PIECE_0_WIDTH, PIECE_0_HEIGHT);

			float new_u = static_cast<float>(data[i].i_piece_num % 3);
			if (new_u < 0.0f) new_u *= -1.0f;

			UV new_uv = { new_u, 0.0f };
			new_image->SetAnim(&new_uv);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_piece.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_piece.size(); )
	{
		this->mvp_piece[i]->UnInitilize();
		delete this->mvp_piece[i];
		this->mvp_piece.erase(this->mvp_piece.begin() + i);
	}
}

void CStageEditor::UpdateTornade(std::vector<TornadeData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_tornado.size()) {
			Transform transform = this->mvp_tornado[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_tornado[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_TORNADO);

			new_image->SetDefaultPosAndColl(TORNADE_WIDTH, TORNADE_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_tornado.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_tornado.size(); )
	{
		this->mvp_tornado[i]->UnInitilize();
		delete this->mvp_tornado[i];
		this->mvp_tornado.erase(this->mvp_tornado.begin() + i);
	}
}

void CStageEditor::UpdateDharma(std::vector<DharmaData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_dharma.size()) {
			Transform transform = this->mvp_dharma[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_dharma[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_DHARMA);

			new_image->SetDefaultPosAndColl(DHARMA_WIDTH, DHARMA_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_dharma.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_dharma.size(); )
	{
		this->mvp_dharma[i]->UnInitilize();
		delete this->mvp_dharma[i];
		this->mvp_dharma.erase(this->mvp_dharma.begin() + i);
	}
}

void CStageEditor::UpdateCannon(std::vector<CannonData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_cannon.size()) {
			Transform transform = this->mvp_cannon[i]->GetTransform();
			transform.position = data[i].position;
			transform.angle.z = data[i].mf_angle_z;
			this->mvp_cannon[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_CANNON);

			new_image->SetDefaultPosAndColl(CANNON_WIDTH, CANNON_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			transform.angle.z = data[i].mf_angle_z;
			new_image->SetTransform(&transform);

			this->mvp_cannon.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_cannon.size(); )
	{
		this->mvp_cannon[i]->UnInitilize();
		delete this->mvp_cannon[i];
		this->mvp_cannon.erase(this->mvp_cannon.begin() + i);
	}
}

void CStageEditor::UpdateLion(std::vector<LionData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_lion.size()) {
			Transform transform = this->mvp_lion[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_lion[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_LION_TOP);

			new_image->SetDefaultPosAndColl(LION_WIDTH, LION_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_lion.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_lion.size(); )
	{
		this->mvp_lion[i]->UnInitilize();
		delete this->mvp_lion[i];
		this->mvp_lion.erase(this->mvp_lion.begin() + i);
	}
}

void CStageEditor::InputDharmaData(DharmaData * data)
{
	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	float mf_max_speed;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	std::cout << "ダルマの最大速度を入力してください。(0.0未満で初期値、0.0で元の値)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_dharma_max_speed;
	if (mf_max_speed > 0.0f) data->mf_max_speed = mf_max_speed;

	int dir;

	std::cout << "ダルマが最初に動く方向を選んでください。(0 で左、1 で右)\n : ";
	std::cin >> dir;
	if (dir == 0) data->mb_is_left = true;
	if (dir == 1) data->mb_is_left = false;

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::InputCannonData(CannonData * data)
{
	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	float mf_max_speed;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	std::cout << "弾の最大速度を入力してください。(0.0未満で初期値、0.0で元の値)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_cannon_max_speed;
	if (mf_max_speed > 0.0f) data->mf_max_speed = mf_max_speed;

	float wait;

	std::cout << "玉の発射感覚を入力してください。(0.0未満で初期値、0.0で元の値)\n<float 秒> : ";
	std::cin >> wait;
	if (wait < 0.0f) wait = cip_data.M_DATA.mf_cannon_wait;
	if (wait > 0.0f) data->mf_wait = wait;

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::InputLionTopData(LionData * data)
{
	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	float mf_max_speed;
	float wait;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	std::cout << "リフトの下る最大速度を入力してください。(0.0未満で初期値、0.0で元の値)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_lion_down_max_speed;
	if (mf_max_speed > 0.0f) data->mf_down_max_speed = mf_max_speed;

	std::cout << "リフトの下るまでの待ち時間を入力してください。(0.0未満で初期値、0.0で元の値)\n<float 秒> : ";
	std::cin >> wait;
	if (wait < 0.0f) wait = cip_data.M_DATA.mf_lion_down_wait;
	if (wait > 0.0f) data->mf_down_wait = wait;

	std::cout << "リフトの上がる最大速度を入力してください。(0.0未満で初期値、0.0で元の値)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_lion_up_max_speed;
	if (mf_max_speed > 0.0f) data->mf_up_max_speed = mf_max_speed;

	std::cout << "リフトの上がるまでの待ち時間を入力してください。(0.0未満で初期値、0.0で元の値)\n<float 秒> : ";
	std::cin >> wait;
	if (wait < 0.0f) wait = cip_data.M_DATA.mf_lion_up_wait;
	if (wait > 0.0f) data->mf_up_wait = wait;

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::NormalUpdate()
{
	// ボタンの更新
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->Update();
	}

	// ページ更新
	for (size_t i = 0; i < this->mvp_pages.size(); i++)
	{
		this->mvp_pages[i]->Update();
	}

	// カーソル更新
	for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
	{
		this->mvp_cursor_image[j]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// セーブボタン
	if (this->mvp_main_buttons[SEB_SAVE]->GetONOFF()) {
		this->mvp_main_buttons[SEB_SAVE]->SetONOFF(false);
		this->SaveStage();
	}

	// 終了ボタンが押された
	if (this->mvp_main_buttons[SEB_BACK]->GetONOFF()) {
		scene_manager->ChangeScene(SCENE_EDITOR_SELECT);
	}

	// スプライト追加ボタン
	if (this->mvp_main_buttons[SEB_ADD_SPRITE]->GetONOFF()) {
		this->mvp_main_buttons[SEB_ADD_SPRITE]->SetONOFF(false);
		this->AddSprite();
	}

	// スプライト設定ボタン
	if (this->mvp_main_buttons[SEB_SPRITE_OPTION]->GetONOFF()) {
		this->mvp_main_buttons[SEB_SPRITE_OPTION]->SetONOFF(false);
		g_edit_mode = EM_SPRITE_OPTION;
		g_view_mode = EV_SPRITE_SELECT;
	}

	// プレイヤー移動ボタン
	if (this->mvp_main_buttons[SEB_PLAYER]->GetONOFF()) {
		this->mvp_main_buttons[SEB_PLAYER]->SetONOFF(false);
		this->MovePlayer();
	}

	// マップチップ選択ボタン
	if (this->mvp_main_buttons[SEB_MAPCHIP]->GetONOFF()) {
		this->mvp_main_buttons[SEB_MAPCHIP]->SetONOFF(false);
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		g_chip_brush.mi_chip_id = MAPCHIP_SPRITE_NONE;
		g_chip_brush.mf_angle_z = 0.0f;
		g_chip_brush.m_tag = TAG_MAPCHIP;
		g_edit_mode = EM_MAPCHIP_SELECT;
		g_view_mode = EV_SPRITE_SELECT;
	}

	// ギミック選択ボタン
	if (this->mvp_main_buttons[SEB_GIMMICK]->GetONOFF()) {
		this->mvp_main_buttons[SEB_GIMMICK]->SetONOFF(false);
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		g_chip_brush.mi_chip_id = MAPCHIP_SPRITE_NONE;
		g_chip_brush.mf_angle_z = 0.0f;
		g_chip_brush.m_tag = TAG_MAPCHIP;
		g_edit_mode = EM_GIMMIK_SELECT;
		g_view_mode = EV_GIMMIK_SELECT;
	}

	// 背景選択ボタン
	if (this->mvp_main_buttons[SEB_BACK_PAGE]->GetONOFF()) {
		this->mvp_main_buttons[SEB_BACK_PAGE]->SetONOFF(false);
		g_edit_mode = EM_BACK_SELECT;
		g_view_mode = EV_SPRITE_SELECT;
	}

	// ワールド選択ボタンが押された
	if (this->mvp_main_buttons[SEB_WORLD_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_LEFT]->SetONOFF(false);
		if (gi_world_index > 0) gi_world_index--;
	}
	if (this->mvp_main_buttons[SEB_WORLD_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_RIGHT]->SetONOFF(false);
		if (gi_world_index < gi_world_max - 1) gi_world_index++;
	}
	if (this->mvp_main_buttons[SEB_WORLD_MAX_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_MAX_LEFT]->SetONOFF(false);
		if (gi_world_max > 1) gi_world_max--;
		if (gi_world_max - 1 < gi_world_index) gi_world_index = gi_world_max;
	}
	if (this->mvp_main_buttons[SEB_WORLD_MAX_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_MAX_RIGHT]->SetONOFF(false);
		if (gi_world_max < WORLD_MAX) gi_world_max++;
	}

	// 裏表ボタン
	if (this->mvp_main_buttons[SEB_FRONT_BACK]->GetONOFF()) {
		if (gb_is_front) gb_is_front = false;
	}
	else {
		if (!gb_is_front) gb_is_front = true;
	}

	// 左回転ボタン
	if (this->mvp_main_buttons[SEB_ROTATE_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_ROTATE_LEFT]->SetONOFF(false);
		g_chip_brush.mf_angle_z += 90.0f;
		if (g_chip_brush.mf_angle_z >= 360.0f) g_chip_brush.mf_angle_z -= 360.0f;
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = g_chip_brush.mf_angle_z;
			this->mvp_cursor_image[i]->SetTransform(&transform);
		}
	}

	// 右回転ボタン
	if (this->mvp_main_buttons[SEB_ROTATE_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_ROTATE_RIGHT]->SetONOFF(false);
		g_chip_brush.mf_angle_z -= 90.0f;
		if (g_chip_brush.mf_angle_z < 0.0f) g_chip_brush.mf_angle_z += 360.0f;
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = g_chip_brush.mf_angle_z;
			this->mvp_cursor_image[i]->SetTransform(&transform);
		}
	}

	// ステージ選択ボタンが押された
	if (this->mvp_main_buttons[SEB_STAGE_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_LEFT]->SetONOFF(false);
		if (gi_stage_index > 0) gi_stage_index--;
	}
	if (this->mvp_main_buttons[SEB_STAGE_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_RIGHT]->SetONOFF(false);
		if (gi_stage_index < gi_stage_max - 1) gi_stage_index++;
	}
	if (this->mvp_main_buttons[SEB_STAGE_MAX_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_MAX_LEFT]->SetONOFF(false);
		if (gi_stage_max > 1) gi_stage_max--;
		if (gi_stage_max - 1 < gi_stage_index) gi_stage_index = gi_stage_max;
	}
	if (this->mvp_main_buttons[SEB_STAGE_MAX_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_MAX_RIGHT]->SetONOFF(false);
		if (gi_stage_max < WORLD_MAX) gi_stage_max++;
	}

	{
		int world_index = gi_world_index + 1;
		UV value_10 = { static_cast<float>((world_index % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(world_index % 10), 0.0f };
		this->mvp_value_text[VT_WORLD_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_WORLD_01]->SetAnim(&value_01);
	}

	{
		UV value_10 = { static_cast<float>((gi_world_max % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(gi_world_max % 10), 0.0f };
		this->mvp_value_text[VT_WORLD_MAX_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_WORLD_MAX_01]->SetAnim(&value_01);
	}

	{
		int stage_index = gi_stage_index + 1;
		UV value_10 = { static_cast<float>((stage_index % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(stage_index % 10), 0.0f };
		this->mvp_value_text[VT_STAGE_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_STAGE_01]->SetAnim(&value_01);
	}

	{
		UV value_10 = { static_cast<float>((gi_stage_max % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(gi_stage_max % 10), 0.0f };
		this->mvp_value_text[VT_STAGE_MAX_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_STAGE_MAX_01]->SetAnim(&value_01);
	}

	this->UpdateStage();

	this->ColliderTest();
}

void CStageEditor::NormalColliderTest()
{
	// 当たられるオブジェクト
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_main_buttons[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mvp_main_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::NormalDraw()
{
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_pages.size(); i++)
	{
		this->mvp_pages[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_mapchip.size(); i++)
	{
		this->mvp_mapchip[i]->Draw();
	}

	std::vector<CImage*>* vp_ap_gimmik[6];
	vp_ap_gimmik[0] = &this->mvp_goal;
	vp_ap_gimmik[1] = &this->mvp_piece;
	vp_ap_gimmik[2] = &this->mvp_tornado;
	vp_ap_gimmik[3] = &this->mvp_dharma;
	vp_ap_gimmik[4] = &this->mvp_cannon;
	vp_ap_gimmik[5] = &this->mvp_lion;
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < vp_ap_gimmik[i]->size(); j++)
		{
			(*vp_ap_gimmik[i])[j]->Draw();
		}
	}

	if (gb_is_front) {
		for (size_t i = 0; i < this->mvp_player.size(); i++)
		{
			this->mvp_player[i]->Draw();
		}
	}
	for (size_t i = 0; i < this->mvp_text.size(); i++)
	{
		this->mvp_text[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_value_text.size(); i++)
	{
		this->mvp_value_text[i]->Draw();
	}
	this->mp_cursor->Draw();
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->Draw();
	}
}

void CStageEditor::SpriteOptionUpdate()
{
	// ボタンの更新
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// 戻るボタン
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		g_edit_mode = EM_NONE;
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::SpriteOptionColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// 当たられるオブジェクト
		for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip_image[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->ChangeSprite(i);
			}
		}
	}

	// 当たられるオブジェクト
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::SpriteOptionDraw()
{
	for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
	{
		this->mvp_mapchip_image[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Draw();
	}
	this->mp_cursor->Draw();
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->Draw();
	}
}

void CStageEditor::MapChipSelectUpdate()
{
	// ボタンの更新
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	// カーソル更新
	for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
	{
		this->mvp_cursor_image[j]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// 戻るボタン
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		if (g_chip_brush.mi_chip_id == MAPCHIP_SPRITE_NONE) {
			g_edit_mode = EM_NONE;
		}
		else {
			g_edit_mode = EM_MAPCHIP;
		}
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::MapChipSelectColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// 当たられるオブジェクト
		for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip_image[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				g_chip_brush.mi_chip_id = static_cast<int>(i);
				g_chip_brush.mf_angle_z = 0.0f;
				g_chip_brush.m_tag = TAG_MAPCHIP;

				for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
				{
					this->mvp_cursor_image[j]->SetTexture(&gv_sprite_data[g_chip_brush.mi_chip_id]);
					this->mvp_cursor_image[j]->SetDefaultPosition(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);
				}

			}
		}
	}

	// 当たられるオブジェクト
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::MapChipUpdate()
{
	this->NormalUpdate();
}

void CStageEditor::MapChipColliderTest()
{
	if (this->mp_input->GetKeyPress(VK_LBUTTON)) {
		// 当たられるオブジェクト
		for (size_t i = 0; i < this->mvp_mapchip.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				int x = static_cast<int>(i) % (MAPCHIP_NUM_WIDTH * AREA_NUM);
				int y = static_cast<int>(i) / (MAPCHIP_NUM_WIDTH * AREA_NUM);

				if (this->mvp_main_buttons[SEB_PAINT]->GetONOFF())
				{
					if (gb_is_front) {
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = g_chip_brush.mi_chip_id;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;
					}
					else {
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = g_chip_brush.mi_chip_id;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;
					}
				}
				else if (this->mvp_main_buttons[SEB_ERASE]->GetONOFF())
				{
					if (gb_is_front) {
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = 0.0f;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = TAG_MAPCHIP;
					}
					else {
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = 0.0f;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = TAG_MAPCHIP;
					}
				}
			}
		}
	}
	this->NormalColliderTest();
}

void CStageEditor::BackSelectUpdate()
{
	// ボタンの更新
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// 戻るボタン
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		g_edit_mode = EM_NONE;
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::BackSelectColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// 当たられるオブジェクト
		for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip_image[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				if (gb_is_front) ga_stage_data[gi_world_index][gi_stage_index].mi_front_back = static_cast<int>(i);
				else ga_stage_data[gi_world_index][gi_stage_index].mi_back_back = static_cast<int>(i);
			}
		}
	}

	// 当たられるオブジェクト
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::GimmikSelectUpdate()
{
	// ボタンの更新
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	// カーソル更新
	for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
	{
		this->mvp_cursor_image[j]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// 戻るボタン
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		if (g_chip_brush.m_tag == TAG_MAPCHIP) {
			g_edit_mode = EM_NONE;
		}
		else {
			g_edit_mode = EM_GIMMIK;
		}
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::GimmikSelectColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// 当たられるオブジェクト
		for (size_t i = 0; i < this->mvp_gimmik_image.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_gimmik_image[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				g_chip_brush.mi_chip_id = MAPCHIP_NONE;
				g_chip_brush.mf_angle_z = 0.0f;

				switch (i)
				{
				case GSI_GOAL:
					g_chip_brush.m_tag = TAG_GOAL;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_GOAL);
						this->mvp_cursor_image[j]->SetDefaultPosition(GOAL_WIDTH, GOAL_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_PIECE_1:
					g_chip_brush.m_tag = TAG_PIECE_1;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_PIECE_0);
						this->mvp_cursor_image[j]->SetDefaultPosition(PIECE_0_WIDTH, PIECE_0_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_PIECE_2:
					g_chip_brush.m_tag = TAG_PIECE_2;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_PIECE_0);
						this->mvp_cursor_image[j]->SetDefaultPosition(PIECE_0_WIDTH, PIECE_0_HEIGHT);

						UV new_uv = { 1.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_PIECE_3:
					g_chip_brush.m_tag = TAG_PIECE_3;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_PIECE_0);
						this->mvp_cursor_image[j]->SetDefaultPosition(PIECE_0_WIDTH, PIECE_0_HEIGHT);

						UV new_uv = { 2.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_RED_BLOCK:
					g_chip_brush.m_tag = TAG_RED_BLOCK;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_RED_BULE);
						this->mvp_cursor_image[j]->SetDefaultPosition(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_BLUE_BLOCK:
					g_chip_brush.m_tag = TAG_BLUE_BLOCK;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_RED_BULE);
						this->mvp_cursor_image[j]->SetDefaultPosition(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

						UV new_uv = { 1.0f, 1.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_TORNADO:
					g_chip_brush.m_tag = TAG_TORNADO;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_TORNADO);
						this->mvp_cursor_image[j]->SetDefaultPosition(TORNADE_WIDTH, TORNADE_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_DHARMA:
					g_chip_brush.m_tag = TAG_DHARMA;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_DHARMA);
						this->mvp_cursor_image[j]->SetDefaultPosition(DHARMA_WIDTH, DHARMA_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_CANNON:
					g_chip_brush.m_tag = TAG_CANNON;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_CANNON);
						this->mvp_cursor_image[j]->SetDefaultPosition(CANNON_WIDTH, CANNON_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_LION_TOP:
					g_chip_brush.m_tag = TAG_LION_TOP;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_LION_TOP);
						this->mvp_cursor_image[j]->SetDefaultPosition(LION_WIDTH, LION_HEIGHT);

						//UV new_uv = { 0.0f, 0.0f };
						//this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_LION_BOTTOM:
					g_chip_brush.m_tag = TAG_LION_BOTTOM;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_LION_BOTTOM);
						this->mvp_cursor_image[j]->SetDefaultPosition(LION_WIDTH, LION_HEIGHT);

						//UV new_uv = { 0.0f, 1.0f };
						//this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				default:
					g_chip_brush.m_tag = TAG_MAPCHIP;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->SetTexture(&g_default_chip);
						this->mvp_cursor_image[j]->SetDefaultPosition(4.0f, 4.0f);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				}
			}
		}
	}

	// 当たられるオブジェクト
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::GimmikSelectDraw()
{
	for (size_t i = 0; i < this->mvp_gimmik_image.size(); i++)
	{
		this->mvp_gimmik_image[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Draw();
	}
	this->mp_cursor->Draw();
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->Draw();
	}
}

void CStageEditor::GimmikUpdate()
{
	this->NormalUpdate();
}

void CStageEditor::GimmikColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// ペイントモードとイレイズモードで変える
		if (this->mvp_main_buttons[SEB_PAINT]->GetONOFF())
		{
			// 当たられるオブジェクト
			for (int i = 0; i < this->mvp_mapchip.size(); i++)
			{
				// それぞれのコライダーを取得
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_mapchip[i]->GetConvertedCollider();

				// 当たり判定結果
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true のとき当たってます。
				if (hit.is_hit) {
					int x = i % (MAPCHIP_NUM_WIDTH * AREA_NUM);
					int y = i / (MAPCHIP_NUM_WIDTH * AREA_NUM);

					switch (g_chip_brush.m_tag)
					{
					case TAG_GOAL:
					{
						GoalData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj.emplace_back(data);
						}
					}
					break;
					case TAG_PIECE_1:
					{
						PieceData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.i_piece_num = 0;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.emplace_back(data);
						}
					}
					break;
					case TAG_PIECE_2:
					{
						PieceData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.i_piece_num = 1;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.emplace_back(data);
						}
					}
					break;
					case TAG_PIECE_3:
					{
						PieceData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.i_piece_num = 2;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.emplace_back(data);
						}
					}
					break;
					case TAG_RED_BLOCK:
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						break;
					case TAG_BLUE_BLOCK:
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						break;
					case TAG_TORNADO:
					{
						TornadeData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj.emplace_back(data);
						}
					}
					break;
					case TAG_DHARMA:
					{
						DharmaData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.mf_max_speed = DEFAULT_INPUT_VALUE;
						data.mb_is_left = true;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj.emplace_back(data);
						}
					}
					break;
					case TAG_CANNON:
					{
						CannonData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.mf_angle_z = g_chip_brush.mf_angle_z;
						data.mf_max_speed = DEFAULT_INPUT_VALUE;
						data.mf_wait = DEFAULT_INPUT_VALUE;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj.emplace_back(data);
						}
					}
					break;
					case TAG_LION_TOP:
					{
						if (x % MAPCHIP_NUM_WIDTH == 0 || x % MAPCHIP_NUM_WIDTH == MAPCHIP_NUM_WIDTH - 1) break;

						LionData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.mf_up_max_speed = DEFAULT_INPUT_VALUE;
						data.mf_down_max_speed = DEFAULT_INPUT_VALUE;
						data.mf_up_wait = DEFAULT_INPUT_VALUE;
						data.mf_down_wait = DEFAULT_INPUT_VALUE;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj.emplace_back(data);
						}
					}
					break;
					case TAG_LION_BOTTOM:
						if (x % MAPCHIP_NUM_WIDTH == 0 || x % MAPCHIP_NUM_WIDTH == MAPCHIP_NUM_WIDTH - 1) break;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x - 1][y].mi_chip_id = LEFT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x - 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x - 1][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = CENTER_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x + 1][y].mi_chip_id = RIGHT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x + 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x + 1][y].m_tag = g_chip_brush.m_tag;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x - 1][y].mi_chip_id = LEFT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x - 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x - 1][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = CENTER_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x + 1][y].mi_chip_id = RIGHT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x + 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x + 1][y].m_tag = g_chip_brush.m_tag;
						}
						break;
					default:
						break;
					}
				}

			}
		}
		else if (this->mvp_main_buttons[SEB_ERASE]->GetONOFF())
		{
			switch (g_chip_brush.m_tag)
			{
			case TAG_GOAL:
				// 当たられるオブジェクト
				for (int i = 0; i < this->mvp_goal.size(); i++)
				{
					// それぞれのコライダーを取得
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_goal[i]->GetConvertedCollider();

					// 当たり判定結果
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true のとき当たってます。
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_PIECE_1:
			case TAG_PIECE_2:
			case TAG_PIECE_3:
				// 当たられるオブジェクト
				for (int i = 0; i < this->mvp_piece.size(); i++)
				{
					// それぞれのコライダーを取得
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_piece[i]->GetConvertedCollider();

					// 当たり判定結果
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true のとき当たってます。
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_TORNADO:
				// 当たられるオブジェクト
				for (int i = 0; i < this->mvp_tornado.size(); i++)
				{
					// それぞれのコライダーを取得
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_tornado[i]->GetConvertedCollider();

					// 当たり判定結果
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true のとき当たってます。
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_DHARMA:
				// 当たられるオブジェクト
				for (int i = 0; i < this->mvp_dharma.size(); i++)
				{
					// それぞれのコライダーを取得
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_dharma[i]->GetConvertedCollider();

					// 当たり判定結果
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true のとき当たってます。
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_CANNON:
				// 当たられるオブジェクト
				for (int i = 0; i < this->mvp_cannon.size(); i++)
				{
					// それぞれのコライダーを取得
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_cannon[i]->GetConvertedCollider();

					// 当たり判定結果
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true のとき当たってます。
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_LION_TOP:
				// 当たられるオブジェクト
				for (int i = 0; i < this->mvp_lion.size(); i++)
				{
					// それぞれのコライダーを取得
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_lion[i]->GetConvertedCollider();

					// 当たり判定結果
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true のとき当たってます。
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_RED_BLOCK:
			case TAG_BLUE_BLOCK:
			case TAG_LION_BOTTOM:
				// 当たられるオブジェクト
				for (int i = 0; i < this->mvp_mapchip.size(); i++)
				{
					// それぞれのコライダーを取得
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_mapchip[i]->GetConvertedCollider();

					// 当たり判定結果
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true のとき当たってます。
					if (hit.is_hit) {
						int x = i % (MAPCHIP_NUM_WIDTH * AREA_NUM);
						int y = i / (MAPCHIP_NUM_WIDTH * AREA_NUM);

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = TAG_MAPCHIP;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = TAG_MAPCHIP;
						}
					}
				}
				break;
			default:
				break;
			}
		}
	}
	if (this->mp_input->GetKeyTrigger(VK_RBUTTON)) {
		switch (g_chip_brush.m_tag)
		{
		case TAG_DHARMA:
			// 当たられるオブジェクト
			for (int i = 0; i < this->mvp_dharma.size(); i++)
			{
				// それぞれのコライダーを取得
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_dharma[i]->GetConvertedCollider();

				// 当たり判定結果
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true のとき当たってます。
				if (hit.is_hit) {
					if (gb_is_front) {
						this->InputDharmaData(&ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj[i]);
					}
					else {
						this->InputDharmaData(&ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj[i]);
					}
					break;
				}
			}
			break;
		case TAG_CANNON:
			// 当たられるオブジェクト
			for (int i = 0; i < this->mvp_cannon.size(); i++)
			{
				// それぞれのコライダーを取得
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_cannon[i]->GetConvertedCollider();

				// 当たり判定結果
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true のとき当たってます。
				if (hit.is_hit) {
					if (gb_is_front) {
						this->InputCannonData(&ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj[i]);
					}
					else {
						this->InputCannonData(&ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj[i]);
					}
					break;
				}
			}
			break;
		case TAG_LION_TOP:
			// 当たられるオブジェクト
			for (int i = 0; i < this->mvp_lion.size(); i++)
			{
				// それぞれのコライダーを取得
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_lion[i]->GetConvertedCollider();

				// 当たり判定結果
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true のとき当たってます。
				if (hit.is_hit) {
					if (gb_is_front) {
						this->InputLionTopData(&ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj[i]);
					}
					else {
						this->InputLionTopData(&ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj[i]);
					}
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	this->NormalColliderTest();
}

