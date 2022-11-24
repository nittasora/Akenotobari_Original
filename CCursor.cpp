#include "CCursor.h"
#include "CInput.h"
#include "CGameManager.h"
#include "CDirect3D.h"

#include "CSingleton.h"

void CCursor::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_TEST_0);

	// 判定タグはカーソル
	this->m_tag = TAG_CURSOR;

	// 画像・コライダーサイズの適用
	this->SetDefaultPosAndColl(4.0f, 4.0f);
}

void CCursor::UnInitilize()
{
}

void CCursor::Update()
{
	// 前回からの移動量を取得
	POINT cursor_deff = CSingleton<CInput>::GetInstance()->GetCursorClientDefference();

	// カーソルが移動しているなら
	if (cursor_deff.x > 1 || cursor_deff.x < -1 || cursor_deff.y > 1 || cursor_deff.y < -1) {

		// カメラ座標取得
		Transform camera_tr = CSingleton<CGameManager>::GetInstance()->GetTransformCamera();

		// スクリーンサイズ取得(基準座標変更のため)
		UINT screen_width;
		UINT screen_height;
		CSingleton<CDirect3D>::GetInstance()->GetScreenSize(&screen_width, &screen_height);

		// カーソル位置の計算
		POINT cursor_pos = CSingleton<CInput>::GetInstance()->GetCursorClientPosition();

		// (カーソル位置 X + カメラ位置 X - スクリーンサイズ X の半分)
		float cursor_x = static_cast<float>(cursor_pos.x) + camera_tr.position.x - (static_cast<float>(screen_width) / 2.0f);

		// (-カーソル位置 Y + カメラ位置 Y - スクリーンサイズ Y の半分)
		float cursor_y = static_cast<float>(-cursor_pos.y) + camera_tr.position.y + (static_cast<float>(screen_height) / 2.0f);

		// 座標のセット
		this->m_transform.position.x = cursor_x;
		this->m_transform.position.y = cursor_y;
	}
}
