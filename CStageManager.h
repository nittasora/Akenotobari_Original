#pragma once

#include <vector>
#include <array>
#include <stack>

#include "CGameManager.h"
#include "CXAudio2.h"

#include "StageData.h"
#include "CPage.h"
#include "CMapChip.h"
#include "CPlayer.h"
#include "CImage.h"
#include "CLabel.h"
#include "CBall.h"

#include "CSingleton.h"

enum BORDER_OBJECT
{
	BO_LEFT,
	BO_RIGHT,
	BO_TOP,
	BO_BOTTOM,
	BO_MAX
};

enum FOLD_CONDITION
{
	FOLD_NONE,
	FOLD_TURN_PREPARE,
	FOLD_TURN_ACT,
	FOLD_TURN_END,
	FOLD_TURN_VIEW_PREPARE,
	FOLD_TURN_VIEW_ACT,
	FOLD_TURN_VIEW_END,
	FOLD_FOLD_PREPARE,
	FOLD_FOLD_ACT,
	FOLD_FOLD_END,
	FOLD_OPEN_PREPARE,
	FOLD_OPEN_ACT,
	FOLD_OPEN_END,
	FOLD_OPEN_ALL_PREPARE,
	FOLD_OPEN_ALL_ACT,
	FOLD_OPEN_ALL_END,
	FOLD_PAGE_SELECT_PREPARE,
	FOLD_PAGE_SELECT_ACT,

	// �Q�[���I�[�o�[
	FOLD_GAMEOVER_PREPARE,
	FOLD_GAMEOVER_ACT,
	FOLD_GAMEOVER_END,

	// �Q�[���N���A
	FOLD_GAMECLEAR_PREPARE,
	FOLD_GAMECLEAR_ACT,
	FOLD_GAMECLEAR_END,
};

enum PLAYER_BORDER_OBJECT
{
	POB_LEFT,
	POB_RIGHT,
	POB_MAX
};

enum STAGE_MANAGER_BUTTON
{
	SMB_BUTTON_1,
	SMB_BUTTON_2,
	SMB_MAX
};

struct PageNest
{
	//====================
	// �I�u�W�F�N�g�֌W
	//====================

	// �y�[�W�摜
	CPage* mp_page;

	// �}�b�v�`�b�v�����邩�ǂ���
	std::array<std::array<std::array<int, MAPCHIP_NUM_WIDTH>, MAPCHIP_NUM_HEIGHT>, FRONT_BACK_MAX> maaai_chip_pos;

	// �}�b�v�`�b�v���̂���
	std::array<std::array<std::vector<CMapChip*>, MAPCHIP_NUM_HEIGHT>, FRONT_BACK_MAX> maavp_chip;

	// �v���C���[
	std::array<CPlayer*, FRONT_BACK_MAX> map_player;

	// �\��t����ꂽ�D�̔ԍ�
	std::array <std::vector<int>, FRONT_BACK_MAX> mavi_label_index;

	// �Œ�M�~�b�N
	std::array <std::vector<CUnit*>, FRONT_BACK_MAX> mvp_fixed_gimmick;

	// �Œ肵�ĂȂ��M�~�b�N
	std::array <std::vector<CUnit*>, FRONT_BACK_MAX> mvp_un_fixed_gimmick;

	// �g���Ă���e
	std::array < std::vector<CBall*>, FRONT_BACK_MAX> mvp_ball;

	//====================
	// �f�[�^�֌W
	//====================

	// �X�V����邩�ǂ���
	bool mb_is_active;

	// �\��������������
	FRONT_BACK m_front_back;

	// �y�[�W�̏ꏊ
	int mi_index_x;
	int mi_index_y;
};

class CStageManager
{
public:
	CStageManager();
	~CStageManager();

	// �X�V����
	void Update();
	void PauseUpdate();

	// �`�揈��
	void Draw();

	void CreateBall(Translate position, float max_speed, float angle_z, FRONT_BACK fb);

private:
	void TextureLoader();
	void StageLoader();
	void PageAndMapChip();
	void CreateGoal(std::vector<GoalData>* data, FRONT_BACK fb);
	void CreatePiece(std::vector<PieceData>* data, FRONT_BACK fb);
	void CreateTornade(std::vector<TornadeData>* data, FRONT_BACK fb);
	void CreateCannon(std::vector<CannonData>* data, FRONT_BACK fb);
	void CreateDharma(std::vector<DharmaData>* data, FRONT_BACK fb);
	void CreateLionTop(std::vector<LionData>* data, FRONT_BACK fb);
	void Player();
	void Initialized();
	void UpdateIndex();
	void SortIndex();
	void UpdatePageObject();
	void SortPageObject();
	void UpdateFakeGimmick(PageNest* page_nest, FRONT_BACK front_back, LEFT_RIGHT left_right);

	void MapChipTest(CGameObject* p_obj);
	void MapChipTest(std::vector<CGameObject*>* vp_obj);
	void FixedGimmickTest(CGameObject* p_obj);
	void FixedGimmickTest(std::vector<CGameObject*>* vp_obj);
	void UnFixedGimmickTest(CGameObject* p_obj);
	void UnFixedGimmickTest(std::vector<CGameObject*>* vp_obj);
	void BallTest(CGameObject* p_obj);
	void BallTest(std::vector<CGameObject*>* vp_obj);
	void FakeGimmickTest(CGameObject* p_obj);
	void FakeGimmickTest(std::vector<CGameObject*>* vp_obj);
	void WarpTest();
	void InitilizeFoldAxis(PageNest* target);
	void SetFoldAxis(PageNest* target);
	void SetFoldAxisOpen(PageNest* target);
	void SetFoldAxis(PageNest* target, bool reverse);
	void SetFrontAxis(PageNest* target);
	void SetFrontAxisOpen(PageNest* target);
	void SetFrontAxis(PageNest* target, bool reverse, bool is_open);

	// �ʏ�p
	void NormalUpdate();
	void NormalDraw();

	// ���Ԃ��p
	void TurnUpdate();
	void TurnDraw();

	// �y�[�W�I��p
	void PageSelectUpdate();
	void PageSelectDraw();

	// �y�[�W�܂�p
	void FoldUpdate();
	void FoldDraw();

	// �y�[�W�J���p
	void OpenUpadate();
	void OpenDraw();

	// �y�[�W�S�ĊJ���p
	void OpenAllUpadate();
	void OpenAllDraw();

	// ���m�F�p
	void ViewUpdate();
	void ViewDraw();

	// �Q�[���N���A�p
	void ClearUpdate();
	void ClearDraw();

	// �Q�[���I�[�o�[�p
	void OverUpdate();
	void OverDraw();

	// �y�[�W�̕`��
	void BasePageDraw();
	void FoldPageDraw();
	void FrontPageDraw();

	//====================
	// �f�[�^�֌W
	//====================

	struct
	{
		std::vector<IO_SpriteDataEX> mv_io_sprite;
		std::vector<SpriteData> mv_sprite;
		StageData m_stage;
	} m_data;

	//====================
	// �I�u�W�F�N�g�֌W
	//====================

	// �쐬�����I�u�W�F�N�g�̉���p
	std::vector<CGameObject*> mvp_object;

	struct
	{
		// �y�[�W�摜
		std::vector<CPage*> mvp_page;

		// �}�b�v�`�b�v
		std::vector<CMapChip*> mvp_mapchip;

		// �Ԑu���b�N
		std::vector<CMapChip*> mvp_rb_block;

		// ���ʒ[�̃}�b�v�`�b�v�̋^���I�ȓ����蔻��p
		std::array<CMapChip*, MAPCHIP_NUM_HEIGHT> map_fake_left_chip;
		std::array<CMapChip*, MAPCHIP_NUM_HEIGHT> map_fake_right_chip;

		// ���ʒ[�̃M�~�b�N�̋^���I�ȓ����蔻��p
		std::vector<CImage*> mvp_fake_left_gimmick;
		std::vector<CImage*> mvp_fake_right_gimmick;

		// �v���C���[
		CPlayer* mp_player;

		//
		std::array<CLabel*, LABEL_NUM_MAX> map_label;

		// �X�e�[�W�̌��E�_
		struct
		{
			// �X�e�[�W�㉺���E�̋��E
			std::array<CImage*, BO_MAX> map_border;

			// �X�e�[�W���[�ƉE�[
			CImage* mp_left_warp;
			CImage* mp_right_warp;
		} m_border;

		// �����Ă���I�u�W�F�N�g
		struct
		{
			// �}�b�v�`�b�v
			std::array<std::vector<CMapChip*>, MAPCHIP_NUM_HEIGHT> mav_mapchip;

			// �v���C���[
			CPlayer* mp_player;

			// �Œ�M�~�b�N
			std::vector<CUnit*> mvp_fixed_gimmick;

			// �����M�~�b�N
			std::vector<CUnit*> mvp_un_fixed_gimmick;

			// �g���Ă���e
			std::vector<CBall*> mvp_ball;

			// �g���Ă��Ȃ��e
			std::vector<CBall*> mvp_un_ball;
		} m_active;

		// UI
		struct
		{
			// ���Ԃ��p�t�F�[�h
			CImage* mp_fade;

			// �y�[�W�I��p�J�[�\��
			CImage* mp_cursor_frame;

			// �I���y�[�W�\���p�J�[�\��
			CImage* mp_fixed_cursor_frame;

			// HP��UI
			CImage* mp_hp;;

			// �J�P���g��UI
			CImage* mp_piece_frame;;

			// �I���\�y�[�W�\���p
			std::array<CImage*, AREA_NUM + 2> map_check_image;

			// ���炵�p
			std::array<CImage*, AREA_NUM + 2> map_limit_image;

			// �w�i�摜
			CImage* mp_back_image;
			CImage* mp_back_color_image;
			std::array<CImage*, BO_MAX> map_back_image;
			std::array<CImage*, BO_MAX> map_back_color_image;

			// �N���A�E�I�[�o�[�摜
			CUnit* mp_clear_frame;
			CUnit* mp_clear_book;
			std::vector<CUnit*> mvp_result_button;
			std::vector<CUnit*> mvp_result_frame;
			std::vector<CUnit*> mvp_result_cursor;

			CImage* mp_over_back;

			//CImage* mp_result_frame;
			//CImage* mp_result_piece_frame;
			//std::vector<CImage*> mvp_result_button;
			//std::vector<CImage*> mvp_result_frame;
			//std::vector<CImage*> mvp_result_cursor;

			// �J�P���̉摜
			std::array<CImage*, PIECE_NUM> map_get_piece;
			std::array<CUnit*, PIECE_NUM> map_result_get_piece;
			std::array<CUnit*, PIECE_NUM> map_grand_get_piece;
		} m_ui;
	} m_object;


	//====================
	// �܂�E�J���֌W
	//====================

	struct FOLD_DATA
	{
		// ��ɂȂ�y�[�W�̗v�f
		std::vector<std::vector<int>> mvvi_base;

		// �܂���y�[�W�̗v�f
		std::vector<std::vector<int>> mvvi_fold;

		// ��O�̃y�[�W�̗v�f
		std::vector<std::vector<int>> mvvi_front;

		// �y�[�W�̍��W
		std::array<int, AREA_NUM> maf_page_x;
		std::array<int, AREA_NUM> maf_page_y;

		// ���ݑO���ǂ���������Ă��邩
		FRONT_BACK m_front_back;

		// ���ݍ��܂肩�E�܂肩
		LEFT_RIGHT m_left_right;

		// x���̎�
		float mf_axis_x;
	};

	struct
	{
		// ��J���p�f�[�^
		std::stack<FOLD_DATA> ms_old_data;

		// �f�[�^�Q
		FOLD_DATA m_data;

		// �y�[�W���
		std::array<PageNest, AREA_NUM> ma_page_nest;

		// �y�[�W�̏ꏊ
		std::array<std::array<int, AREA_NUM>, AREA_NUM> maai_page_index;

		// �܂�邩�ǂ���
		std::array<bool, AREA_NUM + 2> mab_can_page;

		// �[�̍��W
		size_t mi_top_x;
		size_t mi_tail_x;
		size_t mi_top_y;
		size_t mi_tail_y;

		// ���Ԃ�����ԂŊJ���B
		bool mb_reverse_open;

		// �D�̐�
		int mi_label_num;

		// �{�^��������������
		float mf_push_time;

		// ��]���x
		float mf_rotate_speed;

		// �Ó]�X�s�[�h
		float mf_fade_speed;

		// �S�ĊJ������
		float mf_push_time_limit;
	} m_fold;


	//
	//
	//

	FOLD_CONDITION m_condition;
	FOLD_CONDITION m_next_condition;

	//SpriteData* mp_front_sprite;
	//SpriteData* mp_back_sprite;

	SpriteData m_stage_back_sprite;

	float mf_angle_y;

	int mi_cursor_pos;
	int mi_old_cursor_pos;
	int mi_cursor_line;
	int mi_old_cursor_line;
	float mf_old_axis;
	bool mb_initialized_flg;

	CGameManager* mp_game_manager;
	CXAudio2* mp_xaudio2;
};

