#include "CEditorSample.h"

#include <fstream>
#include <iostream>
#include <string>

#include "CStageManager.h"
#include "CInput.h"
#include "CButton.h"
#include "CCursor.h"

#include "CDirect3D.h"
#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"

#include "CSingleton.h"

CEditorSample::CEditorSample()
{
	this->mp_stage_manager = nullptr;
}

CEditorSample::~CEditorSample()
{
}

void CEditorSample::Initialize()
{
	FILE* ifp;
	FILE* ofp;

	// コンソールを作成
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	// 読み込むワールド番号を入力させる
	int world = 0;
	std::cout << "ワールド番号を入力してください : \n";
	std::cin >> world;
	std::cout << std::endl;
	world--;

	// 読み込むステージ番号を入力させる
	int stage = 0;
	std::cout << "ステージ番号を入力してください : \n";
	std::cin >> stage;
	std::cout << std::endl;
	stage--;

	// ワールド番号とステージ番号を設定
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();
	game_manager->SetWorldIndex(world);
	game_manager->SetStageIndex(stage);

	// コンソール用ウィンドウの終了
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

	// ステージマネージャーインスタンス取得
	this->mp_stage_manager = new CStageManager;

	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	// もどるボタンの作成
	{
		CButton* new_image = new CButton;
		new_image->Initilize();

		// ボタンの大きさを変える
		float width = BUTTON_W_WIDTH;
		float height = BUTTON_W_HEIGHT;
		new_image->SetDefaultPosAndColl(width, height);

		// ボタンの座標を変える
		Transform transform = new_image->GetTransform();
		transform.position.y = (screen_height * 0.5f) - (height * 1.5f);
		new_image->SetTransform(&transform);

		// ボタンの画像と受け付ける入力の設定
		new_image->SetButton(VK_LBUTTON, XI_BUTTON_B);
		new_image->SetSprite(SPRITE_TEST_BUTTON_W_0, { 2.0f, 0.0f }, true);

		// ボタン登録
		this->mvp_button.emplace_back(new_image);
	}
}

void CEditorSample::Finalize()
{
	// ステージマネージャーの解放
	if (this->mp_stage_manager != nullptr) {
		delete this->mp_stage_manager;
		this->mp_stage_manager = nullptr;
	}

	// ボタンの開放
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		this->mvp_button[i]->UnInitilize();
		delete this->mvp_button[i];
	}
	this->mvp_button.clear();
}

void CEditorSample::Update()
{
	// ボタンの更新
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		this->mvp_button[i]->Update();
	}

	this->ColliderTest();

	// ボタンがONならばエディターの選択画面へ
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		if (this->mvp_button[i]->GetONOFF()) {
			this->mvp_button[i]->SetONOFF(false);
			CSingleton<CSceneManager>::GetInstance()->ChangeScene(SCENE_EDITOR_SELECT);
		}
	}

	// ステージの更新
	this->mp_stage_manager->Update();
}

void CEditorSample::Draw()
{
	// ステージの描画
	this->mp_stage_manager->Draw();

	// ボタンの描画
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		this->mvp_button[i]->Draw();
	}
}

void CEditorSample::PauseUpdate()
{
	// Pause中は何もしない
}

void CEditorSample::PauseDraw()
{
	// Pause中も描画はする
	this->Draw();
}

void CEditorSample::ColliderTest()
{
	// カーソルの取得
	CCursor* cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();

	// ボタンとカーソルの当たり判定
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		// それぞれのコライダーを取得
		Collider obj_collider = cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_button[i]->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->mvp_button[i]->OnCollision(&hit, cursor);
		}
	}
}
