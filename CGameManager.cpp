#include "CGameManager.h"

#include <fstream>
#include <iostream>
#include <string>

#include "CCamera.h"
#include "ModelData.h"
#include "UserMath.h"
#include "CDirect3D.h"
#include "CCursor.h"
#include "CXAudio2.h"

#include "CSingleton.h"

namespace
{
	constexpr int FOLD_NUM_MAX = 99;
}

CGameManager::CGameManager() : mui_current_camera(0)
{
	// 一応クリア
	this->mvp_cameras.clear();

	// カメラを作り初期化する
	CCamera* new_camera = new CCamera;
	new_camera->Initilize();

	// 画面の幅
	UINT width;
	UINT height;

	// カメラに画面幅設定
	CSingleton<CDirect3D>::GetInstance()->GetScreenSize(&width, &height);
	new_camera->SetScreenScale(static_cast<float>(width), static_cast<float>(height));

	// カメラを加える
	this->AddCamera(new_camera);

	// カーソルの作成
	this->mp_cursor = new CCursor;
	this->mp_cursor->Initilize();

	// ステージは無し
	this->mi_world_index = -1;
	this->mi_stage_index = -1;

	// ステージを負った回数もなし
	this->mi_fold_num = 0;
	this->mb_fold_switch = false;

	// セーブデータ読み込み
	this->LoadSave();
}

CGameManager::~CGameManager()
{
	// カメラの開放
	for (size_t i = 0; i < this->mvp_cameras.size(); i++)
	{
		this->mvp_cameras[i]->Finalize();
		delete this->mvp_cameras[i];
	}
	this->mvp_cameras.clear();

	// カーソルの開放
	this->mp_cursor->UnInitilize();
	delete this->mp_cursor;
}

void CGameManager::Update()
{
	// カメラの更新
	for (size_t i = 0; i < this->mvp_cameras.size(); i++)
	{
		this->mvp_cameras[i]->Update();
	}

	// カーソルの更新
	this->mp_cursor->Update();
}

Transform CGameManager::GetTransformCamera()
{
	// カメラのトランスフォーム情報取得
	return this->mvp_cameras[this->mui_current_camera]->GetTransform();
}

Transform CGameManager::GetTransformCamera(size_t index)
{
	// 指定したカメラのトランスフォーム情報取得
	if (index < this->mvp_cameras.size()) {
		return this->mvp_cameras[index]->GetTransform();
	}

	//指定したカメラがないので現在のカメラのトランスフォーム情報取得
	return this->mvp_cameras[this->mui_current_camera]->GetTransform();
}

void CGameManager::SetTransformCamera(const Transform * transform)
{
	// カメラのトランスフォーム情報設定
	this->mvp_cameras[this->mui_current_camera]->SetTransform(transform);
}

void CGameManager::SetTransformCamera(const Transform * transform, size_t index)
{
	// 指定したカメラのトランスフォーム情報設定
	if (index < this->mvp_cameras.size()) {
		this->mvp_cameras[index]->SetTransform(transform);
		return;
	}

	//指定したカメラがないので現在のカメラのトランスフォーム情報設定
	this->mvp_cameras[this->mui_current_camera]->SetTransform(transform);
}

size_t CGameManager::AddCamera(CCamera * new_camera)
{
	// カメラの追加
	this->mvp_cameras.emplace_back(new_camera);
	return this->mvp_cameras.size() - 1;
}

bool CGameManager::ChangeCamera(size_t index)
{
	// カメラ変更
	if (index < this->mvp_cameras.size()) {
		this->mui_current_camera = index;
		return true;
	}
	return false;
}

Matrix4_4 CGameManager::Get2DCameraMatrix()
{
	return this->mvp_cameras[this->mui_current_camera]->Get2DMatrix();
}

Matrix4_4 CGameManager::Get2DCameraMatrix(size_t index)
{
	// 指定したカメラがあるなら指定したカメラ
	if (index < this->mvp_cameras.size()) {
		return this->mvp_cameras[index]->Get2DMatrix();
	}
	// ないなら現在のカメラ
	return this->mvp_cameras[this->mui_current_camera]->Get2DMatrix();
}

CCursor * CGameManager::GetCursor()
{
	return this->mp_cursor;
}

int CGameManager::GetWorldIndex()
{
	return this->mi_world_index;
}

void CGameManager::SetWorldIndex(int index)
{
	this->mi_world_index = index;
}

int CGameManager::GetStageIndex()
{
	return this->mi_stage_index;
}

void CGameManager::SetStageIndex(int index)
{
	this->mi_stage_index = index;
}

int CGameManager::GetFoldNum()
{
	return this->mi_fold_num;
}

void CGameManager::SetFoldNum(int new_num)
{
	// 新しい値に
	this->mi_fold_num = new_num;

	// 値を決められた範囲に
	if (this->mi_fold_num > FOLD_NUM_MAX) this->mi_fold_num = FOLD_NUM_MAX;
	if (this->mi_fold_num < 0) this->mi_fold_num = 0;
}

void CGameManager::AddFoldNum()
{
	// 値を増やす
	this->mi_fold_num++;

	// 値を決められた範囲に
	if (this->mi_fold_num > FOLD_NUM_MAX) this->mi_fold_num = FOLD_NUM_MAX;
	if (this->mi_fold_num < 0) this->mi_fold_num = 0;
}

bool CGameManager::GetFoldSwitch()
{
	return this->mb_fold_switch;
}

void CGameManager::SetFoldSwitch(bool new_switch)
{
	this->mb_fold_switch = new_switch;
}

void CGameManager::ChangeFoldSwitch()
{
	this->mb_fold_switch = !this->mb_fold_switch;
}

int CGameManager::GetHP()
{
	return this->mi_player_hp;
}

void CGameManager::SetHP(int new_hp)
{
	// HPは決められた範囲内に合わす
	if (new_hp > HP_MAX) new_hp = HP_MAX;
	if (new_hp < HP_MIN) new_hp = HP_MIN;

	// 新しい値
	this->mi_player_hp = new_hp;
}

void CGameManager::PullHP()
{
	// HPを減らす
	this->mi_player_hp--;

	// 最小値を下回ってはいけない
	if (this->mi_player_hp < HP_MIN) this->mi_player_hp = HP_MIN;
}

bool CGameManager::CheckGetPiece(int piece_num)
{
	// 最大値が指定されたなら全て確認
	if (piece_num == PIECE_NUM) {
		// 一つでも足りなければ false
		for (size_t i = 0; i < this->mab_get_piece.size(); i++)
		{
			if (!this->mab_get_piece[i]) return false;
		}
		return true;
	}
	// 特定のカケラを指定した場合
	else if (0 <= piece_num && piece_num < PIECE_NUM) {
		return this->mab_get_piece[piece_num];
	}
	return false;
}

void CGameManager::SetGetPiece(int piece_num, bool is_get)
{
	// 最大値が指定されたなら全て代入
	if (piece_num == PIECE_NUM) {
		for (size_t i = 0; i < this->mab_get_piece.size(); i++)
		{
			this->mab_get_piece[i] = is_get;
		}
	}
	// 特定のカケラを指定した場合
	else if (0 <= piece_num && piece_num < PIECE_NUM) {
		this->mab_get_piece[piece_num] = is_get;
	}
}

bool CGameManager::CheckGoal()
{
	return this->mb_is_goal;
}

void CGameManager::SetGoal(bool is_goal)
{
	this->mb_is_goal = is_goal;
}

void CGameManager::InitilizeStageData()
{
	// 折り回数 0
	this->SetFoldNum(0);

	// 折りスイッチを初期化
	this->SetFoldSwitch(false);

	// HPを最大値に
	this->SetHP(HP_MAX);

	// 全カケラを未取得に
	this->SetGetPiece(PIECE_NUM, false);

	// ゴールしていない
	this->SetGoal(false);
}

SAVE_DATA CGameManager::GetSave()
{
	return this->m_save_piece;
}

void CGameManager::SavePiece()
{
	// カケラを取得したなら、セーブデータを更新
	if (this->mi_world_index >= 0 && this->mi_world_index < GAME_WORLD_MAX) {
		if (this->mi_stage_index >= 0 && this->mi_stage_index < GAME_STAGE_MAX) {
			for (size_t i = 0; i < PIECE_NUM; i++)
			{
				if (this->mab_get_piece[i]) {
					this->m_save_piece[this->mi_world_index][this->mi_stage_index][i] = true;
				}
			}
		}
	}

	std::string file_name = "assets/Game/Save/save.dat";

	std::ofstream ofs(file_name);

	// テキストファイルに書き出す
	if (!ofs.fail()) {
		for (size_t w = 0; w < this->m_save_piece.size(); w++)
		{
			for (size_t s = 0; s < this->m_save_piece[w].size(); s++)
			{
				for (size_t i = 0; i < this->m_save_piece[w][s].size(); i++)
				{
					ofs << this->m_save_piece[w][s][i] << std::endl;
				}
			}
		}

		// (音量も書き出す)
		CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();
		ofs << audio->XA_GetCommonVolume() << std::endl;
	}
}

void CGameManager::LoadSave()
{
	// カケラを全て未所持に
	for (size_t w = 0; w < this->m_save_piece.size(); w++)
	{
		for (size_t s = 0; s < this->m_save_piece[w].size(); s++)
		{
			for (size_t i = 0; i < this->m_save_piece[w][s].size(); i++)
			{
				this->m_save_piece[w][s][i] = false;
			}
		}
	}

	std::string file_name = "assets/Game/Save/save.dat";

	std::ifstream ifs(file_name);

	// テキストファイルから読みだす
	if (!ifs.fail()) {
		for (size_t w = 0; w < this->m_save_piece.size(); w++)
		{
			for (size_t s = 0; s < this->m_save_piece[w].size(); s++)
			{
				for (size_t i = 0; i < this->m_save_piece[w][s].size(); i++)
				{
					ifs >> this->m_save_piece[w][s][i];
				}
			}
		}
		// (音量も読み込む)
		CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();
		float volume;
		ifs >> volume;
		audio->XA_SetCommonVolume(volume);
	}
}

void CGameManager::DeleteSave()
{
	// カケラを全て未所持に
	for (size_t i = 0; i < this->m_save_piece.size(); i++)
	{
		for (size_t j = 0; j < this->m_save_piece[i].size(); j++)
		{
			for (size_t k = 0; k < this->m_save_piece[i][j].size(); k++)
			{
				this->m_save_piece[i][j][k] = false;
			}
		}
	}

	std::string file_name = "assets/Game/Save/save.dat";

	std::ofstream ofs(file_name);

	// セーブ用テキストファイルを初期値に書き換える
	if (!ofs.fail()) {
		for (size_t w = 0; w < this->m_save_piece.size(); w++)
		{
			for (size_t s = 0; s < this->m_save_piece[w].size(); s++)
			{
				for (size_t i = 0; i < this->m_save_piece[w][s].size(); i++)
				{
					ofs << this->m_save_piece[w][s][i] << std::endl;
				}
			}
		}

		ofs << 0.5f << std::endl;
	}
}
