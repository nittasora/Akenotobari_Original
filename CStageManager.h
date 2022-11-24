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

	// ゲームオーバー
	FOLD_GAMEOVER_PREPARE,
	FOLD_GAMEOVER_ACT,
	FOLD_GAMEOVER_END,

	// ゲームクリア
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
	// オブジェクト関係
	//====================

	// ページ画像
	CPage* mp_page;

	// マップチップがあるかどうか
	std::array<std::array<std::array<int, MAPCHIP_NUM_WIDTH>, MAPCHIP_NUM_HEIGHT>, FRONT_BACK_MAX> maaai_chip_pos;

	// マップチップそのもの
	std::array<std::array<std::vector<CMapChip*>, MAPCHIP_NUM_HEIGHT>, FRONT_BACK_MAX> maavp_chip;

	// プレイヤー
	std::array<CPlayer*, FRONT_BACK_MAX> map_player;

	// 貼り付けられた札の番号
	std::array <std::vector<int>, FRONT_BACK_MAX> mavi_label_index;

	// 固定ギミック
	std::array <std::vector<CUnit*>, FRONT_BACK_MAX> mvp_fixed_gimmick;

	// 固定してないギミック
	std::array <std::vector<CUnit*>, FRONT_BACK_MAX> mvp_un_fixed_gimmick;

	// 使っている弾
	std::array < std::vector<CBall*>, FRONT_BACK_MAX> mvp_ball;

	//====================
	// データ関係
	//====================

	// 更新されるかどうか
	bool mb_is_active;

	// 表向き化裏向きか
	FRONT_BACK m_front_back;

	// ページの場所
	int mi_index_x;
	int mi_index_y;
};

class CStageManager
{
public:
	CStageManager();
	~CStageManager();

	// 更新処理
	void Update();
	void PauseUpdate();

	// 描画処理
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

	// 通常用
	void NormalUpdate();
	void NormalDraw();

	// 裏返し用
	void TurnUpdate();
	void TurnDraw();

	// ページ選択用
	void PageSelectUpdate();
	void PageSelectDraw();

	// ページ折り用
	void FoldUpdate();
	void FoldDraw();

	// ページ開き用
	void OpenUpadate();
	void OpenDraw();

	// ページ全て開き用
	void OpenAllUpadate();
	void OpenAllDraw();

	// 裏確認用
	void ViewUpdate();
	void ViewDraw();

	// ゲームクリア用
	void ClearUpdate();
	void ClearDraw();

	// ゲームオーバー用
	void OverUpdate();
	void OverDraw();

	// ページの描画
	void BasePageDraw();
	void FoldPageDraw();
	void FrontPageDraw();

	//====================
	// データ関係
	//====================

	struct
	{
		std::vector<IO_SpriteDataEX> mv_io_sprite;
		std::vector<SpriteData> mv_sprite;
		StageData m_stage;
	} m_data;

	//====================
	// オブジェクト関係
	//====================

	// 作成したオブジェクトの解放用
	std::vector<CGameObject*> mvp_object;

	struct
	{
		// ページ画像
		std::vector<CPage*> mvp_page;

		// マップチップ
		std::vector<CMapChip*> mvp_mapchip;

		// 赤青ブロック
		std::vector<CMapChip*> mvp_rb_block;

		// 裏面端のマップチップの疑似的な当たり判定用
		std::array<CMapChip*, MAPCHIP_NUM_HEIGHT> map_fake_left_chip;
		std::array<CMapChip*, MAPCHIP_NUM_HEIGHT> map_fake_right_chip;

		// 裏面端のギミックの疑似的な当たり判定用
		std::vector<CImage*> mvp_fake_left_gimmick;
		std::vector<CImage*> mvp_fake_right_gimmick;

		// プレイヤー
		CPlayer* mp_player;

		//
		std::array<CLabel*, LABEL_NUM_MAX> map_label;

		// ステージの限界点
		struct
		{
			// ステージ上下左右の境界
			std::array<CImage*, BO_MAX> map_border;

			// ステージ左端と右端
			CImage* mp_left_warp;
			CImage* mp_right_warp;
		} m_border;

		// 見えているオブジェクト
		struct
		{
			// マップチップ
			std::array<std::vector<CMapChip*>, MAPCHIP_NUM_HEIGHT> mav_mapchip;

			// プレイヤー
			CPlayer* mp_player;

			// 固定ギミック
			std::vector<CUnit*> mvp_fixed_gimmick;

			// 動くギミック
			std::vector<CUnit*> mvp_un_fixed_gimmick;

			// 使っている弾
			std::vector<CBall*> mvp_ball;

			// 使っていない弾
			std::vector<CBall*> mvp_un_ball;
		} m_active;

		// UI
		struct
		{
			// 裏返し用フェード
			CImage* mp_fade;

			// ページ選択用カーソル
			CImage* mp_cursor_frame;

			// 選択ページ表示用カーソル
			CImage* mp_fixed_cursor_frame;

			// HPのUI
			CImage* mp_hp;;

			// カケラ枠のUI
			CImage* mp_piece_frame;;

			// 選択可能ページ表示用
			std::array<CImage*, AREA_NUM + 2> map_check_image;

			// ずらし用
			std::array<CImage*, AREA_NUM + 2> map_limit_image;

			// 背景画像
			CImage* mp_back_image;
			CImage* mp_back_color_image;
			std::array<CImage*, BO_MAX> map_back_image;
			std::array<CImage*, BO_MAX> map_back_color_image;

			// クリア・オーバー画像
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

			// カケラの画像
			std::array<CImage*, PIECE_NUM> map_get_piece;
			std::array<CUnit*, PIECE_NUM> map_result_get_piece;
			std::array<CUnit*, PIECE_NUM> map_grand_get_piece;
		} m_ui;
	} m_object;


	//====================
	// 折り・開き関係
	//====================

	struct FOLD_DATA
	{
		// 基準になるページの要素
		std::vector<std::vector<int>> mvvi_base;

		// 折られるページの要素
		std::vector<std::vector<int>> mvvi_fold;

		// 手前のページの要素
		std::vector<std::vector<int>> mvvi_front;

		// ページの座標
		std::array<int, AREA_NUM> maf_page_x;
		std::array<int, AREA_NUM> maf_page_y;

		// 現在前後ろどちらを向いているか
		FRONT_BACK m_front_back;

		// 現在左折りか右折りか
		LEFT_RIGHT m_left_right;

		// x軸の軸
		float mf_axis_x;
	};

	struct
	{
		// 一つ開く用データ
		std::stack<FOLD_DATA> ms_old_data;

		// データ群
		FOLD_DATA m_data;

		// ページ情報
		std::array<PageNest, AREA_NUM> ma_page_nest;

		// ページの場所
		std::array<std::array<int, AREA_NUM>, AREA_NUM> maai_page_index;

		// 折れるかどうか
		std::array<bool, AREA_NUM + 2> mab_can_page;

		// 端の座標
		size_t mi_top_x;
		size_t mi_tail_x;
		size_t mi_top_y;
		size_t mi_tail_y;

		// 裏返した状態で開く。
		bool mb_reverse_open;

		// 札の数
		int mi_label_num;

		// ボタンを押した時間
		float mf_push_time;

		// 回転速度
		float mf_rotate_speed;

		// 暗転スピード
		float mf_fade_speed;

		// 全て開く時間
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

