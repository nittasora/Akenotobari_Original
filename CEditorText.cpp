#include "CEditorText.h"

void CEditorText::Initilize()
{
	// 画像生成
	this->AddTexture(SPRITE_TEST_CHAR_0);

	// 画像・コライダーサイズの適用
	this->SetDefaultPosAndColl(CHAR_WIDTH, CHAR_HEIGHT);
}

void CEditorText::UnInitilize()
{
}

void CEditorText::SetSprite(int id, UV uv)
{
	// 画像変更
	this->ChangeTexture(id);

	// uv設定
	this->m_anim = uv;
}
