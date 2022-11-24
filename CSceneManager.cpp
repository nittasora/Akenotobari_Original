#include "CSceneManager.h"
#include "CSingleton.h"
#include "CDirect3D.h"
#include "CFadeManager.h"
#include "CMenuManager.h"
#include "CGameManager.h"
#include "CSpriteFactory.h"
#include "CXAudio2.h"

#include "CTitle.h"
#include "CStageSelect.h"
#include "CStagePlay.h"

#include "CEditorSelect.h"
#include "CStageEditor.h"
#include "CEditorSample.h"

void Initializer(CSceneManager* manager, SCENE_INDEX index) {
	manager->SceneInitializer(index);
}

void UnInitializer(CSceneManager* manager, SCENE_INDEX index) {
	manager->SceneUnInitializer(index);
}

CSceneManager::CSceneManager()
{
}

CSceneManager::~CSceneManager()
{
}

void CSceneManager::Initialize()
{
	// �ǂݍ��ݏ����p
	this->m_initializer.condition = THREAD_UN_USE;
	this->m_un_initializer.condition = THREAD_UN_USE;

	// ���[�v�͑�����
	this->mb_loop_continue = true;

	// �摜�̓ǂݍ���
	CSpriteFactory* sorite_factory = CSingleton<CSpriteFactory>::GetInstance();
	sorite_factory->LoadDefaultSprites();

	// �V�[���C���f�b�N�X�̏�����
	this->m_current_scene_index = SCENE_INDEX_NONE;
	this->m_old_scene_index = SCENE_INDEX_NONE;
	this->m_next_scene_index = START_SCENE_INDEX;

	// �N���A�J���[�̏�����
	this->mfa_clear_color[0] = 0.0f;
	this->mfa_clear_color[1] = 0.0f;
	this->mfa_clear_color[2] = 0.0f;
	this->mfa_clear_color[3] = 1.0f;

	this->mp_next_scene = nullptr;
	this->mp_current_scene = nullptr;
	this->mp_old_scene = nullptr;

	// �C���X�^���X�擾
	this->direct3d = CSingleton<CDirect3D>::GetInstance();
	this->fade_manager = CSingleton<CFadeManager>::GetInstance();
	this->menu_manager = CSingleton<CMenuManager>::GetInstance();
	this->game_manager = CSingleton<CGameManager>::GetInstance();

	// �C���X�^���X������
	this->menu_manager->Initialize();

}

void CSceneManager::Finalize()
{
	// �X���b�h�I���҂�
	if (this->m_initializer.condition != THREAD_UN_USE)
	{
		this->m_initializer.thread.join();
	}

	// �X���b�h�I���҂�
	if (this->m_un_initializer.condition != THREAD_UN_USE)
	{
		this->m_un_initializer.thread.join();
	}

	// ���j���[�̏I������
	this->menu_manager->Finalize();
}

void CSceneManager::Update()
{
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();

	if (this->m_next_scene_index != SCENE_INDEX_NONE) {
		switch (this->m_initializer.condition)
		{
		case THREAD_UN_USE:
			// �g�p��ԕύX
			this->m_initializer.condition = THREAD_USE;

			for (int i = SOUND_LABEL_BGM_TITLE; i < SOUND_LABEL_BGM_MAX; i++)
			{
				audio->XA_SetFadeout(static_cast<SOUND_LABEL>(i), 0.0f, 0.1f);
			}

			// �������J�n
			this->m_initializer.thread = std::thread(&Initializer, this, this->m_next_scene_index);

			// �t�F�[�h�A�E�g�J�n
			this->fade_manager->StartFadeOut();
			break;
		case THREAD_WAIT:
			// �t�F�[�h�A�E�g���I�����Ă���V�[����؂�ւ�����
			if (!this->fade_manager->CheckFadeOutEnd()) break;

			this->m_initializer.thread.join();

			// �g�p��ԕύX
			this->m_initializer.condition = THREAD_UN_USE;

			// �V�[������ւ�
			this->m_old_scene_index = this->m_current_scene_index;
			this->m_current_scene_index = this->m_next_scene_index;
			this->m_next_scene_index = SCENE_INDEX_NONE;

			this->mp_current_scene = this->mp_next_scene;
			this->mp_next_scene = nullptr;

			// ������ʏ��Ԃɂ��܂�
			this->mp_update = &(CScene::Update);
			this->mp_draw = &(CScene::Draw);

			this->menu_manager->ChangeMenuSceneEX(MENU_NONE, true);
			this->menu_manager->EraseOldIndex();

			// �t�F�[�h�C�����J�n���܂��B
			this->fade_manager->StartFadeIn();
			break;
		}
	}
	if (this->m_old_scene_index != SCENE_INDEX_NONE) {
		switch (this->m_un_initializer.condition)
		{
		case THREAD_UN_USE:
			// �g�p��ԕύX
			this->m_un_initializer.condition = THREAD_USE;

			// �I���J�n
			this->m_un_initializer.thread = std::thread(&UnInitializer, this, this->m_old_scene_index);
			break;
		case THREAD_WAIT:
			this->m_un_initializer.thread.join();
			// �g�p��ԕύX
			this->m_un_initializer.condition = THREAD_UN_USE;

			// �V�[������ւ�
			this->m_old_scene_index = SCENE_INDEX_NONE;
			break;
		}
	}

	this->game_manager->Update();

	if (this->mp_current_scene != nullptr && this->fade_manager->CheckFadeInEnd()) {
		// �V�[���X�V
		(this->mp_current_scene->*mp_update)();
	}

	this->menu_manager->Update();
	this->fade_manager->Update();

	// ��ʃN���A
	direct3d->GetContext()->ClearRenderTargetView(direct3d->GetRenderTargetView(), this->mfa_clear_color);

	if (this->mp_current_scene != nullptr) {
		// �V�[���`��
		(this->mp_current_scene->*mp_draw)();
	}

	this->menu_manager->Draw();
	this->fade_manager->Draw();

	// ��ʐ؂�ւ�
	direct3d->GetSwapChain()->Present(0, 0);
}

bool CSceneManager::ChangeScene(SCENE_INDEX next_sceneindex)
{
	// ���ɃV�[���؂�ւ����Ȃ玸�s
	if (this->m_initializer.condition != THREAD_UN_USE) return false;
	if (this->m_un_initializer.condition != THREAD_UN_USE) return false;

	// ���Ɏ��̃V�[�����w�肳��Ă���Ȃ玸�s
	if (this->m_next_scene_index != SCENE_INDEX_NONE) return false;

	// �t�F�[�h�C�����I�����ĂȂ��Ă����s
	if (!this->fade_manager->CheckFadeInEnd()) return false;

	// ���̃V�[�����w��
	this->m_next_scene_index = next_sceneindex;

	// ����
	return true;
}

bool CSceneManager::ChangeScene(SCENE_INDEX next_sceneindex, FADE_TYPE type)
{
	// ���ɃV�[���؂�ւ����Ȃ玸�s
	if (this->m_initializer.condition != THREAD_UN_USE) return false;
	if (this->m_un_initializer.condition != THREAD_UN_USE) return false;

	// ���Ɏ��̃V�[�����w�肳��Ă���Ȃ玸�s
	if (this->m_next_scene_index != SCENE_INDEX_NONE) return false;

	// �t�F�[�h�C�����I�����ĂȂ��Ă����s
	if (!this->fade_manager->CheckFadeInEnd()) return false;

	// ���̃V�[�����w��
	this->m_next_scene_index = next_sceneindex;

	// �t�F�[�h�̎�ނ�ύX
	this->fade_manager->SetType(type);

	// ����
	return true;
}

void CSceneManager::FinishLoop()
{
	this->mb_loop_continue = false;
}

bool CSceneManager::CheckLoopContinue()
{
	return this->mb_loop_continue;
}

void CSceneManager::ChangeUpdate(bool is_normal)
{
	if (is_normal) {
		this->mp_update = &(CScene::Update);
		this->mp_draw = &(CScene::Draw);
	}
	else {
		this->mp_update = &(CScene::PauseUpdate);
		this->mp_draw = &(CScene::PauseDraw);
	}
}

void CSceneManager::SceneInitializer(SCENE_INDEX index)
{
	this->mp_old_scene = this->mp_current_scene;

	switch (index)
	{
	case SCENE_TITLE:
		this->mp_next_scene = new CTitle;
		break;
	case SCENE_STAGE_SELECT:
		this->mp_next_scene = new CStageSelect;
		break;
	case SCENE_STAGE_PLAY:
		this->mp_next_scene = new CStagePlay;
		break;
	case SCENE_EDITOR_SELECT:
		this->mp_next_scene = new CEditorSelect;
		break;
	case SCENE_STAGE_EDITOR:
		this->mp_next_scene = new CStageEditor;
		break;
	case SCENE_EDITOR_SAMPLE:
		this->mp_next_scene = new CEditorSample;
		break;
	}
	this->mp_next_scene->Initialize();

	this->m_initializer.condition = THREAD_WAIT;
}

void CSceneManager::SceneUnInitializer(SCENE_INDEX index)
{
	if (this->mp_old_scene != nullptr) {
		this->mp_old_scene->Finalize();
		delete this->mp_old_scene;
		this->mp_old_scene = nullptr;
	}
	this->m_un_initializer.condition = THREAD_WAIT;
}
