#pragma once

#include <vector>
#include "SpriteData.h"

class CSpriteFactory
{
public:
	CSpriteFactory();
	~CSpriteFactory();

	void CreateSprite(SpriteData* sprite, int sprite_id);

	// スプライト情報読み込み
	void LoadSprites(std::vector<IO_SpriteData>& sprite_data);

	// ソース内のスプライト情報読み込み
	void LoadDefaultSprites();

private:
	// 作成済みスプライト
	std::vector<SpriteData> m_sprites;
};

