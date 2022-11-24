#pragma once

#include "CImage.h"

class CButton : public CImage
{
public:
	// 初期化
	void Initilize() override;

	// 終了
	void UnInitilize() override;

	// 更新
	void Update() override;

	// 当たり判定
	void OnCollision(COLLIDER_RESULT* collider_res, CGameObject* other) override;

	// OnかOffかを取得
	bool GetONOFF();

	// OnかOffかを設定
	void SetONOFF(bool is_on);

	// スプライトを設定
	void SetSprite(SPRITE_INDEX sprite, UV point, bool anim_on);

	// 入力するキー及びボタンの設定
	void SetButton(int key, int button);

	// 他のボタンを設定
	void SetOtherButtonObject(CButton* other_button);

private:
	// 推したアニメーションをするか
	bool mb_anim_on;

	// onかoffか
	bool mb_is_on;

	// 画像のどこを使うか
	UV m_point;

	// インプット用ボタン
	int mi_key;
	int mi_button;

	// 他のボタン
	std::vector<CButton*> mvp_other_buttons;
};

