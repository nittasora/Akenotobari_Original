#include "CTitle.h"

#include "CDirect3D.h"
#include"CInput.h"

#include "CImage.h"
#include "CButton.h"
#include "CCursor.h"
#include "CPlayerRunImage.h"

#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"
#include "CEditorManager.h"
#include "UserMath.h"
#include "CMainWindow.h"
#include "CInputParameter.h"
#include "CXAudio2.h"

#include "CSingleton.h"

enum TITLE_BUTTON
{
	TB_START,
	TB_OPTION,
	TB_FINISH,
	TB_MAX
};

CTitle::CTitle()
{
	this->mp_editor_manager = nullptr;
}

CTitle::~CTitle()
{
}

void CTitle::Initialize()
{
	this->mp_editor_manager = new CEditorManager;

	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	UINT32 random_value = UserMath::XOS_Rand();
	this->mi_world_num = static_cast<int>(random_value % GAME_WORLD_MAX);

	CMainWindow* cmain = CSingleton<CMainWindow>::GetInstance();

	{
		SpriteData new_data;
		std::string file_name = "assets/Game/Sprites/Common/Title/block.png";

		HRESULT hr = direct3d->LoadTexture(file_name.c_str(), &new_data.mSRV);
		std::string error_text = "画像読み込みに失敗しました。\nファイル名 : " + file_name;
		if (!cmain->ErrorCheck(hr, error_text.c_str())) {
			new_data.anim_max = 6;
			new_data.slice_x = 3;
			new_data.slice_y = 2;

			new_data.slice.u = 1.0f / 3.0f;
			new_data.slice.v = 1.0f / 2.0f;

			new_data.count_check = 1.0f;

			this->mv_sprite_data.emplace_back(new_data);
		};
	}

	{
		SpriteData new_data;
		std::string file_name;
		switch (this->mi_world_num)
		{
		case 2:
			file_name = "assets/Game/Sprites/Common/Title/back_02.png";
			break;
		case 1:
			file_name = "assets/Game/Sprites/Common/Title/back_01.png";
			break;
		case 0:
			file_name = "assets/Game/Sprites/Common/Title/back_00.png";
			break;
		default:
			file_name = "assets/Game/Sprites/Common/Title/back_00.png";
			this->mi_world_num = 0;
			break;
		}

		HRESULT hr = direct3d->LoadTexture(file_name.c_str(), &new_data.mSRV);
		std::string error_text = "画像読み込みに失敗しました。\nファイル名 : " + file_name;
		if (!cmain->ErrorCheck(hr, error_text.c_str())) {
			new_data.anim_max = 2;
			new_data.slice_x = 2;
			new_data.slice_y = 1;

			new_data.slice.u = 1.0f / 2.0f;
			new_data.slice.v = 1.0f;

			new_data.count_check = 1.0f;

			this->mv_sprite_data.emplace_back(new_data);
		};
	}

	float height_percent = screen_height / 480.0f;
	float back_width = 1344.0f * height_percent;
	float back_height = screen_height;

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_0);
		new_image->SetTexture(&this->mv_sprite_data[1]);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(back_width, back_height);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_back.emplace_back(new_image);
	}

	// 背景画像の追加
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_0);
		new_image->SetTexture(&this->mv_sprite_data[1]);

		UV new_uv = { 1.0f, 0.0f };
		new_image->SetAnim(&new_uv);

		Transform transform = new_image->GetTransform();
		transform.position.x = back_width;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(back_width, back_height);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_back.emplace_back(new_image);
	}

	float chip_width = 48.0f * height_percent;
	float chip_height = 48.0f * height_percent;

	// 背景画像の追加
	size_t chip_num = static_cast<size_t>(back_width / chip_width) * 2;
	for (size_t i = 0; i < chip_num; i++)
	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		// テクスチャ追加
		new_image->AddTexture(SPRITE_TEST_0);
		new_image->SetTexture(&this->mv_sprite_data[0]);

		UV new_uv = { static_cast<float>(this->mi_world_num), 0.0f };
		if (i >= chip_num / 2) new_uv.v = 1.0f;
		new_image->SetAnim(&new_uv);

		Transform transform = new_image->GetTransform();
		transform.position.x = (chip_width / 2.0f) - (back_width / 2.0f) + (chip_width * static_cast<float>(i));
		transform.position.y = (-screen_height / 2.0f) + (chip_height / 2.0f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(chip_width, chip_height);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_chip.emplace_back(new_image);
	}

	float player_width = PLAYER_RUN_WIDTH * height_percent;
	float player_height = PLAYER_RUN_HEIGHT * height_percent;

	{
		// 画像用オブジェクトの生成
		CPlayerRunImage* new_image = new CPlayerRunImage;
		new_image->Initilize();

		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width / 2.25f) + (chip_height * 2.0f);
		transform.position.y = (-screen_height / 2.0f) + (chip_height)+(player_height / 2.0f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(player_width, player_height);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mp_player = new_image;
	}

	this->mvp_button.resize(TB_MAX);
	this->mvp_cursor.resize(TB_MAX);
	this->mvp_frame.resize(TB_MAX);

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_TITLE_LOGO);

		Transform transform = new_image->GetTransform();
		transform.position.y = (chip_height * 2.0f) - 10.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(430.0f, 405.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_BUTTON_FRAME);

		Transform transform = new_image->GetTransform();
		transform.position.y = (-chip_height * 1.25f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(350.0f, 90.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_frame[TB_START] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_BUTTON_FRAME);

		Transform transform = new_image->GetTransform();
		transform.position.y = (-chip_height * 1.25f) - 92.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(350.0f, 90.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_frame[TB_OPTION] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_BUTTON_FRAME);

		Transform transform = new_image->GetTransform();
		transform.position.y = (-chip_height * 1.25f) - 166.0f;
		new_image->SetTransform(&transform);

		UV new_uv = { 1.0f, 0.0f };
		new_image->SetAnim(&new_uv);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(350.0f, 90.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_frame[TB_FINISH] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_BUTTON);

		Transform transform = new_image->GetTransform();
		transform.position.y = (-chip_height * 1.25f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(345.0f, 75.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_button[TB_START] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_BUTTON);

		Transform transform = new_image->GetTransform();
		transform.position.y = (-chip_height * 1.25f) - 92.0f;
		new_image->SetTransform(&transform);

		UV new_uv = { 1.0f, 2.0f };
		new_image->SetAnim(&new_uv);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(345.0f, 75.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_button[TB_OPTION] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_BUTTON);

		Transform transform = new_image->GetTransform();
		transform.position.y = (-chip_height * 1.25f) - 166.0f;
		new_image->SetTransform(&transform);

		UV new_uv = { 2.0f, 1.0f };
		new_image->SetAnim(&new_uv);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(345.0f, 75.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_button[TB_FINISH] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_CURSOR);

		Transform transform = new_image->GetTransform();
		transform.position.x = -208.0f;
		transform.position.y = (-chip_height * 1.25f);
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(50.0f, 53.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_cursor[TB_START] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_CURSOR);

		Transform transform = new_image->GetTransform();
		transform.position.x = -208.0f;
		transform.position.y = (-chip_height * 1.25f) - 92.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(50.0f, 53.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_cursor[TB_OPTION] = new_image;
	}

	{
		// 画像用オブジェクトの生成
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_CURSOR);

		Transform transform = new_image->GetTransform();
		transform.position.x = -145.0f;
		transform.position.y = (-chip_height * 1.25f) - 166.0f;
		new_image->SetTransform(&transform);

		// 画像の大きさを変える
		new_image->SetDefaultPosAndColl(50.0f, 53.0f);

		// 画像オブジェクトの追加
		this->mvp_objects.emplace_back(new_image);
		this->mvp_cursor[TB_FINISH] = new_image;
	}

	// カーソル
	this->mp_cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();

	// カーソルの位置
	this->mi_cursor_line = TB_START;
	this->mi_old_cursor_line = this->mi_cursor_line;
	this->mf_old_axis = 0.0f;

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

	this->ColliderTest();
}

void CTitle::Finalize()
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

	if (this->mp_editor_manager != nullptr) {
		delete this->mp_editor_manager;
		this->mp_editor_manager = nullptr;
	}

	this->mp_cursor = nullptr;
}

void CTitle::Update()
{
	if (!this->mb_initialized_flg) {
		this->mb_initialized_flg = true;
		this->mp_xaudio2->Start(SOUND_LABEL_BGM_TITLE);
		this->mp_xaudio2->XA_Setvolume(SOUND_LABEL_BGM_TITLE, 0.0f);
		this->mp_xaudio2->XA_SetFadein(SOUND_LABEL_BGM_TITLE, DEFAULT_MAX_VOLUME, DEFAULT_DIFF_VOLUME);
	}

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
		if (this->mi_cursor_line < TB_START) {
			this->mi_cursor_line = TB_FINISH;
		}
	}
	else if (this->mp_input->GetKeyTrigger('S') || this->mp_input->GetButtonTrigger(XI_BUTTON_DOWN) || check_axis < 0.0f) {
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->mi_cursor_line++;
		if (this->mi_cursor_line > TB_FINISH) {
			this->mi_cursor_line = TB_START;
		}
	}
	else if (this->mp_input->GetKeyTrigger(VK_LBUTTON) || this->mp_input->GetButtonTrigger(XI_BUTTON_B)) {
		this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
		if (this->mi_cursor_line == TB_START) {
			scene_manager->ChangeScene(SCENE_STAGE_SELECT, FADE_TYPE_02);
		}
		if (this->mi_cursor_line == TB_OPTION) {
			menu_manager->ChangeMenuSceneEX(MENU_PAUSE, false);
		}
		if (this->mi_cursor_line == TB_FINISH) {
			scene_manager->FinishLoop();
		}
	}
	this->mf_old_axis = axis;

	if (this->mp_input->GetKeyPress(VK_SHIFT) && this->mp_input->GetKeyPress(VK_DELETE)) {
		if (CSingleton<CMainWindow>::GetInstance()->TextBox("セーブデータを削除しますか?")) {
			CSingleton<CGameManager>::GetInstance()->DeleteSave();
		}
	}

	this->mp_editor_manager->Update();

	this->CommonUpdate();

	POINT point = this->mp_input->GetCursorClientDefference();
	if (point.x != 0 || point.y != 0) {
		this->ColliderTest();
	}

	if (this->mi_old_cursor_line != this->mi_cursor_line) {
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_MOVE);
	}

	Color new_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	this->mvp_cursor[this->mi_cursor_line]->SetColor(&new_color);
	this->mvp_frame[this->mi_cursor_line]->SetColor(&new_color);

}

void CTitle::Draw()
{
	for (size_t i = 0; i < this->mvp_objects.size(); i++)
	{
		this->mvp_objects[i]->Draw();
	}
}

void CTitle::PauseUpdate()
{
	this->CommonUpdate();
}

void CTitle::PauseDraw()
{
	this->Draw();
}

void CTitle::ColliderTest()
{
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_button[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mi_cursor_line = static_cast<TITLE_BUTTON>(i);
			break;
		}
	}
}

void CTitle::CommonUpdate()
{
	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	float speed = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data().M_DATA.mf_player_horizontal_max_speed;

	float height_percent = screen_height / 480.0f;
	float back_width = 1344.0f * height_percent;
	float back_height = screen_height;
	float chip_width = 480.0f * height_percent;
	float chip_height = 480.0f * height_percent;

	for (size_t i = 0; i < this->mvp_back.size(); i++)
	{
		Transform transform = this->mvp_back[i]->GetTransform();
		transform.position.x -= speed;

		if (transform.position.x <= -back_width) {
			transform.position.x += back_width * 2.0f;
		}

		this->mvp_back[i]->SetTransform(&transform);
	}

	for (size_t i = 0; i < this->mvp_chip.size(); i++)
	{
		Transform transform = this->mvp_chip[i]->GetTransform();
		transform.position.x -= speed;

		if (transform.position.x <= -back_width) {
			transform.position.x += back_width * 2.0f;
		}

		this->mvp_chip[i]->SetTransform(&transform);
	}

	this->mp_player->Update();
}
