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
	// �V�[���C���f�b�N�X�̏�����
	this->m_current_scene_index = MENU_NONE;
	while (!this->m_old_scene_index.empty())
	{
		this->m_old_scene_index.pop();
	}

	// �e�V�[�����쐬
	this->mpa_scenes[MENU_NONE] = CSingleton<CMenuScene>::GetInstance();
	this->mpa_scenes[MENU_PAUSE] = CSingleton<CMPause>::GetInstance();
	this->mpa_scenes[MENU_TIPS] = CSingleton<CMTips>::GetInstance();

	// �e�V�[����������
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
	// ���݂Ɠ������j���[�Ȃ疳��
	if (this->m_current_scene_index == index) return;

	// �ЂƂO�̃��j���[�ɓo�^���A�ύX
	this->m_old_scene_index.push(this->m_current_scene_index);
	this->m_current_scene_index = index;
	this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);
}

void CMenuManager::ChangeMenuSceneEX(MENU_INDEX index, bool is_not_pause)
{
	// ���݂Ɠ������j���[�Ȃ疳��
	if (this->m_current_scene_index == index) return;

	// �ЂƂO�̃��j���[�ɓo�^���A�ύX
	this->m_old_scene_index.push(this->m_current_scene_index);
	this->m_current_scene_index = index;
	this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);

	// �����Pause��Ԃ�������
	CSingleton<CSceneManager>::GetInstance()->ChangeUpdate(is_not_pause);
}

void CMenuManager::BackMenuScene()
{
	// �ЂƂO�̃��j���[���Ȃ�
	if (this->m_old_scene_index.empty()) {
		this->m_current_scene_index = MENU_NONE;
	}
	else {
		// �X�^�b�N����ЂƂO�̃��j���[�����o���ύX
		this->m_current_scene_index = this->m_old_scene_index.top();
		this->m_old_scene_index.pop();
		this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);
	}
}

void CMenuManager::BackMenuSceneEX(bool is_not_pause)
{
	// �ЂƂO�̃��j���[���Ȃ�
	if (this->m_old_scene_index.empty()) {
		this->m_current_scene_index = MENU_NONE;
		CSingleton<CSceneManager>::GetInstance()->ChangeUpdate(is_not_pause);
	}
	else {
		// �X�^�b�N����ЂƂO�̃��j���[�����o���ύX
		this->m_current_scene_index = this->m_old_scene_index.top();
		this->m_old_scene_index.pop();
		this->mpa_scenes[this->m_current_scene_index]->SetCondition(MENU_FADE_IN_PREPARE);
	}

	// ����Ƀ��j���[���Ȃ��Ȃ�Pause��Ԃ�������
	if (this->m_current_scene_index == MENU_NONE) {
		CSingleton<CSceneManager>::GetInstance()->ChangeUpdate(is_not_pause);
	}
}

void CMenuManager::EraseOldIndex()
{
	// �Ȃ��Ȃ�܂Ŏ��o��
	while (!this->m_old_scene_index.empty())
	{
		this->m_old_scene_index.pop();
	}
}
