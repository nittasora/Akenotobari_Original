#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// ゲームシーンの管理
// スレッドセーフじゃないので注意してください

#include <array>
#include <thread>
#include <Windows.h>
#include "SceneData.h"
#include "CScene.h"
#include "FadeData.h"

// スレッドの状態
enum TreadCondition
{
	THREAD_UN_USE,
	THREAD_USE,
	THREAD_WAIT
};

class CDirect3D;
class CFadeManager;
class CMenuManager;
class CGameManager;

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();

	//=============================
	// 処理関係
	//=============================

	// 初期化処理
	void Initialize();

	// 終了処理
	void Finalize();

	// 更新処理
	void Update();

	// シーン切り替え(競合した場合早い者勝ち) -> 失敗で false
	bool ChangeScene(SCENE_INDEX next_sceneindex);
	bool ChangeScene(SCENE_INDEX next_sceneindex, FADE_TYPE type);

	// ループを終了するフラグを立てる
	void FinishLoop();

	// ループが続いているかを返す
	bool CheckLoopContinue();

	// 更新処理を入れ替えます
	void ChangeUpdate(bool is_normal);

	// 初期化・終了
	void SceneInitializer(SCENE_INDEX index);
	void SceneUnInitializer(SCENE_INDEX index);

private:
	//=============================
	// シーン管理用
	//=============================

	// 現在のシーンインデックス
	SCENE_INDEX m_current_scene_index;

	// 次のシーンインデックス
	SCENE_INDEX m_next_scene_index;

	// 前のシーンインデックス
	SCENE_INDEX m_old_scene_index;

	// ループを続けるかどうかのフラグ
	bool mb_loop_continue;

	// 初期化用スレッド
	struct
	{
		std::thread thread;
		TreadCondition condition;
	} m_initializer;

	// 解放用スレッド
	struct
	{
		std::thread thread;
		TreadCondition condition;
	} m_un_initializer;

	// ウィンドウ塗りつぶしようカラー
	FLOAT mfa_clear_color[4];

	// 関数ポインタに別名を付ける
	using SceneFunc = void(CScene::*)();

	// 更新・描画
	SceneFunc mp_update;
	SceneFunc mp_draw;

	// シーン入れ替え用
	CScene* mp_next_scene;
	CScene* mp_current_scene;
	CScene* mp_old_scene;

	// 各種マネージャー
	CDirect3D* direct3d;
	CFadeManager* fade_manager;
	CMenuManager* menu_manager;
	CGameManager* game_manager;
};

