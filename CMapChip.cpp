#include "CMapChip.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CGameManager.h"

#include "StageData.h"

#include "CSingleton.h"

void CMapChip::Initilize()
{
	this->m_tag = TAG_MAPCHIP;
	this->AddTexture(SPRITE_TEST_0);
	this->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);
}

void CMapChip::UnInitilize()
{
}
