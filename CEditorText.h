#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// エディターで使用する文字の画像。
// 決め打ちでuvを変更する
//
// ※ゲーム本体で使う場合は、最新版CImageを推奨

#include "CImage.h"

class CEditorText : public CImage
{
public:
	// 初期化処理
	virtual void Initilize() override;

	// 終了処理
	virtual void UnInitilize() override;

	// 画像設定
	void SetSprite(int id, UV uv);
};

