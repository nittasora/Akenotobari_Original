#pragma once

#include <vector>
#include "SpriteData.h"

class CSpriteFactory
{
public:
	CSpriteFactory();
	~CSpriteFactory();

	void CreateSprite(SpriteData* sprite, int sprite_id);

	// �X�v���C�g���ǂݍ���
	void LoadSprites(std::vector<IO_SpriteData>& sprite_data);

	// �\�[�X���̃X�v���C�g���ǂݍ���
	void LoadDefaultSprites();

private:
	// �쐬�ς݃X�v���C�g
	std::vector<SpriteData> m_sprites;
};

