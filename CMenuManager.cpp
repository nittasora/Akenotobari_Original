#include "CMenuManager.h"
#include "CSceneManager.h"
#include "CSingleton.h"

#include "CMPause.h"
#include "CMTips.h"

CMenuManager::CMenuManager()
{
}

CMenuManager::~CMenuManager()
{
}

void CMenuManager::Initialize()
{
	// シーンインデックスの初期化
	this->m_current_scene_index = MENU_NONE;
	while (!this->m_old_scene_index.empty())
	{
		this->m_old_scene_index.pop();
	}

	// 各シーンを作成
	this->mpa_scenes[MENU_NONE] = CSingleton<CMenuScene>::GetInstance();
	this->mpa_scenes[MENU_PAUSE] = CSingleton<CMPause>::GetInstance();
	this->mpa_scenes[MENU_TIPS] = CSingleton<CMTips>::GetInstance();

	// 各シーンを初期化
	for (int i = 0; i < MENU_INDEX_MAX; i++)
	{
		this->mpa_scenes[i]->Initialize();
	}

}

void CMenuManager::Finalize()
{
	for (int i = 0; i < MENU_INDEX_MAX; i++)
	{
		this->mpa_scenes[i]->Finalize();
	}
}

void CMenuManager::Update()
{
	this->mpa_scenes[this->m_current_scene_index]->Update();
}

void CMenuManager::Draw()
{
	this->mpa_scenes[this->m_current_scene_index]->Draw();
}

void CMenuManager::ChangeMenuScene(MENU_INDEX index)
{
	// 現在と同じメニューなら無視
	if (this->m_current_scene_index == index) return;

	// ひとつ前のメニューに登録し、変更
	this->m_old_scene_index.push(this->m_current_scene_index);
	this->m_current_scene_index = index;
	this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);
}

void CMenuManager::ChangeMenuSceneEX(MENU_INDEX index, bool is_not_pause)
{
	// 現在と同じメニューなら無視
	if (this->m_current_scene_index == index) return;

	// ひとつ前のメニューに登録し、変更
	this->m_old_scene_index.push(this->m_current_scene_index);
	this->m_current_scene_index = index;
	this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);

	// さらにPause状態をいじる
	CSingleton<CSceneManager>::GetInstance()->ChangeUpdate(is_not_pause);
}

void CMenuManager::BackMenuScene()
{
	// ひとつ前のメニューがない
	if (this->m_old_scene_index.empty()) {
		this->m_current_scene_index = MENU_NONE;
	}
	else {
		// スタックからひとつ前のメニューを取り出し変更
		this->m_current_scene_index = this->m_old_scene_index.top();
		this->m_old_scene_index.pop();
		this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);
	}
}

void CMenuManager::BackMenuSceneEX(bool is_not_pause)
{
	// ひとつ前のメニューがない
	if (this->m_old_scene_index.empty()) {
		this->m_current_scene_index = MENU_NONE;
		CSingleton<CSceneManager>::GetInstance()->ChangeUpdate(is_not_pause);
	}
	else {
		// スタックからひとつ前のメニューを取り出し変更
		this->m_current_scene_index = this->m_old_scene_index.top();
		this->m_old_scene_index.pop();
		this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);
	}

	// さらにメニューがないならPause状態をいじる
	if (this->m_current_scene_index == MENU_NONE) {
		CSingleton<CSceneManager>::GetInstance()->ChangeUpdate(is_not_pause);
	}
}

void CMenuManager::EraseOldIndex()
{
	// なくなるまで取り出す
	while (!this->m_old_scene_index.empty())
	{
		this->m_old_scene_index.pop();
	}
}
