#include "CStagePlay.h"

#include "CStageManager.h"
#include "CInput.h"
#include "CButton.h"
#include "CCursor.h"

#include "CDirect3D.h"
#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"
#include "CTimer.h"

#include "CSingleton.h"

namespace
{
	constexpr float TIPS_TIME = 1.0f;
}

CStagePlay::CStagePlay()
{
	this->mp_stage_manager = nullptr;
}

CStagePlay::~CStagePlay()
{
}

void CStagePlay::Initialize()
{
	this->mp_stage_manager = new CStageManager;
	this->mf_time = 0.0f;
}

void CStagePlay::Finalize()
{
	if (this->mp_stage_manager != nullptr) {
		delete this->mp_stage_manager;
		this->mp_stage_manager = nullptr;
	}
}

void CStagePlay::Update()
{
	if (this->mf_time < TIPS_TIME) {
		this->mf_time += this->mp_timer->GetDeltaTime();
		if (this->mf_time >= TIPS_TIME) {
			CSingleton<CMenuManager>::GetInstance()->ChangeMenuSceneEX(MENU_TIPS, false);
		}
	}

	this->mp_stage_manager->Update();
}

void CStagePlay::Draw()
{
	this->mp_stage_manager->Draw();
}

void CStagePlay::PauseUpdate()
{

}

void CStagePlay::PauseDraw()
{
	this->Draw();
}
