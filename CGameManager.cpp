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
	// �ꉞ�N���A
	this->mvp_cameras.clear();

	// �J��������菉��������
	CCamera* new_camera = new CCamera;
	new_camera->Initilize();

	// ��ʂ̕�
	UINT width;
	UINT height;

	// �J�����ɉ�ʕ��ݒ�
	CSingleton<CDirect3D>::GetInstance()->GetScreenSize(&width, &height);
	new_camera->SetScreenScale(static_cast<float>(width), static_cast<float>(height));

	// �J������������
	this->AddCamera(new_camera);

	// �J�[�\���̍쐬
	this->mp_cursor = new CCursor;
	this->mp_cursor->Initilize();

	// �X�e�[�W�͖���
	this->mi_world_index = -1;
	this->mi_stage_index = -1;

	// �X�e�[�W�𕉂����񐔂��Ȃ�
	this->mi_fold_num = 0;
	this->mb_fold_switch = false;

	// �Z�[�u�f�[�^�ǂݍ���
	this->LoadSave();
}

CGameManager::~CGameManager()
{
	// �J�����̊J��
	for (size_t i = 0; i < this->mvp_cameras.size(); i++)
	{
		this->mvp_cameras[i]->Finalize();
		delete this->mvp_cameras[i];
	}
	this->mvp_cameras.clear();

	// �J�[�\���̊J��
	this->mp_cursor->UnInitilize();
	delete this->mp_cursor;
}

void CGameManager::Update()
{
	// �J�����̍X�V
	for (size_t i = 0; i < this->mvp_cameras.size(); i++)
	{
		this->mvp_cameras[i]->Update();
	}

	// �J�[�\���̍X�V
	this->mp_cursor->Update();
}

Transform CGameManager::GetTransformCamera()
{
	// �J�����̃g�����X�t�H�[�����擾
	return this->mvp_cameras[this->mui_current_camera]->GetTransform();
}

Transform CGameManager::GetTransformCamera(size_t index)
{
	// �w�肵���J�����̃g�����X�t�H�[�����擾
	if (index < this->mvp_cameras.size()) {
		return this->mvp_cameras[index]->GetTransform();
	}

	//�w�肵���J�������Ȃ��̂Ō��݂̃J�����̃g�����X�t�H�[�����擾
	return this->mvp_cameras[this->mui_current_camera]->GetTransform();
}

void CGameManager::SetTransformCamera(const Transform * transform)
{
	// �J�����̃g�����X�t�H�[�����ݒ�
	this->mvp_cameras[this->mui_current_camera]->SetTransform(transform);
}

void CGameManager::SetTransformCamera(const Transform * transform, size_t index)
{
	// �w�肵���J�����̃g�����X�t�H�[�����ݒ�
	if (index < this->mvp_cameras.size()) {
		this->mvp_cameras[index]->SetTransform(transform);
		return;
	}

	//�w�肵���J�������Ȃ��̂Ō��݂̃J�����̃g�����X�t�H�[�����ݒ�
	this->mvp_cameras[this->mui_current_camera]->SetTransform(transform);
}

size_t CGameManager::AddCamera(CCamera * new_camera)
{
	// �J�����̒ǉ�
	this->mvp_cameras.emplace_back(new_camera);
	return this->mvp_cameras.size() - 1;
}

bool CGameManager::ChangeCamera(size_t index)
{
	// �J�����ύX
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
	// �w�肵���J����������Ȃ�w�肵���J����
	if (index < this->mvp_cameras.size()) {
		return this->mvp_cameras[index]->Get2DMatrix();
	}
	// �Ȃ��Ȃ猻�݂̃J����
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
	// �V�����l��
	this->mi_fold_num = new_num;

	// �l�����߂�ꂽ�͈͂�
	if (this->mi_fold_num > FOLD_NUM_MAX) this->mi_fold_num = FOLD_NUM_MAX;
	if (this->mi_fold_num < 0) this->mi_fold_num = 0;
}

void CGameManager::AddFoldNum()
{
	// �l�𑝂₷
	this->mi_fold_num++;

	// �l�����߂�ꂽ�͈͂�
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
	// HP�͌��߂�ꂽ�͈͓��ɍ��킷
	if (new_hp > HP_MAX) new_hp = HP_MAX;
	if (new_hp < HP_MIN) new_hp = HP_MIN;

	// �V�����l
	this->mi_player_hp = new_hp;
}

void CGameManager::PullHP()
{
	// HP�����炷
	this->mi_player_hp--;

	// �ŏ��l��������Ă͂����Ȃ�
	if (this->mi_player_hp < HP_MIN) this->mi_player_hp = HP_MIN;
}

bool CGameManager::CheckGetPiece(int piece_num)
{
	// �ő�l���w�肳�ꂽ�Ȃ�S�Ċm�F
	if (piece_num == PIECE_NUM) {
		// ��ł�����Ȃ���� false
		for (size_t i = 0; i < this->mab_get_piece.size(); i++)
		{
			if (!this->mab_get_piece[i]) return false;
		}
		return true;
	}
	// ����̃J�P�����w�肵���ꍇ
	else if (0 <= piece_num && piece_num < PIECE_NUM) {
		return this->mab_get_piece[piece_num];
	}
	return false;
}

void CGameManager::SetGetPiece(int piece_num, bool is_get)
{
	// �ő�l���w�肳�ꂽ�Ȃ�S�đ��
	if (piece_num == PIECE_NUM) {
		for (size_t i = 0; i < this->mab_get_piece.size(); i++)
		{
			this->mab_get_piece[i] = is_get;
		}
	}
	// ����̃J�P�����w�肵���ꍇ
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
	// �܂�� 0
	this->SetFoldNum(0);

	// �܂�X�C�b�`��������
	this->SetFoldSwitch(false);

	// HP���ő�l��
	this->SetHP(HP_MAX);

	// �S�J�P���𖢎擾��
	this->SetGetPiece(PIECE_NUM, false);

	// �S�[�����Ă��Ȃ�
	this->SetGoal(false);
}

SAVE_DATA CGameManager::GetSave()
{
	return this->m_save_piece;
}

void CGameManager::SavePiece()
{
	// �J�P�����擾�����Ȃ�A�Z�[�u�f�[�^���X�V
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

	// �e�L�X�g�t�@�C���ɏ����o��
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

		// (���ʂ������o��)
		CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();
		ofs << audio->XA_GetCommonVolume() << std::endl;
	}
}

void CGameManager::LoadSave()
{
	// �J�P����S�Ė�������
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

	// �e�L�X�g�t�@�C������ǂ݂���
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
		// (���ʂ��ǂݍ���)
		CXAudio2* audio = CSingleton<CXAudio2>::GetInstance();
		float volume;
		ifs >> volume;
		audio->XA_SetCommonVolume(volume);
	}
}

void CGameManager::DeleteSave()
{
	// �J�P����S�Ė�������
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

	// �Z�[�u�p�e�L�X�g�t�@�C���������l�ɏ���������
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
