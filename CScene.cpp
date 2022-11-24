#include "CScene.h"

#include "CDirect3D.h"
#include "CXAudio2.h"
#include "CInput.h"
#include "CTimer.h"

#include "CSingleton.h"

CScene::CScene()
{
	//this->mb_initialized_flg = false;
	this->mp_directx = CSingleton<CDirect3D>::GetInstance();
	this->mp_xaudio2 = CSingleton<CXAudio2>::GetInstance();
	this->mp_input = CSingleton<CInput>::GetInstance();
	this->mp_timer = CSingleton<CTimer>::GetInstance();
	mb_initialized_flg = false;
}

CScene::~CScene()
{
	this->mp_directx = nullptr;
	this->mp_xaudio2 = nullptr;
	this->mp_input = nullptr;
	this->mp_timer = nullptr;
}

//bool CScene::GetInitializedFlag()
//{
//	return this->mb_initialized_flg;
//}
//
//void CScene::SetInitializedFlag(bool flg)
//{
//	this->mb_initialized_flg = flg;
//}
