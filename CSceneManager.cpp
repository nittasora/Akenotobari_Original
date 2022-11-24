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
	// 読み込み処理用
	this->m_initializer.condition = THREAD_UN_USE;
	this->m_un_initializer.condition = THREAD_UN_USE;

	// ループは続ける
	this->mb_loop_continue = true;

	// 画像の読み込み
	CSpriteFactory* sorite_factory = CSingleton<CSpriteFactory>::GetInstance();
	sorite_factory->LoadDefaultSprites();

	// シーンインデックスの初期化
	this->m_current_scene_index = SCENE_INDEX_NONE;
	this->m_old_scene_index = SCENE_INDEX_NONE;
	this->m_next_scene_index = START_SCENE_INDEX;

	// クリアカラーの初期化
	this->mfa_clear_color[0] = 0.0f;
	this->mfa_clear_color[1] = 0.0f;
	this->mfa_clear_color[2] = 0.0f;
	this->mfa_clear_color[3] = 1.0f;

	this->mp_next_scene = nullptr;
	this->mp_current_scene = nullptr;
	this->mp_old_scene = nullptr;

	// インスタンス取得
	this->direct3d = CSingleton<CDirect3D>::GetInstance();
	this->fade_manager = CSingleton<CFadeManager>::GetInstance();
	this->menu_manager = CSingleton<CMenuManager>::GetInstance();
	this->game_manager = CSingleton<CGameManager>::GetInstance();

	// インスタンス初期化
	this->menu_manager->Initialize();

}

void CSceneManager::Finalize()
{
	// スレッド終了待ち
	if (this->m_initializer.condition != THREAD_UN_USE)
	{
		this->m_initializer.thread.join();
	}

	// スレッド終了待ち
	if (this->m_un_initializer.condition != THREAD_UN_USE)
	{
		this->m_un_initializer.thread.join();
	}

	// メニューの終了処理
	this->menu_manager->Finalize();
}

void CSceneManager::Update()
{
	CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();

	if (this->m_next_scene_index != SCENE_INDEX_NONE) {
		switch (this->m_initializer.condition)
		{
		case THREAD_UN_USE:
			// 使用状態変更
			this->m_initializer.condition = THREAD_USE;

			for (int i = SOUND_LABEL_BGM_TITLE; i < SOUND_LABEL_BGM_MAX; i++)
			{
				audio->XA_SetFadeout(static_cast<SOUND_LABEL>(i), 0.0f, 0.1f);
			}

			// 初期化開始
			this->m_initializer.thread = std::thread(&Initializer, this, this->m_next_scene_index);

			// フェードアウト開始
			this->fade_manager->StartFadeOut();
			break;
		case THREAD_WAIT:
			// フェードアウトが終了してからシーンを切り替えたい
			if (!this->fade_manager->CheckFadeOutEnd()) break;

			this->m_initializer.thread.join();

			// 使用状態変更
			this->m_initializer.condition = THREAD_UN_USE;

			// シーン入れ替え
			this->m_old_scene_index = this->m_current_scene_index;
			this->m_current_scene_index = this->m_next_scene_index;
			this->m_next_scene_index = SCENE_INDEX_NONE;

			this->mp_current_scene = this->mp_next_scene;
			this->mp_next_scene = nullptr;

			// 処理を通常状態にします
			this->mp_update = &(CScene::Update);
			this->mp_draw = &(CScene::Draw);

			this->menu_manager->ChangeMenuSceneEX(MENU_NONE, true);
			this->menu_manager->EraseOldIndex();

			// フェードインを開始します。
			this->fade_manager->StartFadeIn();
			break;
		}
	}
	if (this->m_old_scene_index != SCENE_INDEX_NONE) {
		switch (this->m_un_initializer.condition)
		{
		case THREAD_UN_USE:
			// 使用状態変更
			this->m_un_initializer.condition = THREAD_USE;

			// 終了開始
			this->m_un_initializer.thread = std::thread(&UnInitializer, this, this->m_old_scene_index);
			break;
		case THREAD_WAIT:
			this->m_un_initializer.thread.join();
			// 使用状態変更
			this->m_un_initializer.condition = THREAD_UN_USE;

			// シーン入れ替え
			this->m_old_scene_index = SCENE_INDEX_NONE;
			break;
		}
	}

	this->game_manager->Update();

	if (this->mp_current_scene != nullptr && this->fade_manager->CheckFadeInEnd()) {
		// シーン更新
		(this->mp_current_scene->*mp_update)();
	}

	this->menu_manager->Update();
	this->fade_manager->Update();

	// 画面クリア
	direct3d->GetContext()->ClearRenderTargetView(direct3d->GetRenderTargetView(), this->mfa_clear_color);

	if (this->mp_current_scene != nullptr) {
		// シーン描画
		(this->mp_current_scene->*mp_draw)();
	}

	this->menu_manager->Draw();
	this->fade_manager->Draw();

	// 画面切り替え
	direct3d->GetSwapChain()->Present(0, 0);
}

bool CSceneManager::ChangeScene(SCENE_INDEX next_sceneindex)
{
	// 既にシーン切り替え中なら失敗
	if (this->m_initializer.condition != THREAD_UN_USE) return false;
	if (this->m_un_initializer.condition != THREAD_UN_USE) return false;

	// 既に次のシーンが指定されているなら失敗
	if (this->m_next_scene_index != SCENE_INDEX_NONE) return false;

	// フェードインが終了してなくても失敗
	if (!this->fade_manager->CheckFadeInEnd()) return false;

	// 次のシーンを指定
	this->m_next_scene_index = next_sceneindex;

	// 成功
	return true;
}

bool CSceneManager::ChangeScene(SCENE_INDEX next_sceneindex, FADE_TYPE type)
{
	// 既にシーン切り替え中なら失敗
	if (this->m_initializer.condition != THREAD_UN_USE) return false;
	if (this->m_un_initializer.condition != THREAD_UN_USE) return false;

	// 既に次のシーンが指定されているなら失敗
	if (this->m_next_scene_index != SCENE_INDEX_NONE) return false;

	// フェードインが終了してなくても失敗
	if (!this->fade_manager->CheckFadeInEnd()) return false;

	// 次のシーンを指定
	this->m_next_scene_index = next_sceneindex;

	// フェードの種類を変更
	this->fade_manager->SetType(type);

	// 成功
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
