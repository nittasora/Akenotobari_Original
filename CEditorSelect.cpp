#include "CEditorSelect.h"

#include "CDirect3D.h"
#include"CInput.h"

#include "CImage.h"
#include "CButton.h"
#include "CCursor.h"

#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"

#include "CSingleton.h"

// それぞれのボタンの役割
enum EDITOR_SELECT_BUTTON
{
	ESB_STAGE,			// ステージエディタ―へ
	ESB_EDITOR_SAMPLE,	// サンプルへ
	ESB_FINISH,			// 終了
	ESB_MAX
};

CEditorSelect::CEditorSelect()
{
}

CEditorSelect::~CEditorSelect()
{
}

void CEditorSelect::Initialize()
{
	// 一応
	this->mvp_images.clear();
	this->mvp_buttons.clear();

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

		// 画像オブジェクトの登録
		this->mvp_images.emplace_back(new_image);
	}

	// ボタンをあらかじめ用意
	this->mvp_buttons.resize(ESB_MAX);
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		// ボタンオブジェクト生成
		this->mvp_buttons[i] = new CButton;
		this->mvp_buttons[i]->Initilize();

		// ボタンの大きさを変える
		float width = BUTTON_W_WIDTH;
		float height = BUTTON_W_HEIGHT;
		this->mvp_buttons[i]->SetDefaultPosAndColl(width, height);

		// ボタンの座標を変更
		Transform transform = this->mvp_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (width * 1.5f) * static_cast<float>(i % 8 + 1);
		transform.position.y = (screen_height * 0.5f) - (height * 1.5f) * static_cast<float>(i / 8 + 1);
		this->mvp_buttons[i]->SetTransform(&transform);

		// ボタンの登録
		this->mvp_buttons[i]->SetButton(VK_LBUTTON, XI_BUTTON_B);
	}

	// ボタン専用のテクスチャ設定関数
	this->mvp_buttons[ESB_STAGE]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 1.0f, 0.0f }, true);
	this->mvp_buttons[ESB_EDITOR_SAMPLE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 4.0f, 2.0f }, true);
	this->mvp_buttons[ESB_FINISH]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 2.0f, 0.0f }, true);

	// カーソル取得
	this->mp_cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();
}

void CEditorSelect::Finalize()
{
	// 画像オブジェクトの開放
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->UnInitilize();
		delete this->mvp_images[i];
	}
	this->mvp_images.clear();

	// ボタンの解放
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->UnInitilize();
		delete this->mvp_buttons[i];
	}
	this->mvp_buttons.clear();

	// カーソルを手放す
	this->mp_cursor = nullptr;
}

void CEditorSelect::Update()
{
	// ボタンの更新
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->Update();
	}

	// インスタンス取得
	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// ステージ作成ボタンが押された
	if (this->mvp_buttons[ESB_STAGE]->GetONOFF()) {
		// ステージエディタ―へ
		scene_manager->ChangeScene(SCENE_STAGE_EDITOR);
	}

	// エディターサンプルボタンが押された
	if (this->mvp_buttons[ESB_EDITOR_SAMPLE]->GetONOFF()) {
		// サンプルへ
		scene_manager->ChangeScene(SCENE_EDITOR_SAMPLE);
	}

	// 終了ボタンが押された
	if (this->mvp_buttons[ESB_FINISH]->GetONOFF()) {
		// ゲームループ終了
		scene_manager->FinishLoop();
	}

	this->ColliderTest();
}

void CEditorSelect::Draw()
{
	// 画像の描画
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->Draw();
	}

	// ボタンの描画
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->Draw();
	}
}

void CEditorSelect::PauseUpdate()
{
	// Pause中にボタンを戻す
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->SetONOFF(false);
	}
}

void CEditorSelect::PauseDraw()
{
	this->Draw();
}

void CEditorSelect::ColliderTest()
{
	// ボタンとカーソルの当たり判定
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_buttons[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mvp_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}
