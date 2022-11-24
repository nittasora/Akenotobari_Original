#pragma once

// 作成 : 新田

//=============================
// 概要など
//=============================
// ゲームで使う変数やカメラの管理

#include <vector>
#include <array>
#include "StageData.h"

struct Transform;
struct Matrix4_4;

class CCamera;
class CCursor;

// セーブデータ用の変数
using SAVE_DATA = std::array<std::array<std::array<bool, PIECE_NUM>, GAME_STAGE_MAX>, GAME_WORLD_MAX>;

class CGameManager
{
public:
	CGameManager();
	~CGameManager();

	// 更新処理
	void Update();

	// カメラのトランスフォーム情報取得
	Transform GetTransformCamera();
	Transform GetTransformCamera(size_t index);

	// カメラのトランスフォーム情報設定
	void SetTransformCamera(const Transform* transform);
	void SetTransformCamera(const Transform* transform, size_t index);

	// カメラの追加
	// 戻り値は新しいインデックス
	size_t AddCamera(CCamera* new_camera);

	// カメラの変更
	bool ChangeCamera(size_t index);

	// 2Dカメラ用行列の取得
	Matrix4_4 Get2DCameraMatrix();
	Matrix4_4 Get2DCameraMatrix(size_t index);

	// カーソルの取得
	CCursor* GetCursor();

	// ワールドの要素数のGet・Set
	int GetWorldIndex();
	void SetWorldIndex(int index);

	// ステージの要素数のGet・Set
	int GetStageIndex();
	void SetStageIndex(int index);

	// 折った回数関係
	int GetFoldNum();
	void SetFoldNum(int new_num);
	void AddFoldNum();

	// 折ったかどうか関係
	bool GetFoldSwitch();
	void SetFoldSwitch(bool new_switch);
	void ChangeFoldSwitch();

	// HP関係
	int GetHP();
	void SetHP(int new_hp);
	void PullHP();

	// 取得したカケラ関係
	bool CheckGetPiece(int piece_num);
	void SetGetPiece(int piece_num, bool is_get);

	// ゴール関係
	bool CheckGoal();
	void SetGoal(bool is_goal);

	// ステージで使うデータの初期化
	void InitilizeStageData();

	// 入手したカケラ関係
	SAVE_DATA GetSave();
	void SavePiece();
	void LoadSave();
	void DeleteSave();

private:
	// 現在のカメラ
	size_t mui_current_camera;

	// カメラたち
	std::vector<CCamera*> mvp_cameras;

	// カーソル
	CCursor* mp_cursor;

	// ワールドの要素数
	int mi_world_index;

	// ステージの要素数
	int mi_stage_index;

	// 折った回数
	int mi_fold_num;

	// 赤青ブロック用
	bool mb_fold_switch;

	// プレイヤーHP
	int mi_player_hp;

	// ステージで集めたカケラ
	std::array<bool, PIECE_NUM> mab_get_piece;

	// 集めたカケラのデータ
	SAVE_DATA m_save_piece;

	// ゴール判定
	bool mb_is_goal;
};

