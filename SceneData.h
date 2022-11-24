#pragma once

// シーンの要素番号
enum SCENE_INDEX
{
	// ゲームシーン

	//SCENE_SAMPLE,							// サンプルシーンインデックス
	SCENE_TITLE,							// タイトルシーンインデックス
	SCENE_STAGE_SELECT,						// ステージ選択シーンインデックス
	SCENE_STAGE_PLAY,						// ゲームシーンインデックス

	// エディターシーン

	SCENE_EDITOR_SELECT,					// エディター選択シーン
	SCENE_STAGE_EDITOR,						// ステージエディターシーン
	SCENE_EDITOR_SAMPLE,					// エディターのサンプルシーン

	SCENE_INDEX_MAX,						// シーンの数
	SCENE_INDEX_NONE = SCENE_INDEX_MAX		// 次のシーンがない状態
};

// 最初のシーン
constexpr SCENE_INDEX START_SCENE_INDEX = SCENE_TITLE;