#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// �Q�[���Ŏg���ϐ���J�����̊Ǘ�

#include <vector>
#include <array>
#include "StageData.h"

struct Transform;
struct Matrix4_4;

class CCamera;
class CCursor;

// �Z�[�u�f�[�^�p�̕ϐ�
using SAVE_DATA = std::array<std::array<std::array<bool, PIECE_NUM>, GAME_STAGE_MAX>, GAME_WORLD_MAX>;

class CGameManager
{
public:
	CGameManager();
	~CGameManager();

	// �X�V����
	void Update();

	// �J�����̃g�����X�t�H�[�����擾
	Transform GetTransformCamera();
	Transform GetTransformCamera(size_t index);

	// �J�����̃g�����X�t�H�[�����ݒ�
	void SetTransformCamera(const Transform* transform);
	void SetTransformCamera(const Transform* transform, size_t index);

	// �J�����̒ǉ�
	// �߂�l�͐V�����C���f�b�N�X
	size_t AddCamera(CCamera* new_camera);

	// �J�����̕ύX
	bool ChangeCamera(size_t index);

	// 2D�J�����p�s��̎擾
	Matrix4_4 Get2DCameraMatrix();
	Matrix4_4 Get2DCameraMatrix(size_t index);

	// �J�[�\���̎擾
	CCursor* GetCursor();

	// ���[���h�̗v�f����Get�ESet
	int GetWorldIndex();
	void SetWorldIndex(int index);

	// �X�e�[�W�̗v�f����Get�ESet
	int GetStageIndex();
	void SetStageIndex(int index);

	// �܂����񐔊֌W
	int GetFoldNum();
	void SetFoldNum(int new_num);
	void AddFoldNum();

	// �܂������ǂ����֌W
	bool GetFoldSwitch();
	void SetFoldSwitch(bool new_switch);
	void ChangeFoldSwitch();

	// HP�֌W
	int GetHP();
	void SetHP(int new_hp);
	void PullHP();

	// �擾�����J�P���֌W
	bool CheckGetPiece(int piece_num);
	void SetGetPiece(int piece_num, bool is_get);

	// �S�[���֌W
	bool CheckGoal();
	void SetGoal(bool is_goal);

	// �X�e�[�W�Ŏg���f�[�^�̏�����
	void InitilizeStageData();

	// ���肵���J�P���֌W
	SAVE_DATA GetSave();
	void SavePiece();
	void LoadSave();
	void DeleteSave();

private:
	// ���݂̃J����
	size_t mui_current_camera;

	// �J��������
	std::vector<CCamera*> mvp_cameras;

	// �J�[�\��
	CCursor* mp_cursor;

	// ���[���h�̗v�f��
	int mi_world_index;

	// �X�e�[�W�̗v�f��
	int mi_stage_index;

	// �܂�����
	int mi_fold_num;

	// �Ԑu���b�N�p
	bool mb_fold_switch;

	// �v���C���[HP
	int mi_player_hp;

	// �X�e�[�W�ŏW�߂��J�P��
	std::array<bool, PIECE_NUM> mab_get_piece;

	// �W�߂��J�P���̃f�[�^
	SAVE_DATA m_save_piece;

	// �S�[������
	bool mb_is_goal;
};

