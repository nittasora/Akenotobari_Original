#include "CMenuScene.h"

#include "CDirect3D.h"
#include "CXAudio2.h"
#include "CInput.h"
#include "CTimer.h"

#include "CSingleton.h"

CMenuScene::CMenuScene()
{
	this->mp_directx = CSingleton<CDirect3D>::GetInstance();
	this->mp_xaudio2 = CSingleton<CXAudio2>::GetInstance();
	this->mp_input = CSingleton<CInput>::GetInstance();
	this->mp_timer = CSingleton<CTimer>::GetInstance();
}

CMenuScene::~CMenuScene()
{
	this->mp_directx = nullptr;
	this->mp_xaudio2 = nullptr;
	this->mp_input = nullptr;
	this->mp_timer = nullptr;

	this->m_current_condition = MENU_FADE_IN_PREPARE;
	this->m_next_condition = MENU_FADE_IN_PREPARE;
}

void CMenuScene::Initialize()
{
	// 何もしないことに定評のあるメニュー
}

void CMenuScene::Finalize()
{
	// 何もしないことに定評のあるメニュー
}

void CMenuScene::Update()
{
	// 何もしないことに定評のあるメニュー
}

void CMenuScene::Draw()
{
	// 何もしないことに定評のあるメニュー
}

void CMenuScene::SetCondition(MENU_CONDITION next)
{
	this->m_next_condition = next;
}
