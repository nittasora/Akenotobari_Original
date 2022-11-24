#include "CFadeManager.h"

#include "CDirect3D.h"

#include "CGameObject.h"
#include "CImage.h"
#include "CPlayerRunImage.h"

#include "CSingleton.h"

// フェードに使う
enum FADE_IMAGE
{
	FADE_IMAGE_NORMAL,	// 通常オブジェクト
	FADE_IMAGE_PLAYER,	// プレイヤー画像
	FADE_IMAGE_MAX
};

CFadeManager::CFadeManager()
{
	// フェードの状況を初期化
	this->m_data.m_condeition = FADE_OUT_END;
	this->m_data.m_next_condeition = FADE_OUT_END;
	this->m_data.m_type = FADE_TYPE_00;
	this->m_data.mi_fade_count = 0;

	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	// 前オブジェクト共通部分
	this->m_object.mvp_image.resize(FADE_IMAGE_MAX);
	for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
	{
		CImage* new_image;
		switch (i)
		{
			// プレイヤー画像
		case FADE_IMAGE_PLAYER:
			new_image = new CPlayerRunImage;
			break;
			// 通常画像
		default:
			new_image = new CImage;
			break;
		}
		new_image->Initilize();


		// オブジェクトの描画を止める
		obj_flg flg = OBJ_FLG_ACTIVE;
		new_image->SetFlg(&flg);

		// 登録
		this->m_object.mvp_image[i] = new_image;
		this->m_object.mvp_delete_obj.emplace_back(new_image);
	}

	// FADE_IMAGE_NORMAL について
	{
		// 画像設定
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->AddTexture(SPRITE_TEST_2);

		// 色変更
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// 大きさ変更
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetDefaultPosAndColl(screen_width, screen_height);
	}

	{
		// 大きさ変更
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetDefaultPosAndColl(PLAYER_RUN_WIDTH * 2, PLAYER_RUN_HEIGHT * 2);
	}
}

CFadeManager::~CFadeManager()
{
	// オブジェクトの開放
	for (size_t i = 0; i < this->m_object.mvp_delete_obj.size(); i++)
	{
		this->m_object.mvp_delete_obj[i]->UnInitilize();
		delete this->m_object.mvp_delete_obj[i];
	}
}

void CFadeManager::Update()
{
	// 次の状態に移行
	if (this->m_data.m_condeition != this->m_data.m_next_condeition) this->m_data.m_condeition = this->m_data.m_next_condeition;

	// 種類別フェードの更新処理へ
	switch (this->m_data.m_type)
	{
	case FADE_TYPE_00:
		this->UpdateType00();
		break;
	case FADE_TYPE_01:
		this->UpdateType01();
		break;
	case FADE_TYPE_02:
		this->UpdateType02();
		break;
	}
}

void CFadeManager::Draw()
{
	// 種類別フェードの描画処理へ
	switch (this->m_data.m_type)
	{
	case FADE_TYPE_00:
		this->DrawType00();
		break;
	case FADE_TYPE_01:
		this->DrawType01();
		break;
	case FADE_TYPE_02:
		this->DrawType02();
		break;
	}
}

void CFadeManager::StartFadeIn()
{
	// フェードアウトが終わっていなければフェードインできない
	if (this->m_data.m_condeition != FADE_OUT_END) return;

	// フェードインの準備へ
	this->m_data.m_next_condeition = FADE_IN_PREPARE;
}

void CFadeManager::StartFadeOut()
{
	// フェードインが終わっていなければフェードアウトできない
	if (this->m_data.m_condeition != FADE_IN_END) return;

	// フェードアウトの準備へ
	this->m_data.m_next_condeition = FADE_OUT_PREPARE;
}

void CFadeManager::StartFadeOut(FADE_TYPE type)
{
	// フェードインが終わっていなければフェードアウトできない
	if (this->m_data.m_condeition != FADE_IN_END) return;

	// フェードの種類を変更
	if (type >= 0 && type < FADE_TYPE_MAX) this->m_data.m_type = type;

	// フェードアウトの準備へ
	this->m_data.m_next_condeition = FADE_OUT_PREPARE;
}

bool CFadeManager::CheckFadeInEnd()
{
	// フェードインが終わったかどうか
	return this->m_data.m_condeition == FADE_IN_END;
}

bool CFadeManager::CheckFadeOutEnd()
{
	// フェードアウトが終わったかどうか
	return this->m_data.m_condeition == FADE_OUT_END;
}

void CFadeManager::SetType(FADE_TYPE type)
{
	// フェードが見えている間はフェードの種類を変えられない
	if (this->m_data.m_condeition != FADE_IN_END) return;

	// フェードの種類を変更
	if (type >= 0 && type < FADE_TYPE_MAX) this->m_data.m_type = type;
}

void CFadeManager::UpdateType00()
{
	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	switch (this->m_data.m_condeition)
	{
		//==============================
		// フェードイン準備
		//==============================
	case FADE_IN_PREPARE:
	{
		// 黒色にする
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// 真ん中にする
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// 一度すべての描画を止める
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// オブジェクトの描画をするようにする
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// フェードイン実行へ
		this->m_data.m_next_condeition = FADE_IN_ACT;
	}
	break;
	//==============================
	// フェードイン終了まで
	//==============================
	case FADE_IN_ACT:
	{
		// 黒色にする
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a -= 0.05f;

		if (color.a <= 0.0f) {
			color.a = 0.0f;

			// フェードイン終了へ
			this->m_data.m_next_condeition = FADE_IN_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// フェードイン終了中
	//==============================
	case FADE_IN_END:
		break;
		//==============================
		// フェードアウト準備
		//==============================
	case FADE_OUT_PREPARE:
	{
		// 黒色にする
		Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// 真ん中にする
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// 一度すべての描画を止める
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// オブジェクトの描画をするようにする
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// フェードアウト実行へ
		this->m_data.m_next_condeition = FADE_OUT_ACT;
	}
	break;
	//==============================
	// フェードアウト終了まで
	//==============================
	case FADE_OUT_ACT:
	{
		// 黒色にする
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a += 0.05f;

		if (color.a >= 1.0f) {
			color.a = 1.0f;

			// フェードイン終了へ
			this->m_data.m_next_condeition = FADE_OUT_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// フェードアウト終了中
	//==============================
	case FADE_OUT_END:
		break;
	}
}

void CFadeManager::DrawType00()
{
	this->m_object.mvp_image[FADE_IMAGE_NORMAL]->Draw();
}

void CFadeManager::UpdateType01()
{
	switch (this->m_data.m_condeition)
	{
		//==============================
		// フェードイン準備
		//==============================
	case FADE_IN_PREPARE:
	{
		// 黒色にする
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// 真ん中にする
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// 一度すべての描画を止める
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// オブジェクトの描画をするようにする
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// フェードイン実行へ
		this->m_data.m_next_condeition = FADE_IN_ACT;
	}
	break;
	//==============================
	// フェードイン終了まで
	//==============================
	case FADE_IN_ACT:
	{
		// 透過する
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();
		color.a -= 0.25f;

		if (color.a <= 0.0f) {
			color.a = 0.0f;

			// フェードイン終了へ
			this->m_data.m_next_condeition = FADE_IN_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// フェードイン終了中
	//==============================
	case FADE_IN_END:
		break;
		//==============================
		// フェードアウト準備
		//==============================
	case FADE_OUT_PREPARE:
	{
		// 黒色にする
		Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// 真ん中にする
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// 一度すべての描画を止める
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// オブジェクトの描画をするようにする
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// フェードアウト実行へ
		this->m_data.m_next_condeition = FADE_OUT_ACT;
	}
	break;
	//==============================
	// フェードアウト終了まで
	//==============================
	case FADE_OUT_ACT:
	{
		// 透過する
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();
		color.a += 0.25f;

		if (color.a >= 1.0f) {
			color.a = 1.0f;

			// フェードイン終了へ
			this->m_data.m_next_condeition = FADE_OUT_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// フェードアウト終了中
	//==============================
	case FADE_OUT_END:
		break;
	}
}

void CFadeManager::DrawType01()
{
	this->m_object.mvp_image[FADE_IMAGE_NORMAL]->Draw();
}

void CFadeManager::UpdateType02()
{
	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	this->m_object.mvp_image[FADE_IMAGE_PLAYER]->Update();

	switch (this->m_data.m_condeition)
	{
		//==============================
		// フェードイン準備
		//==============================
	case FADE_IN_PREPARE:
	{
		// 黒色にする
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// 真ん中にする
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// 黒色にする
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);

		// 真ん中にする
		transform = this->m_object.mvp_image[FADE_IMAGE_PLAYER]->GetTransform();
		transform.position.x = 500.0f;
		transform.position.y = -200.0f;
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetTransform(&transform);

		// 一度すべての描画を止める
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// オブジェクトの描画をするようにする
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetFlg(&flg);

		// フェードイン実行へ
		this->m_data.m_next_condeition = FADE_IN_ACT;
	}
	break;
	//==============================
	// フェードイン終了まで
	//==============================
	case FADE_IN_ACT:
	{
		// 黒色にする
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a -= 0.05f;

		if (color.a <= 0.0f) {
			color.a = 0.0f;
			this->m_data.mi_fade_count++;

			// フェードイン終了へ
			this->m_data.m_next_condeition = FADE_IN_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;

		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);
	}
	break;
	//==============================
	// フェードイン終了中
	//==============================
	case FADE_IN_END:
		break;
		//==============================
		// フェードアウト準備
		//==============================
	case FADE_OUT_PREPARE:
	{
		// 黒色にする
		Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// 真ん中にする
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		color = { 1.0f, 1.0f, 1.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);

		// 真ん中にする
		transform = this->m_object.mvp_image[FADE_IMAGE_PLAYER]->GetTransform();
		transform.position.x = 500.0f;
		transform.position.y = -200.0f;
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetTransform(&transform);

		// 一度すべての描画を止める
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// オブジェクトの描画をするようにする
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetFlg(&flg);

		this->m_data.mi_fade_count = 0;

		// フェードアウト実行へ
		this->m_data.m_next_condeition = FADE_OUT_ACT;
	}
	break;
	//==============================
	// フェードアウト終了まで
	//==============================
	case FADE_OUT_ACT:
	{
		// 黒色にする
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a += 0.05f;

		if (color.a >= 1.0f) {
			color.a = 1.0f;
			this->m_data.mi_fade_count++;

			if (this->m_data.mi_fade_count > 20) {
				this->m_data.mi_fade_count = 0;
				// フェードイン終了へ
				this->m_data.m_next_condeition = FADE_OUT_END;
			}
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;

		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);
	}
	break;
	//==============================
	// フェードアウト終了中
	//==============================
	case FADE_OUT_END:
		break;
	}
}

void CFadeManager::DrawType02()
{
	this->m_object.mvp_image[FADE_IMAGE_NORMAL]->Draw();
	this->m_object.mvp_image[FADE_IMAGE_PLAYER]->Draw();
}
