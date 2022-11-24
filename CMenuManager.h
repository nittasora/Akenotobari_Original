#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// メニューの管理

#include <stack>

#include "MenuData.h"
#include "CMenuScene.h"

class CMenuManager
{
public:
	CMenuManager();
	~CMenuManager();

	// 初期化処理
	void Initialize();

	// 終了処理
	void Finalize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	// メニュー変更
	void ChangeMenuScene(MENU_INDEX index);

	// メニュー変更し、Pause状態を変える
	void ChangeMenuSceneEX(MENU_INDEX index, bool is_not_pause);

	// メニューをひとつ戻る
	void BackMenuScene();

	// メニューを一つ戻り、メニューを閉じるならPause状態を変える
	void BackMenuSceneEX(bool is_not_pause);

	// ひとつ前のメニューをすべて消す
	void EraseOldIndex();

private:
	//=============================
	// シーン管理用
	//=============================

	// シーンをまとめる用
	CMenuScene* mpa_scenes[MENU_INDEX_MAX];

	// 前のシーンインデックス
	std::stack<MENU_INDEX> m_old_scene_index;

	// 現在のシーンインデックス
	MENU_INDEX m_current_scene_index;
};

