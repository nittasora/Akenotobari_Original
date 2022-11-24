#include "CStageManager.h"

#include "CMenuManager.h"
#include "CSceneManager.h"
#include "CInput.h"

#include "CDirect3D.h"
#include "CInput.h"
#include "CCursor.h"
#include "CTimer.h"
#include "CInputParameter.h"
#include "CMainWindow.h"

#include "CLionBottom.h"
#include "CRB_Block.h"
#include "CTornade.h"
#include "CGoal.h"
#include "CPiece.h"
#include "CCannon.h"
#include "CDharma.h"
#include "CLionTop.h"
#include "CBall.h"
#include "CUnit.h"

#include "CSingleton.h"

#include <fstream>
#include <iostream>
#include <string>

namespace
{
	// マップチップが存在しない場合
	constexpr int NONE_MAPCHIP = -1;

	// ページが存在しない場合
	constexpr int NONE_PAGE = -1;
}

CStageManager::CStageManager()
{
	// 一応
	this->m_object.mp_player = nullptr;

	this->m_condition = FOLD_NONE;
	this->m_next_condition = FOLD_NONE;
	this->m_fold.m_data.m_front_back = IS_FRONT;
	this->m_fold.m_data.m_left_right = IS_LEFT;
	this->m_fold.mi_label_num = 0;
	this->m_fold.mf_push_time = 0.0f;
	this->mb_initialized_flg = false;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	this->m_fold.mf_rotate_speed = cip_data.M_DATA.mf_stage_rotate_speed;
	this->m_fold.mf_fade_speed = cip_data.M_DATA.mf_stage_fade_speed;
	this->m_fold.mf_push_time_limit = cip_data.M_DATA.mf_stage_push_time_limit;

	this->mp_game_manager = CSingleton<CGameManager>::GetInstance();
	this->mp_game_manager->InitilizeStageData();

	this->mp_xaudio2 = CSingleton<CXAudio2>::GetInstance();

	this->StageLoader();
	this->Initialized();
}

CStageManager::~CStageManager()
{
	for (size_t i = 0; i < this->mvp_object.size(); i++)
	{
		this->mvp_object[i]->UnInitilize();
		delete this->mvp_object[i];
	}
	this->mvp_object.clear();

	for (size_t i = 0; i < this->m_data.mv_sprite.size(); i++)
	{
		COM_SAFE_RELEASE(this->m_data.mv_sprite[i].mSRV);
	}
}

void CStageManager::Update()
{
	if (!this->mb_initialized_flg) {
		this->mb_initialized_flg = true;

		int world = this->mp_game_manager->GetWorldIndex();
		SOUND_LABEL label = SOUND_LABEL_BGM_STAGE_1;
		switch (world)
		{
		case 2:
			label = SOUND_LABEL_BGM_STAGE_3;
			break;
		case 1:
			label = SOUND_LABEL_BGM_STAGE_2;
			break;
		}

		this->mp_xaudio2->Start(label);
		this->mp_xaudio2->XA_Setvolume(label, 0.0f);
		this->mp_xaudio2->XA_SetFadein(label, DEFAULT_MAX_VOLUME, DEFAULT_DIFF_VOLUME);
	}

	if (this->m_condition != this->m_next_condition) {
		this->m_condition = this->m_next_condition;
	}

	switch (this->m_condition)
	{
	case FOLD_NONE:
		this->NormalUpdate();
		break;
	case FOLD_TURN_PREPARE:
	case FOLD_TURN_ACT:
	case FOLD_TURN_END:
		this->TurnUpdate();
		break;
	case FOLD_TURN_VIEW_PREPARE:
	case FOLD_TURN_VIEW_ACT:
	case FOLD_TURN_VIEW_END:
		this->ViewUpdate();
		break;
	case FOLD_FOLD_PREPARE:
	case FOLD_FOLD_ACT:
	case FOLD_FOLD_END:
		this->FoldUpdate();
		break;
	case FOLD_OPEN_PREPARE:
	case FOLD_OPEN_ACT:
	case FOLD_OPEN_END:
		this->OpenUpadate();
		break;
	case FOLD_OPEN_ALL_PREPARE:
	case FOLD_OPEN_ALL_ACT:
	case FOLD_OPEN_ALL_END:
		this->OpenAllUpadate();
		break;
	case FOLD_PAGE_SELECT_PREPARE:
	case FOLD_PAGE_SELECT_ACT:
		this->PageSelectUpdate();
		break;
	case FOLD_GAMEOVER_PREPARE:
	case FOLD_GAMEOVER_ACT:
	case FOLD_GAMEOVER_END:
		this->OverUpdate();
		break;
	case FOLD_GAMECLEAR_PREPARE:
	case FOLD_GAMECLEAR_ACT:
	case FOLD_GAMECLEAR_END:
		this->ClearUpdate();
		break;
	default:
		break;
	}
}

void CStageManager::Draw()
{
	switch (this->m_condition)
	{
	case FOLD_NONE:
		this->NormalDraw();
		break;
	case FOLD_TURN_PREPARE:
	case FOLD_TURN_ACT:
	case FOLD_TURN_END:
		this->TurnDraw();
		break;
	case FOLD_TURN_VIEW_PREPARE:
	case FOLD_TURN_VIEW_ACT:
	case FOLD_TURN_VIEW_END:
		this->ViewDraw();
		break;
	case FOLD_FOLD_PREPARE:
	case FOLD_FOLD_ACT:
	case FOLD_FOLD_END:
		this->FoldDraw();
		break;
	case FOLD_OPEN_PREPARE:
	case FOLD_OPEN_ACT:
	case FOLD_OPEN_END:
		this->OpenDraw();
		break;
	case FOLD_OPEN_ALL_PREPARE:
	case FOLD_OPEN_ALL_ACT:
	case FOLD_OPEN_ALL_END:
		this->OpenAllDraw();
		break;
	case FOLD_PAGE_SELECT_PREPARE:
	case FOLD_PAGE_SELECT_ACT:
		this->PageSelectDraw();
		break;
	case FOLD_GAMEOVER_PREPARE:
	case FOLD_GAMEOVER_ACT:
	case FOLD_GAMEOVER_END:
		this->OverDraw();
		break;
	case FOLD_GAMECLEAR_PREPARE:
	case FOLD_GAMECLEAR_ACT:
	case FOLD_GAMECLEAR_END:
		this->ClearDraw();
		break;
	default:
		break;
	}
}

void CStageManager::TextureLoader()
{
	this->m_data.mv_io_sprite.clear();
	this->m_data.mv_sprite.clear();

	{
		std::ifstream ifs("assets/Game/Stage/StageData.dat");

		if (!ifs.fail()) {
			size_t size;
			int temp;

			ifs >> size;
			ifs >> temp;
			ifs >> temp;

			this->m_data.mv_io_sprite.resize(size);

			for (size_t i = 0; i < this->m_data.mv_io_sprite.size(); i++)
			{
				ifs >> this->m_data.mv_io_sprite[i].m_io_sprite_data.filename;
				ifs >> this->m_data.mv_io_sprite[i].m_io_sprite_data.anim_max;
				ifs >> this->m_data.mv_io_sprite[i].m_io_sprite_data.slice_x;
				ifs >> this->m_data.mv_io_sprite[i].m_io_sprite_data.slice_y;
				ifs >> this->m_data.mv_io_sprite[i].m_io_sprite_data.count_check;
				ifs >> this->m_data.mv_io_sprite[i].m_width;
				ifs >> this->m_data.mv_io_sprite[i].m_height;
			}

			for (size_t i = 0; i < this->m_data.mv_io_sprite.size(); i++)
			{
				SpriteData new_sprite;

				// CDirect3D取得
				CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

				HRESULT hr = direct3d->LoadTexture(this->m_data.mv_io_sprite[i].m_io_sprite_data.filename.c_str(), &new_sprite.mSRV);

				if (FAILED(hr)) {
					new_sprite.mSRV = nullptr;
					this->m_data.mv_sprite.emplace_back(new_sprite);
				}
				else {
					new_sprite.anim_max = this->m_data.mv_io_sprite[i].m_io_sprite_data.anim_max;
					new_sprite.slice_x = this->m_data.mv_io_sprite[i].m_io_sprite_data.slice_x;
					new_sprite.slice_y = this->m_data.mv_io_sprite[i].m_io_sprite_data.slice_y;

					new_sprite.slice.u = 1.0f / static_cast<float>(new_sprite.slice_x);
					new_sprite.slice.v = 1.0f / static_cast<float>(new_sprite.slice_y);

					new_sprite.count_check = this->m_data.mv_io_sprite[i].m_io_sprite_data.count_check;

					this->m_data.mv_sprite.emplace_back(new_sprite);
				}
			}
		}
	}
}

void CStageManager::StageLoader()
{
	this->TextureLoader();

	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();

	int world_index = game_manager->GetWorldIndex();
	int stage_index = game_manager->GetStageIndex();

	StageData stage_data;

	stage_data.mi_front_back = MAPCHIP_SPRITE_NONE;
	stage_data.mi_back_back = MAPCHIP_SPRITE_NONE;
	stage_data.m_player_pos_x = 0;
	stage_data.m_player_pos_y = 1;

	for (size_t x = 0; x < stage_data.mv_front_data.size(); x++)
	{
		for (size_t y = 0; y < stage_data.mv_front_data[x].size(); y++)
		{
			stage_data.mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
			stage_data.mv_front_data[x][y].mf_angle_z = 0.0f;
			stage_data.mv_front_data[x][y].m_tag = TAG_MAPCHIP;
		}
	}
	for (size_t x = 0; x < stage_data.mv_back_data.size(); x++)
	{
		for (size_t y = 0; y < stage_data.mv_back_data[x].size(); y++)
		{
			stage_data.mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
			stage_data.mv_back_data[x][y].mf_angle_z = 0.0f;
			stage_data.mv_back_data[x][y].m_tag = TAG_MAPCHIP;
		}
	}

	{
		std::string dir_name = "assets/Game/Stage/Stage_";
		std::string ex_name = ".dat";

		std::string file_name = dir_name + std::to_string(world_index) + "_" + std::to_string(stage_index) + ex_name;

		std::ifstream ifs(file_name);

		if (!ifs.fail()) {


			// マップチップ初期化
			for (size_t x = 0; x < stage_data.mv_front_data.size(); x++)
			{
				for (size_t y = 0; y < stage_data.mv_front_data[x].size(); y++)
				{
					int tag;
					ifs >> stage_data.mv_front_data[x][y].mi_chip_id;
					ifs >> stage_data.mv_front_data[x][y].mf_angle_z;
					ifs >> tag;
					stage_data.mv_front_data[x][y].m_tag = static_cast<ObjectTag>(tag);

					if (stage_data.mv_front_data[x][y].mi_chip_id > MAPCHIP_SPRITE_NONE && stage_data.mv_front_data[x][y].mi_chip_id < static_cast<int>(this->m_data.mv_sprite.size())) {
						if (this->m_data.mv_sprite[stage_data.mv_front_data[x][y].mi_chip_id].mSRV == nullptr) stage_data.mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
					}
				}
			}
			// マップチップ初期化
			for (size_t x = 0; x < stage_data.mv_back_data.size(); x++)
			{
				for (size_t y = 0; y < stage_data.mv_back_data[x].size(); y++)
				{
					int tag;
					ifs >> stage_data.mv_back_data[x][y].mi_chip_id;
					ifs >> stage_data.mv_back_data[x][y].mf_angle_z;
					ifs >> tag;
					stage_data.mv_back_data[x][y].m_tag = static_cast<ObjectTag>(tag);

					if (stage_data.mv_back_data[x][y].mi_chip_id > MAPCHIP_SPRITE_NONE && stage_data.mv_back_data[x][y].mi_chip_id < static_cast<int>(this->m_data.mv_sprite.size())) {
						if (this->m_data.mv_sprite[stage_data.mv_back_data[x][y].mi_chip_id].mSRV == nullptr) stage_data.mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
					}
				}
			}

			int trash;
			ifs >> stage_data.mi_front_back;
			ifs >> stage_data.mi_back_back;
			ifs >> stage_data.m_player_pos_x;
			ifs >> stage_data.m_player_pos_y;
			ifs >> trash;
			ifs >> trash;

			if (stage_data.mi_front_back > MAPCHIP_SPRITE_NONE && stage_data.mi_front_back < static_cast<int>(this->m_data.mv_sprite.size())) {
				if (this->m_data.mv_sprite[stage_data.mi_front_back].mSRV == nullptr) stage_data.mi_front_back = MAPCHIP_SPRITE_NONE;
			}
			if (stage_data.mi_back_back > MAPCHIP_SPRITE_NONE && stage_data.mi_back_back < static_cast<int>(this->m_data.mv_sprite.size())) {
				if (this->m_data.mv_sprite[stage_data.mi_back_back].mSRV == nullptr) stage_data.mi_back_back = MAPCHIP_SPRITE_NONE;
			}

			int size;
			ifs >> size;
			if (size != END_OF_STAGE_FILE) {
				for (size_t i = 0; i < size; i++)
				{
					GoalData data;
					ifs >> data.position.x;
					ifs >> data.position.y;
					ifs >> data.position.z;
					stage_data.mv_front_goal_obj.emplace_back(data);
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						GoalData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						stage_data.mv_back_goal_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						PieceData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.i_piece_num;
						stage_data.mv_front_piece_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						PieceData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.i_piece_num;
						stage_data.mv_back_piece_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						TornadeData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						stage_data.mv_front_tornade_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						TornadeData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						stage_data.mv_back_tornade_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						DharmaData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_max_speed;
						ifs >> data.mb_is_left;
						stage_data.mv_front_dharma_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						DharmaData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_max_speed;
						ifs >> data.mb_is_left;
						stage_data.mv_back_dharma_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						CannonData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_angle_z;
						ifs >> data.mf_max_speed;
						ifs >> data.mf_wait;
						stage_data.mv_front_cannon_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						CannonData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_angle_z;
						ifs >> data.mf_max_speed;
						ifs >> data.mf_wait;
						stage_data.mv_back_cannon_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						LionData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_up_max_speed;
						ifs >> data.mf_down_max_speed;
						ifs >> data.mf_up_wait;
						ifs >> data.mf_down_wait;
						stage_data.mv_front_lion_obj.emplace_back(data);
					}
				}
			}

			if (size != END_OF_STAGE_FILE) {
				ifs >> size;
				if (size != END_OF_STAGE_FILE) {
					for (size_t i = 0; i < size; i++)
					{
						LionData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_up_max_speed;
						ifs >> data.mf_down_max_speed;
						ifs >> data.mf_up_wait;
						ifs >> data.mf_down_wait;
						stage_data.mv_back_lion_obj.emplace_back(data);
					}
				}
			}
		}
	}

	for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
	{
		this->m_fold.ma_page_nest[i].mp_page = nullptr;

		for (int j = 0; j < this->m_fold.ma_page_nest[i].maavp_chip.size(); j++)
		{
			for (int k = 0; k < this->m_fold.ma_page_nest[i].maavp_chip[j].size(); k++)
			{
				this->m_fold.ma_page_nest[i].maavp_chip[j][k].clear();
			}
		}

		for (size_t j = 0; j < this->m_fold.ma_page_nest[i].map_player.size(); j++)
		{
			this->m_fold.ma_page_nest[i].map_player[j] = nullptr;
		}

		for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick.size(); j++)
		{
			this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j].clear();
		}

		for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick.size(); j++)
		{
			this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j].clear();
		}

		for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_ball.size(); j++)
		{
			this->m_fold.ma_page_nest[i].mvp_ball[j].clear();
		}

		this->m_fold.ma_page_nest[i].m_front_back = IS_FRONT;
		this->m_fold.ma_page_nest[i].mi_index_x = static_cast<int>(i);
		this->m_fold.ma_page_nest[i].mi_index_y = 0;
	}

	this->m_data.m_stage = stage_data;

	this->PageAndMapChip();
}

void CStageManager::PageAndMapChip()
{
	//this->mp_front_sprite = nullptr;
	//this->mp_back_sprite = nullptr;

	// ページ作成
	for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++) {
		CPage* new_image = new CPage;
		new_image->Initilize();

		if (this->m_data.m_stage.mi_front_back != MAPCHIP_SPRITE_NONE && this->m_data.m_stage.mi_back_back != MAPCHIP_SPRITE_NONE) {
			new_image->SetSprite(&this->m_data.mv_sprite[this->m_data.m_stage.mi_front_back], &this->m_data.mv_sprite[this->m_data.m_stage.mi_back_back]);
			//this->mp_front_sprite = &this->m_data.mv_sprite[this->m_data.m_stage.mi_front_back];
			//this->mp_back_sprite = &this->m_data.mv_sprite[this->m_data.m_stage.mi_back_back];
		}
		else {
			new_image->SetSprite(SPRITE_TEST_2, SPRITE_TEST_1);
		}

		new_image->SetDefaultUV(static_cast<int>(i));

		Transform tr = new_image->GetTransform();
		tr.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
		new_image->SetTransform(&tr);

		this->mvp_object.emplace_back(new_image);
		this->m_object.mvp_page.emplace_back(new_image);
		this->m_fold.ma_page_nest[i].mp_page = new_image;
	}

	// マップチップ表作成
	for (size_t x = 0; x < this->m_data.m_stage.mv_front_data.size(); x++)
	{
		for (size_t y = 0; y < this->m_data.m_stage.mv_front_data[x].size(); y++)
		{
			if (this->m_data.m_stage.mv_front_data[x][y].mi_chip_id != MAPCHIP_SPRITE_NONE) {
				CMapChip* new_image;

				switch (this->m_data.m_stage.mv_front_data[x][y].m_tag)
				{
				case TAG_RED_BLOCK:
				{
					CRB_Block* new_rb_block = new CRB_Block;
					new_rb_block->Initilize();
					new_rb_block->SetIsBlue(false);
					new_image = new_rb_block;

					this->m_object.mvp_rb_block.emplace_back(new_image);
				}
				break;
				case TAG_BLUE_BLOCK:
				{
					CRB_Block* new_rb_block = new CRB_Block;
					new_rb_block->Initilize();
					new_rb_block->SetIsBlue(true);
					new_image = new_rb_block;

					this->m_object.mvp_rb_block.emplace_back(new_image);
				}
				break;
				case TAG_LION_BOTTOM:
				{
					CLionBottom* new_lion = new CLionBottom;
					new_lion->Initilize();

					switch (this->m_data.m_stage.mv_front_data[x][y].mi_chip_id)
					{
					case LEFT_LION_BOTTOM:
						new_lion->SetDefaultUV(0.0f, 1.0f / 3.0f, 1.0f, 0.0f);
						break;
					case CENTER_LION_BOTTOM:
						new_lion->SetDefaultUV(1.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f);
						break;
					case RIGHT_LION_BOTTOM:
						new_lion->SetDefaultUV(2.0f / 3.0f, 1.0f, 1.0f, 0.0f);
						break;
					default:
						new_lion->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);
						break;
					}

					new_image = new_lion;
				}
				break;
				case TAG_MAPCHIP:
					new_image = new CMapChip;
					new_image->Initilize();
					new_image->SetTexture(&this->m_data.mv_sprite[this->m_data.m_stage.mv_front_data[x][y].mi_chip_id]);
					break;
				default:
					new_image = new CMapChip;
					new_image->Initilize();
					break;
				}

				Transform tr = new_image->GetTransform();
				tr.position.x = MAPCHIP_WIDTH * (-(static_cast<float>(MAPCHIP_NUM_WIDTH) * static_cast<float>(AREA_NUM)) / 2.0f + 0.5f + static_cast<float>(x));
				tr.position.y = MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f + 0.5f + static_cast<float>(y));
				tr.angle.z = this->m_data.m_stage.mv_front_data[x][y].mf_angle_z;
				new_image->SetTransform(&tr);

				this->mvp_object.emplace_back(new_image);
				this->m_object.mvp_mapchip.emplace_back(new_image);
				this->m_fold.ma_page_nest[x / MAPCHIP_NUM_WIDTH].maavp_chip[IS_FRONT][y].emplace_back(new_image);

				this->m_fold.ma_page_nest[x / MAPCHIP_NUM_WIDTH].maaai_chip_pos[IS_FRONT][y][x % MAPCHIP_NUM_WIDTH] = static_cast<int>(this->m_object.mvp_mapchip.size()) - 1;
			}
			else {
				this->m_fold.ma_page_nest[x / MAPCHIP_NUM_WIDTH].maaai_chip_pos[IS_FRONT][y][x % MAPCHIP_NUM_WIDTH] = MAPCHIP_SPRITE_NONE;
			}
		}
	}

	// マップチップ裏作成
	for (size_t x = 0; x < this->m_data.m_stage.mv_back_data.size(); x++)
	{
		for (size_t y = 0; y < this->m_data.m_stage.mv_back_data[x].size(); y++)
		{
			if (this->m_data.m_stage.mv_back_data[x][y].mi_chip_id != MAPCHIP_SPRITE_NONE) {
				CMapChip* new_image;

				switch (this->m_data.m_stage.mv_back_data[x][y].m_tag)
				{
				case TAG_RED_BLOCK:
				{
					CRB_Block* new_rb_block = new CRB_Block;
					new_rb_block->Initilize();
					new_rb_block->SetIsBlue(false);
					new_image = new_rb_block;

					this->m_object.mvp_rb_block.emplace_back(new_image);
				}
				break;
				case TAG_BLUE_BLOCK:
				{
					CRB_Block* new_rb_block = new CRB_Block;
					new_rb_block->Initilize();
					new_rb_block->SetIsBlue(true);
					new_image = new_rb_block;

					this->m_object.mvp_rb_block.emplace_back(new_image);
				}
				break;
				case TAG_LION_BOTTOM:
				{
					CLionBottom* new_lion = new CLionBottom;
					new_lion->Initilize();

					switch (this->m_data.m_stage.mv_front_data[x][y].mi_chip_id)
					{
					case LEFT_LION_BOTTOM:
						new_lion->SetDefaultUV(0.0f, 1.0f / 3.0f, 1.0f, 0.0f);
						break;
					case CENTER_LION_BOTTOM:
						new_lion->SetDefaultUV(1.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f);
						break;
					case RIGHT_LION_BOTTOM:
						new_lion->SetDefaultUV(2.0f / 3.0f, 1.0f, 1.0f, 0.0f);
						break;
					default:
						new_lion->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);
						break;
					}

					new_image = new_lion;
				}
				break;
				case TAG_MAPCHIP:
					new_image = new CMapChip;
					new_image->Initilize();
					new_image->SetTexture(&this->m_data.mv_sprite[this->m_data.m_stage.mv_back_data[x][y].mi_chip_id]);
					break;
				default:
					new_image = new CMapChip;
					new_image->Initilize();
					break;
				}

				Transform tr = new_image->GetTransform();
				tr.position.x = MAPCHIP_WIDTH * ((static_cast<float>(MAPCHIP_NUM_WIDTH) * static_cast<float>(AREA_NUM)) / 2.0f - 0.5f - static_cast<float>(x));
				tr.position.y = MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f + 0.5f + static_cast<float>(y));
				tr.angle.y = 180.0f;
				tr.angle.z = this->m_data.m_stage.mv_back_data[x][y].mf_angle_z;
				new_image->SetTransform(&tr);

				this->mvp_object.emplace_back(new_image);
				this->m_object.mvp_mapchip.emplace_back(new_image);
				this->m_fold.ma_page_nest[AREA_NUM - x / MAPCHIP_NUM_WIDTH - 1].maavp_chip[IS_BACK][y].emplace_back(new_image);

				this->m_fold.ma_page_nest[AREA_NUM - x / MAPCHIP_NUM_WIDTH - 1].maaai_chip_pos[IS_BACK][y][x % MAPCHIP_NUM_WIDTH] = static_cast<int>(this->m_object.mvp_mapchip.size()) - 1;
			}
			else {
				this->m_fold.ma_page_nest[AREA_NUM - x / MAPCHIP_NUM_WIDTH - 1].maaai_chip_pos[IS_BACK][y][x % MAPCHIP_NUM_WIDTH] = MAPCHIP_SPRITE_NONE;
			}
		}
	}

	this->CreateGoal(&this->m_data.m_stage.mv_front_goal_obj, IS_FRONT);
	this->CreateGoal(&this->m_data.m_stage.mv_back_goal_obj, IS_BACK);

	this->CreatePiece(&this->m_data.m_stage.mv_front_piece_obj, IS_FRONT);
	this->CreatePiece(&this->m_data.m_stage.mv_back_piece_obj, IS_BACK);

	this->CreateTornade(&this->m_data.m_stage.mv_front_tornade_obj, IS_FRONT);
	this->CreateTornade(&this->m_data.m_stage.mv_back_tornade_obj, IS_BACK);

	this->CreateCannon(&this->m_data.m_stage.mv_front_cannon_obj, IS_FRONT);
	this->CreateCannon(&this->m_data.m_stage.mv_back_cannon_obj, IS_BACK);

	this->CreateLionTop(&this->m_data.m_stage.mv_front_lion_obj, IS_FRONT);
	this->CreateLionTop(&this->m_data.m_stage.mv_back_lion_obj, IS_BACK);

	this->CreateDharma(&this->m_data.m_stage.mv_front_dharma_obj, IS_FRONT);
	this->CreateDharma(&this->m_data.m_stage.mv_back_dharma_obj, IS_BACK);

	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	// 境界オブジェクトの作成
	for (size_t i = 0; i < this->m_object.m_border.map_border.size(); i++)
	{
		this->m_object.m_border.map_border[i] = new CImage;
		this->m_object.m_border.map_border[i]->Initilize();

		this->m_object.m_border.map_border[i]->AddTexture(SPRITE_TEST_0);

		this->mvp_object.emplace_back(this->m_object.m_border.map_border[i]);
	}

	// 左境界設定
	{
		ObjectTag tag = TAG_BORDER_LEFT;
		this->m_object.m_border.map_border[BO_LEFT]->SetTag(&tag);

		this->m_object.m_border.map_border[BO_LEFT]->SetDefaultPosAndColl(MAPCHIP_WIDTH * MAPCHIP_NUM_WIDTH, screen_height);
	}

	// 右境界設定
	{
		ObjectTag tag = TAG_BORDER_RIGHT;
		this->m_object.m_border.map_border[BO_RIGHT]->SetTag(&tag);

		this->m_object.m_border.map_border[BO_RIGHT]->SetDefaultPosAndColl(MAPCHIP_WIDTH * MAPCHIP_NUM_WIDTH, screen_height);
	}

	// 下境界設定
	{
		ObjectTag tag = TAG_BORDER_BOTTOM;
		this->m_object.m_border.map_border[BO_BOTTOM]->SetTag(&tag);

		this->m_object.m_border.map_border[BO_BOTTOM]->SetDefaultPosAndColl(screen_width, (screen_height - (MAPCHIP_HEIGHT * MAPCHIP_NUM_HEIGHT)) / 2.0f);

		Transform transform = this->m_object.m_border.map_border[BO_BOTTOM]->GetTransform();
		transform.position.y = (-screen_height + ((screen_height - (MAPCHIP_HEIGHT * MAPCHIP_NUM_HEIGHT)) / 2.0f)) / 2.0f;
		this->m_object.m_border.map_border[BO_BOTTOM]->SetTransform(&transform);
	}

	// 上境界設定
	{
		ObjectTag tag = TAG_BORDER_TOP;
		this->m_object.m_border.map_border[BO_TOP]->SetTag(&tag);

		this->m_object.m_border.map_border[BO_TOP]->SetDefaultPosAndColl(screen_width, (screen_height - (MAPCHIP_HEIGHT * MAPCHIP_NUM_HEIGHT)) / 2.0f);

		Transform transform = this->m_object.m_border.map_border[BO_TOP]->GetTransform();
		transform.position.y = (screen_height - ((screen_height - (MAPCHIP_HEIGHT * MAPCHIP_NUM_HEIGHT)) / 2.0f)) / 2.0f;
		this->m_object.m_border.map_border[BO_TOP]->SetTransform(&transform);
	}

	// 左ワープポイント作成
	{
		this->m_object.m_border.mp_left_warp = new CImage;
		this->m_object.m_border.mp_left_warp->Initilize();

		this->m_object.m_border.mp_left_warp->AddTexture(SPRITE_TEST_1);

		ObjectTag tag = TAG_MAPCHIP;
		m_object.m_border.mp_left_warp->SetTag(&tag);

		this->m_object.m_border.mp_left_warp->SetDefaultPosAndColl(MAPCHIP_WIDTH * MAPCHIP_NUM_WIDTH, screen_height);

		this->mvp_object.emplace_back(this->m_object.m_border.mp_left_warp);
	}

	// 右ワープポイント作成
	{
		this->m_object.m_border.mp_right_warp = new CImage;
		this->m_object.m_border.mp_right_warp->Initilize();

		this->m_object.m_border.mp_right_warp->AddTexture(SPRITE_TEST_1);

		ObjectTag tag = TAG_MAPCHIP;
		m_object.m_border.mp_right_warp->SetTag(&tag);

		this->m_object.m_border.mp_right_warp->SetDefaultPosAndColl(MAPCHIP_WIDTH * MAPCHIP_NUM_WIDTH, screen_height);

		this->mvp_object.emplace_back(this->m_object.m_border.mp_right_warp);
	}

	// 背景
	{
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_TEST_2);

		new_image->SetDefaultPosAndColl(screen_width, screen_height);

		this->m_object.m_ui.mp_back_color_image = new_image;
		this->mvp_object.emplace_back(new_image);
	}
	{
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_TEST_1);

		Color color = { 1.0f, 1.0f, 1.0f, 0.75f };
		new_image->SetColor(&color);

		new_image->SetDefaultPosAndColl(screen_width, screen_height);

		this->m_object.m_ui.mp_back_image = new_image;
		this->mvp_object.emplace_back(new_image);
	}

	// 分割背景
	for (size_t i = 0; i < this->m_object.m_ui.map_back_color_image.size(); i++)
	{
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_TEST_2);

		this->m_object.m_ui.map_back_color_image[i] = new_image;
		this->mvp_object.emplace_back(new_image);
	}
	{
		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		this->m_object.m_ui.map_back_color_image[BO_TOP]->SetImage(-(screen_width / 2.0f), (screen_width / 2.0f), (MAPCHIP_HEIGHT * mapchip_num), (screen_height / 2.0f));
		float new_v = ((MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v > 1.0f) new_v = 1.0f;
		if (new_v < 0.0f) new_v = 0.0f;
		new_v = 1.0f - new_v;
		this->m_object.m_ui.map_back_color_image[BO_TOP]->SetDefaultUV(0.0f, 1.0f, new_v, 0.0f);
	}
	{
		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		this->m_object.m_ui.map_back_color_image[BO_BOTTOM]->SetImage(-(screen_width / 2.0f), (screen_width / 2.0f), -(screen_height / 2.0f), -(MAPCHIP_HEIGHT * mapchip_num));
		float new_v = (-(MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v > 1.0f) new_v = 1.0f;
		if (new_v < 0.0f) new_v = 0.0f;
		new_v = 1.0f - new_v;
		this->m_object.m_ui.map_back_color_image[BO_BOTTOM]->SetDefaultUV(0.0f, 1.0f, 1.0f, new_v);
	}

	for (size_t i = 0; i < this->m_object.m_ui.map_back_image.size(); i++)
	{
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_TEST_1);

		Color color = { 1.0f, 1.0f, 1.0f, 0.75f };
		new_image->SetColor(&color);

		this->m_object.m_ui.map_back_image[i] = new_image;
		this->mvp_object.emplace_back(new_image);
	}
	{
		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		this->m_object.m_ui.map_back_image[BO_TOP]->SetImage(-(screen_width / 2.0f), (screen_width / 2.0f), (MAPCHIP_HEIGHT * mapchip_num), (screen_height / 2.0f));
		float new_v = ((MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v > 1.0f) new_v = 1.0f;
		if (new_v < 0.0f) new_v = 0.0f;
		new_v = 1.0f - new_v;
		this->m_object.m_ui.map_back_image[BO_TOP]->SetDefaultUV(0.0f, 1.0f, new_v, 0.0f);
	}
	{
		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		this->m_object.m_ui.map_back_image[BO_BOTTOM]->SetImage(-(screen_width / 2.0f), (screen_width / 2.0f), -(screen_height / 2.0f), -(MAPCHIP_HEIGHT * mapchip_num));
		float new_v = (-(MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v > 1.0f) new_v = 1.0f;
		if (new_v < 0.0f) new_v = 0.0f;
		new_v = 1.0f - new_v;
		this->m_object.m_ui.map_back_image[BO_BOTTOM]->SetDefaultUV(0.0f, 1.0f, 1.0f, new_v);
	}

	// 偽物のマップチップ生成
	for (size_t i = 0; i < this->m_object.map_fake_left_chip.size(); i++)
	{
		this->m_object.map_fake_left_chip[i] = new CMapChip;
		this->m_object.map_fake_left_chip[i]->Initilize();

		Transform tr = this->m_object.map_fake_left_chip[i]->GetTransform();
		tr.position.y = MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f + 0.5f + static_cast<float>(i % 10));
		this->m_object.map_fake_left_chip[i]->SetTransform(&tr);

		this->mvp_object.emplace_back(this->m_object.map_fake_left_chip[i]);
	}
	for (size_t i = 0; i < this->m_object.map_fake_right_chip.size(); i++)
	{
		this->m_object.map_fake_right_chip[i] = new CMapChip;
		this->m_object.map_fake_right_chip[i]->Initilize();

		Transform tr = this->m_object.map_fake_right_chip[i]->GetTransform();
		tr.position.y = MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f + 0.5f + static_cast<float>(i % 10));
		this->m_object.map_fake_right_chip[i]->SetTransform(&tr);

		this->mvp_object.emplace_back(this->m_object.map_fake_right_chip[i]);
	}

	// フェード用イメージ作成
	{
		this->m_object.m_ui.mp_fade = new CImage;
		this->m_object.m_ui.mp_fade->Initilize();

		this->m_object.m_ui.mp_fade->AddTexture(SPRITE_TEST_2);

		Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
		m_object.m_ui.mp_fade->SetColor(&color);

		this->m_object.m_ui.mp_fade->SetDefaultPosAndColl(screen_width, screen_height);

		this->mvp_object.emplace_back(this->m_object.m_ui.mp_fade);
	}

	for (size_t i = 0; i < this->m_object.m_ui.map_check_image.size(); i++)
	{

		this->m_object.m_ui.map_check_image[i] = new CImage;
		this->m_object.m_ui.map_check_image[i]->Initilize();

		this->m_object.m_ui.map_check_image[i]->AddTexture(SPRITE_TEST_2);

		ObjectTag tag = TAG_BORDER_PAGE;
		m_object.m_ui.map_check_image[i]->SetTag(&tag);

		Color color = { 0.0f, 0.0f, 0.0f, 0.5f };
		m_object.m_ui.map_check_image[i]->SetColor(&color);

		this->m_object.m_ui.map_check_image[i]->SetDefaultPosAndColl(MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH), MAPCHIP_HEIGHT * static_cast<float>(MAPCHIP_NUM_HEIGHT));

		Transform transform = this->m_object.m_ui.map_check_image[i]->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-4.0f + static_cast<float>(i));
		this->m_object.m_ui.map_check_image[i]->SetTransform(&transform);

		this->mvp_object.emplace_back(this->m_object.m_ui.map_check_image[i]);
	}

	for (size_t i = 0; i < this->m_object.m_ui.map_limit_image.size(); i++)
	{

		this->m_object.m_ui.map_limit_image[i] = new CImage;
		this->m_object.m_ui.map_limit_image[i]->Initilize();

		this->m_object.m_ui.map_limit_image[i]->AddTexture(SPRITE_TEST_2);

		ObjectTag tag = TAG_BORDER_PAGE;
		m_object.m_ui.map_limit_image[i]->SetTag(&tag);

		Color color = { 0.0f, 0.0f, 0.0f, 0.5f };
		m_object.m_ui.map_limit_image[i]->SetColor(&color);

		this->m_object.m_ui.map_limit_image[i]->SetDefaultPosAndColl(MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH), MAPCHIP_HEIGHT * static_cast<float>(MAPCHIP_NUM_HEIGHT) * 3.0f);

		Transform transform = this->m_object.m_ui.map_limit_image[i]->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-4.0f + static_cast<float>(i));
		this->m_object.m_ui.map_limit_image[i]->SetTransform(&transform);

		this->mvp_object.emplace_back(this->m_object.m_ui.map_limit_image[i]);
	}

	{
		this->m_object.m_ui.mp_cursor_frame = new CImage;
		this->m_object.m_ui.mp_cursor_frame->Initilize();

		this->m_object.m_ui.mp_cursor_frame->AddTexture(SPRITE_FRAME);

		Color color = { 1.0f, 0.0f, 0.0f, 0.75f };
		m_object.m_ui.mp_cursor_frame->SetColor(&color);

		this->m_object.m_ui.mp_cursor_frame->SetDefaultPosAndColl(196.0f, 490.0f);

		this->mvp_object.emplace_back(this->m_object.m_ui.mp_cursor_frame);
	}

	{
		this->m_object.m_ui.mp_fixed_cursor_frame = new CImage;
		this->m_object.m_ui.mp_fixed_cursor_frame->Initilize();

		this->m_object.m_ui.mp_fixed_cursor_frame->AddTexture(SPRITE_FRAME);

		Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
		m_object.m_ui.mp_fixed_cursor_frame->SetColor(&color);

		this->m_object.m_ui.mp_fixed_cursor_frame->SetDefaultPosAndColl(196.0f, 490.0f);

		this->mvp_object.emplace_back(this->m_object.m_ui.mp_fixed_cursor_frame);
	}

	{
		this->m_object.m_ui.mp_hp = new CImage;
		this->m_object.m_ui.mp_hp->Initilize();

		this->m_object.m_ui.mp_hp->AddTexture(SPRITE_UI_HP);

		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_object.m_ui.mp_hp->SetColor(&color);

		Transform transform = this->m_object.m_ui.mp_hp->GetTransform();
		transform.position.x = -screen_width / 2.0f + 237.5f;
		transform.position.y = screen_height / 2.0f - 75.0f;
		this->m_object.m_ui.mp_hp->SetTransform(&transform);

		this->m_object.m_ui.mp_hp->SetDefaultPosAndColl(475.0f, 150.0f);

		this->mvp_object.emplace_back(this->m_object.m_ui.mp_hp);
	}

	{
		this->m_object.m_ui.mp_piece_frame = new CImage;
		this->m_object.m_ui.mp_piece_frame->Initilize();

		this->m_object.m_ui.mp_piece_frame->AddTexture(SPRITE_UI_FRAME);

		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_object.m_ui.mp_piece_frame->SetColor(&color);

		Transform transform = this->m_object.m_ui.mp_piece_frame->GetTransform();
		transform.position.x = screen_width / 2.0f - 185.0f;
		transform.position.y = screen_height / 2.0f - 76.0f;
		this->m_object.m_ui.mp_piece_frame->SetTransform(&transform);

		this->m_object.m_ui.mp_piece_frame->SetDefaultPosAndColl(370.0f, 148.0f);

		this->mvp_object.emplace_back(this->m_object.m_ui.mp_piece_frame);
	}

	{
		CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();

		float world_inndex = static_cast<float>(game_manager->GetWorldIndex());
		float stage_inndex = static_cast<float>(game_manager->GetStageIndex());
		for (size_t i = 0; i < this->m_object.m_ui.map_get_piece.size(); i++)
		{
			CImage* new_image = new CImage;
			new_image->Initilize();

			new_image->AddTexture(SPRITE_PIECE_1_in);

			Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
			new_image->SetColor(&color);

			Transform transform = new_image->GetTransform();
			transform.position.x = screen_width / 2.0f - 185.0f;
			transform.position.y = screen_height / 2.0f - 76.0f;
			switch (i)
			{
			case 0:
				transform.position.x -= 56.0f;
				transform.position.y -= 8.0f;
				transform.angle.z = -15.0f;
				break;
			case 1:
				transform.position.x -= 4.0f;
				transform.position.y += 26.0f;
				transform.angle.z = 40.0f;
				break;
			case 2:
				transform.position.x += 68.0f;
				transform.position.y -= 8.0f;
				transform.angle.z = 15.0f;
				break;
			}
			new_image->SetTransform(&transform);

			UV new_uv = { static_cast<float>(i + (stage_inndex * PIECE_NUM)), world_inndex };
			new_image->SetAnim(&new_uv);

			new_image->SetDefaultPosAndColl(80.0f, 80.0f);

			this->mvp_object.emplace_back(new_image);
			this->m_object.m_ui.map_get_piece[i] = new_image;
		}

		for (size_t i = 0; i < this->m_object.m_ui.map_result_get_piece.size(); i++)
		{
			CUnit* new_image = new CUnit;
			new_image->Initilize();

			new_image->AddTexture(SPRITE_PIECE_1_in);

			Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
			new_image->SetColor(&color);

			Transform transform = new_image->GetTransform();
			transform.position.x = 0.0f;
			transform.position.y = 0.0f;
			switch (i)
			{
			case 0:
				transform.position.x += 112.0f;
				transform.position.y -= 56.0f;
				transform.angle.z = -15.0f;
				break;
			case 1:
				transform.position.x += 174.0f;
				transform.position.y -= 6.0f;
				transform.angle.z = 40.0f;
				break;
			case 2:
				transform.position.x += 270.0f;
				transform.position.y -= 56.0f;
				transform.angle.z = 15.0f;
				break;
			}
			new_image->SetTransform(&transform);

			UV new_uv = { static_cast<float>(i + (stage_inndex * PIECE_NUM)), world_inndex };
			new_image->SetAnim(&new_uv);

			new_image->SetDefaultPosAndColl(120.0f, 120.0f);

			this->mvp_object.emplace_back(new_image);
			this->m_object.m_ui.map_result_get_piece[i] = new_image;
		}

		for (size_t i = 0; i < this->m_object.m_ui.map_grand_get_piece.size(); i++)
		{
			CUnit* new_image = new CUnit;
			new_image->Initilize();

			new_image->AddTexture(SPRITE_PIECE_1_in);

			Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
			new_image->SetColor(&color);

			Transform transform = new_image->GetTransform();
			transform.position.x = -126.0f;
			transform.position.y = -20.0f;
			new_image->SetTransform(&transform);

			UV new_uv = { static_cast<float>(i + (stage_inndex * PIECE_NUM)), world_inndex };
			new_image->SetAnim(&new_uv);

			new_image->SetDefaultPosAndColl(220.0f, 220.0f);

			this->mvp_object.emplace_back(new_image);
			this->m_object.m_ui.map_grand_get_piece[i] = new_image;
		}
	}

	{
		CUnit* new_image = new CUnit;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_PAGE_FRAME);

		Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
		new_image->SetColor(&color);

		new_image->SetDefaultPosAndColl(1175.0f, 655.0f);

		this->mvp_object.emplace_back(new_image);
		this->m_object.m_ui.mp_clear_frame = new_image;
	}

	{
		CUnit* new_image = new CUnit;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_BOOK_FRAME);

		Transform transform = new_image->GetTransform();
		transform.position.x = -587.0f + 89.0f;
		new_image->SetTransform(&transform);

		Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
		new_image->SetColor(&color);

		new_image->SetDefaultPosAndColl(171.0f, 655.0f);

		this->mvp_object.emplace_back(new_image);
		this->m_object.m_ui.mp_clear_book = new_image;
	}

	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < SMB_MAX; j++)
		{
			CUnit* new_image = new CUnit;
			new_image->Initilize();

			Transform transform = new_image->GetTransform();
			transform.position.x = -332.0f + 172.0f + (427.0f * static_cast<float>(j));
			transform.position.y = -138.0f - 37.0f;

			switch (i)
			{
			case 0:
				new_image->AddTexture(SPRITE_UI_BUTTON);
				new_image->SetDefaultPosAndColl(345.0f, 75.0f);

				this->m_object.m_ui.mvp_result_button.emplace_back(new_image);
				break;
			case 1:
				new_image->AddTexture(SPRITE_UI_BUTTON_FRAME);
				new_image->SetDefaultPosAndColl(350.0f, 90.0f);

				this->m_object.m_ui.mvp_result_frame.emplace_back(new_image);
				break;
			case 2:
				new_image->AddTexture(SPRITE_UI_CURSOR);
				new_image->SetDefaultPosAndColl(50.0f, 53.0f);

				transform.position.x -= (172.0f + 25.0f + 10.0f);

				this->m_object.m_ui.mvp_result_cursor.emplace_back(new_image);
				break;
			}

			new_image->SetTransform(&transform);

			Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
			new_image->SetColor(&color);


			this->mvp_object.emplace_back(new_image);
		}
	}

	{
		CImage* new_image = new CImage;
		new_image->Initilize();

		new_image->AddTexture(SPRITE_UI_GAME_OVER);

		Transform transform = new_image->GetTransform();
		transform.position.x = 34.0f;
		transform.position.y = 60.0f;
		new_image->SetTransform(&transform);

		Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
		new_image->SetColor(&color);

		new_image->SetDefaultPosAndColl(765.0f, 440.0f);

		this->mvp_object.emplace_back(new_image);
		this->m_object.m_ui.mp_over_back = new_image;
	}

	{
		for (size_t i = 0; i < this->m_object.map_label.size(); i++)
		{
			this->m_object.map_label[i] = new CLabel;
			this->m_object.map_label[i]->Initilize();

			UV new_uv = { static_cast<float>(i), 0.0f };
			this->m_object.map_label[i]->SetAnim(&new_uv);

			Transform transform = this->m_object.map_label[i]->GetTransform();
			transform.position.x = -10000.0f;
			transform.position.y = MAPCHIP_HEIGHT * static_cast<float>(MAPCHIP_NUM_HEIGHT / 2) + 10.0f;
			this->m_object.map_label[i]->SetTransform(&transform);

			this->mvp_object.emplace_back(this->m_object.map_label[i]);
		}
	}

	{
		SpriteData new_data;
		std::string file_name;
		int world_num = this->mp_game_manager->GetWorldIndex();
		switch (world_num)
		{
		case 2:
			file_name = "assets/Game/Sprites/Common/Title/back_02.png";
			break;
		case 1:
			file_name = "assets/Game/Sprites/Common/Title/back_01.png";
			break;
		case 0:
			file_name = "assets/Game/Sprites/Common/Title/back_00.png";
			break;
		default:
			file_name = "assets/Game/Sprites/Common/Title/back_00.png";
			break;
		}

		HRESULT hr = direct3d->LoadTexture(file_name.c_str(), &new_data.mSRV);
		std::string error_text = "画像読み込みに失敗しました。\nファイル名 : " + file_name;
		CMainWindow* cmain = CSingleton<CMainWindow>::GetInstance();
		if (!cmain->ErrorCheck(hr, error_text.c_str())) {
			new_data.anim_max = 2;
			new_data.slice_x = 2;
			new_data.slice_y = 1;

			new_data.slice.u = 1.0f / 2.0f;
			new_data.slice.v = 1.0f;

			new_data.count_check = 1.0f;

			this->m_stage_back_sprite = new_data;
		};
	}
}

void CStageManager::CreateGoal(std::vector<GoalData>* data, FRONT_BACK fb)
{
	for (size_t i = 0; i < data->size(); i++)
	{
		CGoal* new_goal = new CGoal;
		new_goal->Initilize();

		Transform transform = new_goal->GetTransform();
		transform.position = (*data)[i].position;
		if (fb == IS_BACK)
		{
			transform.position.x *= -1.0f;
			transform.angle.y = 180.0f;
		}
		new_goal->SetTransform(&transform);

		size_t size = this->m_fold.ma_page_nest.size();
		for (size_t i = 0; i < size; i++)
		{
			float center = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
			if (transform.position.x >= center - (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
				if (transform.position.x <= center + (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
					this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[fb].emplace_back(new_goal);
					break;
				}
			}
		}

		this->mvp_object.emplace_back(new_goal);
	}
}

void CStageManager::CreatePiece(std::vector<PieceData>* data, FRONT_BACK fb)
{
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();

	int world_index = game_manager->GetWorldIndex();
	int stage_index = game_manager->GetStageIndex();

	for (size_t i = 0; i < data->size(); i++)
	{
		CPiece* new_piece = new CPiece;
		new_piece->Initilize();

		Transform transform = new_piece->GetTransform();
		transform.position = (*data)[i].position;
		if (fb == IS_BACK)
		{
			transform.position.x *= -1.0f;
			transform.angle.y = 180.0f;
		}
		new_piece->SetTransform(&transform);

		new_piece->SetPieceNum(world_index, stage_index, (*data)[i].i_piece_num);

		size_t size = this->m_fold.ma_page_nest.size();
		for (size_t i = 0; i < size; i++)
		{
			float center = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
			if (transform.position.x >= center - (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
				if (transform.position.x <= center + (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
					this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[fb].emplace_back(new_piece);
					break;
				}
			}
		}

		this->mvp_object.emplace_back(new_piece);
	}
}

void CStageManager::CreateTornade(std::vector<TornadeData>* data, FRONT_BACK fb)
{
	for (size_t i = 0; i < data->size(); i++)
	{
		CTornade* new_tornade = new CTornade;
		new_tornade->Initilize();

		Transform transform = new_tornade->GetTransform();
		transform.position = (*data)[i].position;
		if (fb == IS_BACK)
		{
			transform.position.x *= -1.0f;
			transform.angle.y = 180.0f;
		}
		new_tornade->SetTransform(&transform);

		size_t size = this->m_fold.ma_page_nest.size();
		for (size_t i = 0; i < size; i++)
		{
			float center = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
			if (transform.position.x >= center - (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
				if (transform.position.x <= center + (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
					this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[fb].emplace_back(new_tornade);
					break;
				}
			}
		}

		this->mvp_object.emplace_back(new_tornade);
	}
}

void CStageManager::CreateCannon(std::vector<CannonData>* data, FRONT_BACK fb)
{
	for (size_t i = 0; i < data->size(); i++)
	{
		CCannon* new_cannon = new CCannon;
		new_cannon->Initilize();

		Transform transform = new_cannon->GetTransform();
		transform.position = (*data)[i].position;
		if (fb == IS_BACK)
		{
			transform.position.x *= -1.0f;
			transform.angle.y = 180.0f;
		}
		transform.angle.z = (*data)[i].mf_angle_z;
		new_cannon->SetTransform(&transform);

		new_cannon->SetData(&(*data)[i], this, fb);

		size_t size = this->m_fold.ma_page_nest.size();
		for (size_t i = 0; i < size; i++)
		{
			float center = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
			if (transform.position.x >= center - (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
				if (transform.position.x <= center + (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
					this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[fb].emplace_back(new_cannon);
					break;
				}
			}
		}

		this->mvp_object.emplace_back(new_cannon);
	}
}

void CStageManager::CreateDharma(std::vector<DharmaData>* data, FRONT_BACK fb)
{
	for (size_t i = 0; i < data->size(); i++)
	{
		CDharma* new_dharma = new CDharma;
		new_dharma->Initilize();

		Transform transform = new_dharma->GetTransform();
		transform.position = (*data)[i].position;
		if (fb == IS_BACK)
		{
			transform.position.x *= -1.0f;
			transform.angle.y = 180.0f;
		}
		new_dharma->SetTransform(&transform);

		new_dharma->SetData(&(*data)[i]);

		size_t size = this->m_fold.ma_page_nest.size();
		for (size_t i = 0; i < size; i++)
		{
			float center = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
			if (transform.position.x >= center - (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
				if (transform.position.x <= center + (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
					this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[fb].emplace_back(new_dharma);
					break;
				}
			}
		}

		this->mvp_object.emplace_back(new_dharma);
	}
}

void CStageManager::CreateLionTop(std::vector<LionData>* data, FRONT_BACK fb)
{
	for (size_t i = 0; i < data->size(); i++)
	{
		CLionTop* new_lion_top = new CLionTop;
		new_lion_top->Initilize();

		Transform transform = new_lion_top->GetTransform();
		transform.position = (*data)[i].position;
		if (fb == IS_BACK)
		{
			transform.position.x *= -1.0f;
			transform.angle.y = 180.0f;
		}
		new_lion_top->SetTransform(&transform);

		new_lion_top->SetData(&(*data)[i]);

		size_t size = this->m_fold.ma_page_nest.size();
		for (size_t i = 0; i < size; i++)
		{
			float center = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
			if (transform.position.x >= center - (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
				if (transform.position.x <= center + (MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH / 2))) {
					this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[fb].emplace_back(new_lion_top);
					break;
				}
			}
		}

		this->mvp_object.emplace_back(new_lion_top);
	}
}

void CStageManager::CreateBall(Translate position, float max_speed, float angle_z, FRONT_BACK fb)
{
	CBall* new_ball = nullptr;

	for (size_t i = 0; i < this->m_object.m_active.mvp_un_ball.size(); i++)
	{
		if (!this->m_object.m_active.mvp_un_ball[i]->IsActive()) {
			new_ball = this->m_object.m_active.mvp_un_ball[i];
			this->m_object.m_active.mvp_un_ball.erase(this->m_object.m_active.mvp_un_ball.begin() + i);
			break;
		}
	}

	if (new_ball == nullptr) {
		new_ball = new CBall;
		new_ball->Initilize();
		this->mvp_object.emplace_back(new_ball);
	}

	Transform transform = new_ball->GetTransform();
	transform.position = position;
	new_ball->SetTransform(&transform);

	new_ball->Create(max_speed, angle_z, fb);

	this->m_object.m_active.mvp_ball.emplace_back(new_ball);

	this->mp_xaudio2->Start(SOUND_LABEL_SE_CANNON);

}

void CStageManager::Player()
{
	{
		CPlayer* new_image = new CPlayer;
		new_image->Initilize();

		Transform tr = new_image->GetTransform();
		Collider collider = new_image->GetCollider();
		tr.position.x = (MAPCHIP_WIDTH * (-(static_cast<float>(MAPCHIP_NUM_WIDTH) * static_cast<float>(AREA_NUM)) / 2.0f + 0.5f) + (MAPCHIP_WIDTH * static_cast<float>(this->m_data.m_stage.m_player_pos_x))) + (collider.center_x * 2.0f);
		tr.position.y = (MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f - 1.0f) + (PLAYER_COLLIDER_HEIGHT / 2.0f) + (PLAYER_COLLIDER_HEIGHT / 2.0f) + (MAPCHIP_HEIGHT * static_cast<float>(this->m_data.m_stage.m_player_pos_y))) - (collider.center_y * 2.0f);
		new_image->SetTransform(&tr);

		this->m_object.mp_player = new_image;
		this->mvp_object.emplace_back(new_image);

		this->m_fold.ma_page_nest[this->m_data.m_stage.m_player_pos_x / 4].map_player[this->m_fold.ma_page_nest[this->m_data.m_stage.m_player_pos_x / 4].m_front_back] = new_image;
	}
}

void CStageManager::Initialized()
{
	for (size_t x = 0; x < this->m_fold.maai_page_index.size(); x++)
	{
		for (size_t y = 0; y < this->m_fold.maai_page_index[x].size(); y++)
		{
			this->m_fold.maai_page_index[x][y] = NONE_PAGE;
		}
	}

	this->Player();

	this->UpdateIndex();
}

void CStageManager::UpdateIndex()
{
	for (size_t x = 0; x < this->m_fold.maai_page_index.size(); x++)
	{
		for (size_t y = 0; y < this->m_fold.maai_page_index[x].size(); y++)
		{
			this->m_fold.maai_page_index[x][y] = NONE_PAGE;
		}
	}

	this->SortIndex();

	// 端っこの初期化
	this->m_fold.mi_top_x = AREA_NUM - 1;
	this->m_fold.mi_top_y = AREA_NUM - 1;
	this->m_fold.mi_tail_x = 0;
	this->m_fold.mi_tail_y = 0;

	// 要素配列を更新
	for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
	{
		int x = this->m_fold.ma_page_nest[i].mi_index_x;
		int y = this->m_fold.ma_page_nest[i].mi_index_y;

		// 端っこの更新
		if (x > this->m_fold.mi_tail_x) this->m_fold.mi_tail_x = x;
		if (y > this->m_fold.mi_tail_y) this->m_fold.mi_tail_y = y;
		if (x < this->m_fold.mi_top_x) this->m_fold.mi_top_x = x;
		if (y < this->m_fold.mi_top_y) this->m_fold.mi_top_y = y;

		this->m_fold.ma_page_nest[i].mb_is_active = false;

		this->m_fold.maai_page_index[x][y] = static_cast<int>(i);
	}

	// カーソルの場所を初期値に
	{
		this->mi_cursor_pos = this->m_fold.mi_top_x + 1;

		Transform transform = this->m_object.m_ui.mp_cursor_frame->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(this->mi_cursor_pos));
		this->m_object.m_ui.mp_cursor_frame->SetTransform(&transform);
	}

	/*
		{
		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		this->m_object.m_ui.map_back_color_image[BO_TOP]->SetImage(-(screen_width / 2.0f), (screen_width / 2.0f), (MAPCHIP_HEIGHT * mapchip_num), (screen_height / 2.0f));
		float new_v = ((MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v > 1.0f) new_v = 1.0f;
		if (new_v < 0.0f) new_v = 0.0f;
		new_v = 1.0f - new_v;
		this->m_object.m_ui.map_back_color_image[BO_TOP]->SetDefaultUV(0.0f, 1.0f, new_v, 0.0f);
	}
	{
		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		this->m_object.m_ui.map_back_color_image[BO_BOTTOM]->SetImage(-(screen_width / 2.0f), (screen_width / 2.0f), -(screen_height / 2.0f), -(MAPCHIP_HEIGHT * mapchip_num));
		float new_v = (-(MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v > 1.0f) new_v = 1.0f;
		if (new_v < 0.0f) new_v = 0.0f;
		new_v = 1.0f - new_v;
		this->m_object.m_ui.map_back_color_image[BO_BOTTOM]->SetDefaultUV(0.0f, 1.0f, 1.0f, new_v);
	}

	*/

	// CDirect3D取得
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// スクリーンサイズ格納
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	{
		// 左境界の移動
		Transform transform = this->m_object.m_border.map_border[BO_LEFT]->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-4.0f + static_cast<float>(this->m_fold.mi_top_x));
		this->m_object.m_border.map_border[BO_LEFT]->SetTransform(&transform);

		// 右端のアクティブなページを探す
		PageNest* page_nest = nullptr;
		for (size_t i = this->m_fold.mi_tail_y + 1; i > this->m_fold.mi_top_y; i--)
		{
			if (this->m_fold.maai_page_index[this->m_fold.mi_top_x][i - 1] != NONE_PAGE) {
				page_nest = &this->m_fold.ma_page_nest[this->m_fold.maai_page_index[this->m_fold.mi_top_x][i - 1]];
				break;
			}
		}

		// 左方向のマップチップの偽物を用意
		FRONT_BACK front_back = IS_FRONT;
		if (page_nest->m_front_back == IS_FRONT) front_back = IS_BACK;
		for (size_t i = 0; i < this->m_object.map_fake_left_chip.size(); i++)
		{
			Transform tr = this->m_object.map_fake_left_chip[i]->GetTransform();
			if (page_nest->maaai_chip_pos[front_back][i][3] != NONE_MAPCHIP) {
				Collider collider = this->m_object.mvp_mapchip[page_nest->maaai_chip_pos[front_back][i][3]]->GetCollider();
				if (collider.is_active) {
					tr.position.x = transform.position.x + (MAPCHIP_WIDTH * 1.5f);
				}
				else {
					tr.position.x = -1000.0f;
				}
			}
			else {
				tr.position.x = -1000.0f;
			}
			this->m_object.map_fake_left_chip[i]->SetTransform(&tr);
		}

		this->UpdateFakeGimmick(page_nest, front_back, IS_LEFT);

		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		float new_v_1 = (-(MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v_1 > 1.0f) new_v_1 = 1.0f;
		if (new_v_1 < 0.0f) new_v_1 = 0.0f;
		new_v_1 = 1.0f - new_v_1;
		float new_v_2 = ((MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v_2 > 1.0f) new_v_2 = 1.0f;
		if (new_v_2 < 0.0f) new_v_2 = 0.0f;
		new_v_2 = 1.0f - new_v_2;
		{
			this->m_object.m_ui.map_back_image[BO_LEFT]->SetImage(-(screen_width / 2.0f), (transform.position.x + (MAPCHIP_WIDTH * 2.0f)), (MAPCHIP_HEIGHT * mapchip_num), -(MAPCHIP_HEIGHT * mapchip_num));
			float new_u = ((transform.position.x + (MAPCHIP_WIDTH * 2.0f)) + (screen_width / 2.0f)) / screen_width;
			if (new_u > 1.0f) new_u = 1.0f;
			if (new_u < 0.0f) new_u = 0.0f;
			this->m_object.m_ui.map_back_image[BO_LEFT]->SetDefaultUV(0.0f, new_u, new_v_1, new_v_2);
		}
		{
			this->m_object.m_ui.map_back_color_image[BO_LEFT]->SetImage(-(screen_width / 2.0f), (transform.position.x + (MAPCHIP_WIDTH * 2.0f)), (MAPCHIP_HEIGHT * mapchip_num), -(MAPCHIP_HEIGHT * mapchip_num));
			float new_u = ((transform.position.x + (MAPCHIP_WIDTH * 2.0f)) + (screen_width / 2.0f)) / screen_width;
			if (new_u > 1.0f) new_u = 1.0f;
			if (new_u < 0.0f) new_u = 0.0f;
			this->m_object.m_ui.map_back_color_image[BO_LEFT]->SetDefaultUV(0.0f, new_u, new_v_1, new_v_2);
		}

		// 左ワープ地点の移動
		{
			Transform tr = transform;
			tr.position.x -= (MAPCHIP_WIDTH / 4.0f);
			this->m_object.m_border.mp_left_warp->SetTransform(&tr);
		}
	}

	{
		// 右境界の移動
		Transform transform = this->m_object.m_border.map_border[BO_RIGHT]->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-2.0f + static_cast<float>(this->m_fold.mi_tail_x));
		this->m_object.m_border.map_border[BO_RIGHT]->SetTransform(&transform);

		// 左端のアクティブなページを探す
		PageNest* page_nest = nullptr;
		for (size_t i = this->m_fold.mi_tail_y + 1; i > this->m_fold.mi_top_y; i--)
		{
			if (this->m_fold.maai_page_index[this->m_fold.mi_tail_x][i - 1] != NONE_PAGE) {
				page_nest = &this->m_fold.ma_page_nest[this->m_fold.maai_page_index[this->m_fold.mi_tail_x][i - 1]];
				break;
			}
		}

		// 右方向のマップチップの偽物を用意
		FRONT_BACK front_back = IS_FRONT;
		if (page_nest->m_front_back == IS_FRONT) front_back = IS_BACK;
		for (size_t i = 0; i < this->m_object.map_fake_right_chip.size(); i++)
		{
			Transform tr = this->m_object.map_fake_right_chip[i]->GetTransform();
			if (page_nest->maaai_chip_pos[front_back][i][0] != NONE_MAPCHIP) {
				Collider collider = this->m_object.mvp_mapchip[page_nest->maaai_chip_pos[front_back][i][0]]->GetCollider();
				if (collider.is_active) {
					tr.position.x = transform.position.x - (MAPCHIP_WIDTH * 1.5f);
				}
				else {
					tr.position.x = -1000.0f;
				}
			}
			else {
				tr.position.x = -1000.0f;
			}
			this->m_object.map_fake_right_chip[i]->SetTransform(&tr);
		}

		this->UpdateFakeGimmick(page_nest, front_back, IS_RIGHT);

		float mapchip_num = static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f;
		float new_v_1 = (-(MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v_1 > 1.0f) new_v_1 = 1.0f;
		if (new_v_1 < 0.0f) new_v_1 = 0.0f;
		new_v_1 = 1.0f - new_v_1;
		float new_v_2 = ((MAPCHIP_HEIGHT * mapchip_num) + (screen_height / 2.0f)) / screen_height;
		if (new_v_2 > 1.0f) new_v_2 = 1.0f;
		if (new_v_2 < 0.0f) new_v_2 = 0.0f;
		new_v_2 = 1.0f - new_v_2;
		{
			this->m_object.m_ui.map_back_image[BO_RIGHT]->SetImage((transform.position.x - (MAPCHIP_WIDTH * 2.0f)), (screen_width / 2.0f), (MAPCHIP_HEIGHT * mapchip_num), -(MAPCHIP_HEIGHT * mapchip_num));
			float new_u = ((transform.position.x - (MAPCHIP_WIDTH * 2.0f)) + (screen_width / 2.0f)) / screen_width;
			if (new_u > 1.0f) new_u = 1.0f;
			if (new_u < 0.0f) new_u = 0.0f;
			this->m_object.m_ui.map_back_image[BO_RIGHT]->SetDefaultUV(new_u, 1.0f, new_v_1, new_v_2);
		}
		{
			this->m_object.m_ui.map_back_color_image[BO_RIGHT]->SetImage((transform.position.x - (MAPCHIP_WIDTH * 2.0f)), (screen_width / 2.0f), (MAPCHIP_HEIGHT * mapchip_num), -(MAPCHIP_HEIGHT * mapchip_num));
			float new_u = ((transform.position.x - (MAPCHIP_WIDTH * 2.0f)) + (screen_width / 2.0f)) / screen_width;
			if (new_u > 1.0f) new_u = 1.0f;
			if (new_u < 0.0f) new_u = 0.0f;
			this->m_object.m_ui.map_back_color_image[BO_RIGHT]->SetDefaultUV(new_u, 1.0f, new_v_1, new_v_2);
		}

		// 右ワープ地点の移動
		{
			Transform tr = transform;
			tr.position.x += (MAPCHIP_WIDTH / 4.0f);
			this->m_object.m_border.mp_right_warp->SetTransform(&tr);
		}
	}

	if (this->m_stage_back_sprite.mSRV != nullptr) {
		this->m_object.m_ui.mp_back_image->SetTexture(&this->m_stage_back_sprite);
		for (size_t i = 0; i < this->m_object.m_ui.map_back_image.size(); i++)
		{
			this->m_object.m_ui.map_back_image[i]->SetTexture(&this->m_stage_back_sprite);
		}
	}
	if (this->m_fold.m_data.m_front_back == IS_FRONT) {
		if (this->m_stage_back_sprite.mSRV != nullptr) {
			UV new_uv = { 0.0f, 0.0f };
			this->m_object.m_ui.mp_back_image->SetAnim(&new_uv);
			for (size_t i = 0; i < this->m_object.m_ui.map_back_image.size(); i++)
			{
				this->m_object.m_ui.map_back_image[i]->SetAnim(&new_uv);
			}
		}
	}
	else {
		if (this->m_stage_back_sprite.mSRV != nullptr) {
			UV new_uv = { 1.0f, 0.0f };
			this->m_object.m_ui.mp_back_image->SetAnim(&new_uv);
			for (size_t i = 0; i < this->m_object.m_ui.map_back_image.size(); i++)
			{
				this->m_object.m_ui.map_back_image[i]->SetAnim(&new_uv);
			}
		}
	}

	// 当たり判定用配列初期化
	for (size_t i = 0; i < this->m_object.m_active.mav_mapchip.size(); i++)
	{
		this->m_object.m_active.mav_mapchip[i].clear();
	}

	this->m_object.m_active.mp_player = nullptr;
	this->m_object.m_active.mvp_fixed_gimmick.clear();
	this->m_object.m_active.mvp_un_fixed_gimmick.clear();
	this->m_object.m_active.mvp_ball.clear();

	// 更新用ページの探り当て
	for (size_t x = m_fold.mi_top_x; x <= m_fold.mi_tail_x; x++)
	{
		for (size_t y = m_fold.mi_top_y; y <= m_fold.mi_tail_y; y++)
		{
			int index = this->m_fold.maai_page_index[x][y];
			if (index != NONE_PAGE) {
				this->m_fold.ma_page_nest[index].mb_is_active = true;

				// 当たり判定用配列更新
				for (size_t i = 0; i < this->m_object.m_active.mav_mapchip.size(); i++)
				{
					this->m_object.m_active.mav_mapchip[i].emplace_back(this->m_object.map_fake_left_chip[i]);

					for (size_t j = 0; j < this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back][i].size(); j++)
					{
						this->m_object.m_active.mav_mapchip[i].emplace_back(this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back][i][j]);
					}

					this->m_object.m_active.mav_mapchip[i].emplace_back(this->m_object.map_fake_right_chip[i]);
				}

				// プレイヤー獲得
				if (this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back] != nullptr) this->m_object.m_active.mp_player = this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back];

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_object.m_active.mvp_fixed_gimmick.emplace_back(this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back][i]);
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_object.m_active.mvp_un_fixed_gimmick.emplace_back(this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back][i]);
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_object.m_active.mvp_ball.emplace_back(this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back][i]);
				}

				break;
			}
		}
	}

	// カーソルの更新
	{
		CInput* input = CSingleton<CInput>::GetInstance();
		POINT point = input->GetCursorClientDefference();

		CCursor* cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();

		for (size_t i = this->m_fold.mi_top_x; i < this->m_fold.mi_tail_x + 3; i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = cursor->GetConvertedCollider();
			Collider other_collider = this->m_object.m_ui.map_check_image[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_pos = static_cast<int>(i);
				break;
			}
		}
	}
}

void CStageManager::SortIndex()
{
	int tail_x = 0;
	int tail_y = 0;

	for (int i = 0; i < this->m_fold.ma_page_nest.size(); i++)
	{
		int x = this->m_fold.ma_page_nest[i].mi_index_x;
		int y = this->m_fold.ma_page_nest[i].mi_index_y;

		// 端っこの更新
		if (x > tail_x) tail_x = x;
		if (y > tail_y) tail_y = y;
	}

	std::vector<std::vector<int>> tmp_index;
	tmp_index.resize(tail_x + 1);
	for (size_t i = 0; i < tmp_index.size(); i++)
	{
		tmp_index[i].resize(tail_y + 1);
		for (size_t j = 0; j < tmp_index[i].size(); j++)
		{
			tmp_index[i][j] = NONE_PAGE;
		}
	}

	for (int i = 0; i < this->m_fold.ma_page_nest.size(); i++)
	{
		int x = this->m_fold.ma_page_nest[i].mi_index_x;
		int y = this->m_fold.ma_page_nest[i].mi_index_y;

		tmp_index[x][y] = i;
	}

	for (size_t x = 0; x < tmp_index.size(); x++)
	{
		for (size_t j = 0, y = 0; j < tmp_index[x].size(); j++)
		{
			if (tmp_index[x][j] != NONE_PAGE) {
				this->m_fold.ma_page_nest[tmp_index[x][j]].mi_index_x = static_cast<int>(x);
				this->m_fold.ma_page_nest[tmp_index[x][j]].mi_index_y = static_cast<int>(y);
				y++;
			};
		}
	}
}

void CStageManager::UpdatePageObject()
{
	std::vector<PageNest*> page_nest;

	// アクティブなページを探索
	for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
	{
		for (size_t j = 0; j < m_fold.ma_page_nest[i].map_player.size(); j++)
		{
			m_fold.ma_page_nest[i].map_player[j] = nullptr;
		}

		if (!m_fold.ma_page_nest[i].mb_is_active) continue;

		m_fold.ma_page_nest[i].mvp_ball[m_fold.ma_page_nest[i].m_front_back].clear();
		m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[m_fold.ma_page_nest[i].m_front_back].clear();

		page_nest.emplace_back(&m_fold.ma_page_nest[i]);
	}

	// プレイヤーの所在ページ決定
	for (size_t i = 0; i < page_nest.size(); i++)
	{
		{
			// それぞれのコライダーを取得
			Collider obj_collider = this->m_object.mp_player->GetConvertedCollider();
			Collider other_collider = page_nest[i]->mp_page->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test_Point(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				page_nest[i]->map_player[page_nest[i]->m_front_back] = this->m_object.mp_player;
			}
		}

		for (size_t j = 0; j < this->m_object.m_active.mvp_un_fixed_gimmick.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider_2 = this->m_object.m_active.mvp_un_fixed_gimmick[j]->GetConvertedCollider();
			Collider other_collider_2 = page_nest[i]->mp_page->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test_Point(&obj_collider_2, &other_collider_2);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				page_nest[i]->mvp_un_fixed_gimmick[page_nest[i]->m_front_back].emplace_back(this->m_object.m_active.mvp_un_fixed_gimmick[j]);
			}
		}

		for (size_t j = 0; j < this->m_object.m_active.mvp_ball.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider_2 = this->m_object.m_active.mvp_ball[j]->GetConvertedCollider();
			Collider other_collider_2 = page_nest[i]->mp_page->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test_Point(&obj_collider_2, &other_collider_2);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				page_nest[i]->mvp_ball[page_nest[i]->m_front_back].emplace_back(this->m_object.m_active.mvp_ball[j]);
			}
		}
	}
}

void CStageManager::SortPageObject()
{
	this->UpdatePageObject();

	for (size_t x = this->m_fold.mi_top_x; x <= this->m_fold.mi_tail_x; x++)
	{
		for (size_t y = this->m_fold.mi_top_y; y <= this->m_fold.mi_tail_y; y++)
		{
			int index = this->m_fold.maai_page_index[x][y];
			if (index != NONE_PAGE) {

				if (this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back] != nullptr) {
					for (size_t i = 0; i < 2; i++)
					{
						// それぞれのコライダーを取得
						Collider obj_collider = this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back]->GetConvertedCollider();
						Collider other_collider = this->m_object.m_ui.map_limit_image[x + (i * 2)]->GetConvertedCollider();

						// 当たり判定結果
						COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

						// is_hit == true のとき当たってます。
						if (hit.is_hit) {
							this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back]->OnCollision(&hit, this->m_object.m_ui.map_limit_image[x + (i * 2)]);
						}
					}
					this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back]->UpdateTransform();
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					for (size_t j = 0; j < 2; j++)
					{
						// それぞれのコライダーを取得
						Collider obj_collider = this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back][i]->GetConvertedCollider();
						Collider other_collider = this->m_object.m_ui.map_limit_image[x + (j * 2)]->GetConvertedCollider();

						// 当たり判定結果
						COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

						// is_hit == true のとき当たってます。
						if (hit.is_hit) {
							this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back][i]->OnCollision(&hit, this->m_object.m_ui.map_limit_image[x + (j * 2)]);
						}
					}
					this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back][i]->UpdateTransform();
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					for (size_t j = 0; j < 2; j++)
					{
						// それぞれのコライダーを取得
						Collider obj_collider = this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back][i]->GetConvertedCollider();
						Collider other_collider = this->m_object.m_ui.map_limit_image[x + (j * 2)]->GetConvertedCollider();

						// 当たり判定結果
						COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

						// is_hit == true のとき当たってます。
						if (hit.is_hit) {
							this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back][i]->OnCollision(&hit, this->m_object.m_ui.map_limit_image[x + (j * 2)]);
						}
					}
					this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back][i]->UpdateTransform();
				}
				break;
			}
		}
	}
}

void CStageManager::UpdateFakeGimmick(PageNest * page_nest, FRONT_BACK front_back, LEFT_RIGHT left_right)
{
	std::vector<CImage*>* fake_gimmick;
	Transform page_transform = page_nest->mp_page->GetTransform();
	Transform border_transform;
	std::vector<CUnit*>* gimmick[2];
	gimmick[0] = &page_nest->mvp_fixed_gimmick[front_back];
	gimmick[1] = &page_nest->mvp_un_fixed_gimmick[front_back];

	if (left_right == IS_LEFT) {
		fake_gimmick = &this->m_object.mvp_fake_left_gimmick;
		border_transform = this->m_object.m_border.map_border[BO_LEFT]->GetTransform();
	}
	else {
		fake_gimmick = &this->m_object.mvp_fake_right_gimmick;
		border_transform = this->m_object.m_border.map_border[BO_RIGHT]->GetTransform();
	}

	size_t fake_gimmick_size = 0;
	Transform default_transform = { -1000.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	for (size_t i = 0; i < fake_gimmick->size(); i++)
	{
		(*fake_gimmick)[i]->SetTransform(&default_transform);
	}

	for (size_t i = 0; i < 2; i++)
	{
		for (size_t j = 0; j < (*gimmick[i]).size(); j++)
		{
			ObjectTag gimmick_tag = (*gimmick[i])[j]->GetTag();
			switch (gimmick_tag)
			{
			case TAG_CANNON:
			case TAG_LION_TOP:
				if (fake_gimmick_size >= fake_gimmick->size()) {
					CImage* new_image = new CImage;
					new_image->Initilize();

					ObjectTag new_tag = TAG_MAPCHIP;
					new_image->SetTag(&new_tag);
					new_image->AddTexture(SPRITE_TEST_0);

					this->mvp_object.emplace_back(new_image);
					fake_gimmick->emplace_back(new_image);
				}

				{
					Transform gimmick_transform = (*gimmick[i])[j]->GetTransform();
					gimmick_transform.position.x -= page_transform.position.x;
					gimmick_transform.position.x = border_transform.position.x - gimmick_transform.position.x;
					(*fake_gimmick)[fake_gimmick_size]->SetTransform(&gimmick_transform);
					switch (gimmick_tag)
					{
					case TAG_CANNON:
						(*fake_gimmick)[fake_gimmick_size]->SetDefaultPosAndColl(CANNON_WIDTH, CANNON_HEIGHT);
						break;
					case TAG_LION_TOP:
						(*fake_gimmick)[fake_gimmick_size]->SetDefaultPosAndColl(LION_WIDTH, LION_HEIGHT);
						break;
					}
				}

				fake_gimmick_size++;

				break;
			}
		}
	}
}

void CStageManager::MapChipTest(CGameObject * p_obj)
{
	std::vector<CGameObject*> vp_obj;
	vp_obj.emplace_back(p_obj);
	this->MapChipTest(&vp_obj);
}

void CStageManager::MapChipTest(std::vector<CGameObject*>* vp_obj)
{
	for (size_t i = 0; i < vp_obj->size(); i++)
	{
		Velocity velocity = (*vp_obj)[i]->GetVelocity();

		if (velocity.y < 0) {
			for (size_t y = 0; y < this->m_object.m_active.mav_mapchip.size(); y++)
			{
				if (velocity.x < 0) {
					for (size_t x = 0; x < this->m_object.m_active.mav_mapchip[y].size(); x++) {
						// それぞれのコライダーを取得
						Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
						Collider other_collider = this->m_object.m_active.mav_mapchip[y][x]->GetConvertedCollider();

						// 当たり判定結果
						COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

						// is_hit == true のとき当たってます。
						if (hit.is_hit) {
							(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_active.mav_mapchip[y][x]);
							this->m_object.m_active.mav_mapchip[y][x]->OnCollision(&hit, (*vp_obj)[i]);
						}
					}
				}
				else {
					for (size_t x = this->m_object.m_active.mav_mapchip[y].size(); x > 0; x--) {
						// それぞれのコライダーを取得
						Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
						Collider other_collider = this->m_object.m_active.mav_mapchip[y][x - 1]->GetConvertedCollider();

						// 当たり判定結果
						COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

						// is_hit == true のとき当たってます。
						if (hit.is_hit) {
							(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_active.mav_mapchip[y][x - 1]);
							this->m_object.m_active.mav_mapchip[y][x - 1]->OnCollision(&hit, (*vp_obj)[i]);
						}
					}
				}
			}
		}
		else {
			for (size_t y = this->m_object.m_active.mav_mapchip.size(); y > 0; y--)
			{
				if (velocity.x > 0) {
					for (size_t x = 0; x < this->m_object.m_active.mav_mapchip[y - 1].size(); x++) {
						// それぞれのコライダーを取得
						Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
						Collider other_collider = this->m_object.m_active.mav_mapchip[y - 1][x]->GetConvertedCollider();

						// 当たり判定結果
						COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

						// is_hit == true のとき当たってます。
						if (hit.is_hit) {
							(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_active.mav_mapchip[y - 1][x]);
							this->m_object.m_active.mav_mapchip[y - 1][x]->OnCollision(&hit, (*vp_obj)[i]);
						}
					}
				}
				else {
					for (size_t x = this->m_object.m_active.mav_mapchip[y - 1].size(); x > 0; x--) {
						// それぞれのコライダーを取得
						Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
						Collider other_collider = this->m_object.m_active.mav_mapchip[y - 1][x - 1]->GetConvertedCollider();

						// 当たり判定結果
						COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

						// is_hit == true のとき当たってます。
						if (hit.is_hit) {
							(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_active.mav_mapchip[y - 1][x - 1]);
							this->m_object.m_active.mav_mapchip[y - 1][x - 1]->OnCollision(&hit, (*vp_obj)[i]);
						}
					}
				}
			}
		}

		for (size_t j = 0; j < this->m_object.m_border.map_border.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
			Collider other_collider = this->m_object.m_border.map_border[j]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_border.map_border[j]);
			}
		}
	}
}

void CStageManager::FixedGimmickTest(CGameObject * p_obj)
{
	std::vector<CGameObject*> vp_obj;
	vp_obj.emplace_back(p_obj);
	this->FixedGimmickTest(&vp_obj);
}

void CStageManager::FixedGimmickTest(std::vector<CGameObject*>* vp_obj)
{
	for (size_t i = 0; i < vp_obj->size(); i++)
	{
		for (size_t j = 0; j < this->m_object.m_active.mvp_fixed_gimmick.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
			Collider other_collider = this->m_object.m_active.mvp_fixed_gimmick[j]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_active.mvp_fixed_gimmick[j]);
				this->m_object.m_active.mvp_fixed_gimmick[j]->OnCollision(&hit, (*vp_obj)[i]);
			}
		}
	}
}

void CStageManager::UnFixedGimmickTest(CGameObject * p_obj)
{
	std::vector<CGameObject*> vp_obj;
	vp_obj.emplace_back(p_obj);
	this->UnFixedGimmickTest(&vp_obj);
}

void CStageManager::UnFixedGimmickTest(std::vector<CGameObject*>* vp_obj)
{
	for (size_t i = 0; i < vp_obj->size(); i++)
	{
		for (size_t j = 0; j < this->m_object.m_active.mvp_un_fixed_gimmick.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
			Collider other_collider = this->m_object.m_active.mvp_un_fixed_gimmick[j]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_active.mvp_un_fixed_gimmick[j]);
				this->m_object.m_active.mvp_un_fixed_gimmick[j]->OnCollision(&hit, (*vp_obj)[i]);
			}
		}
	}
}

void CStageManager::BallTest(CGameObject * p_obj)
{
	std::vector<CGameObject*> vp_obj;
	vp_obj.emplace_back(p_obj);
	this->BallTest(&vp_obj);
}

void CStageManager::BallTest(std::vector<CGameObject*>* vp_obj)
{
	for (size_t i = 0; i < vp_obj->size(); i++)
	{
		for (size_t j = 0; j < this->m_object.m_active.mvp_ball.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
			Collider other_collider = this->m_object.m_active.mvp_ball[j]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				(*vp_obj)[i]->OnCollision(&hit, this->m_object.m_active.mvp_ball[j]);
				this->m_object.m_active.mvp_ball[j]->OnCollision(&hit, (*vp_obj)[i]);
			}
		}
	}
}

void CStageManager::FakeGimmickTest(CGameObject * p_obj)
{
	std::vector<CGameObject*> vp_obj;
	vp_obj.emplace_back(p_obj);
	this->FakeGimmickTest(&vp_obj);
}

void CStageManager::FakeGimmickTest(std::vector<CGameObject*>* vp_obj)
{
	for (size_t i = 0; i < vp_obj->size(); i++)
	{
		for (size_t j = 0; j < this->m_object.mvp_fake_left_gimmick.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
			Collider other_collider = this->m_object.mvp_fake_left_gimmick[j]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				(*vp_obj)[i]->OnCollision(&hit, this->m_object.mvp_fake_left_gimmick[j]);
			}
		}

		for (size_t j = 0; j < this->m_object.mvp_fake_right_gimmick.size(); j++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = (*vp_obj)[i]->GetConvertedCollider();
			Collider other_collider = this->m_object.mvp_fake_right_gimmick[j]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				(*vp_obj)[i]->OnCollision(&hit, this->m_object.mvp_fake_right_gimmick[j]);
			}
		}
	}
}

void CStageManager::WarpTest()
{

	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->m_object.mp_player->GetConvertedCollider();
		Collider other_collider = this->m_object.m_border.mp_left_warp->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->m_object.mp_player->OnCollision(&hit, this->m_object.m_border.mp_left_warp);
			if (this->m_condition == this->m_next_condition) {
				this->m_next_condition = FOLD_TURN_PREPARE;
				this->m_fold.m_data.m_left_right = IS_LEFT;
				return;
			}
		}
	}

	{
		// それぞれのコライダーを取得
		Collider obj_collider = this->m_object.mp_player->GetConvertedCollider();
		Collider other_collider = this->m_object.m_border.mp_right_warp->GetConvertedCollider();

		// 当たり判定結果
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true のとき当たってます。
		if (hit.is_hit) {
			this->m_object.mp_player->OnCollision(&hit, this->m_object.m_border.mp_right_warp);
			if (this->m_condition == this->m_next_condition) {
				this->m_next_condition = FOLD_TURN_PREPARE;
				this->m_fold.m_data.m_left_right = IS_RIGHT;
				return;
			}
		}
	}

}

void CStageManager::InitilizeFoldAxis(PageNest * target)
{
	target->mp_page->InitilizeFold();

	for (size_t i = 0; i < target->maavp_chip.size(); i++)
	{
		for (size_t x = 0; x < target->maavp_chip[i].size(); x++)
		{
			for (size_t y = 0; y < target->maavp_chip[i][x].size(); y++)
			{
				target->maavp_chip[i][x][y]->InitilizeFold();
			}
		}
	}

	for (size_t i = 0; i < target->mvp_fixed_gimmick.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_fixed_gimmick[i].size(); j++)
		{
			target->mvp_fixed_gimmick[i][j]->InitilizeFold();
		}
	}

	for (size_t i = 0; i < target->mvp_un_fixed_gimmick.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_un_fixed_gimmick[i].size(); j++)
		{
			target->mvp_un_fixed_gimmick[i][j]->InitilizeFold();
		}
	}

	for (size_t i = 0; i < target->mvp_ball.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_ball[i].size(); j++)
		{
			target->mvp_ball[i][j]->InitilizeFold();
		}
	}

	for (size_t i = 0; i < target->map_player.size(); i++)
	{
		if (target->map_player[i] != nullptr) target->map_player[i]->InitilizeFold();
	}

	for (size_t i = 0; i < target->mavi_label_index.size(); i++)
	{
		for (size_t j = 0; j < target->mavi_label_index[i].size(); j++)
		{
			this->m_object.map_label[target->mavi_label_index[i][j]]->InitilizeFold();
		}
	}
}

void CStageManager::SetFoldAxis(PageNest * target)
{
	this->SetFoldAxis(target, false);
}

void CStageManager::SetFoldAxisOpen(PageNest * target)
{
	this->SetFoldAxis(target, this->m_fold.mb_reverse_open);
}

void CStageManager::SetFoldAxis(PageNest * target, bool reverse)
{
	Translate axis = { this->m_fold.m_data.mf_axis_x, 0.0f, 0.0f };
	float angle = 0.0f;

	if (reverse) axis.x *= -1.0f;

	target->mp_page->SetFoldAngle(angle);
	target->mp_page->SetAxis(&axis);
	target->mp_page->SetNotRotate(false);

	for (size_t i = 0; i < target->maavp_chip.size(); i++)
	{
		for (size_t x = 0; x < target->maavp_chip[i].size(); x++)
		{
			for (size_t y = 0; y < target->maavp_chip[i][x].size(); y++)
			{
				target->maavp_chip[i][x][y]->SetFoldAngle(angle);
				target->maavp_chip[i][x][y]->SetAxis(&axis);
				target->maavp_chip[i][x][y]->SetNotRotate(false);
			}
		}
	}

	for (size_t i = 0; i < target->map_player.size(); i++)
	{
		if (target->map_player[i] != nullptr) {
			target->map_player[i]->SetFoldAngle(angle);
			target->map_player[i]->SetAxis(&axis);
			target->map_player[i]->SetNotRotate(false);
		}
	}

	for (size_t i = 0; i < target->mvp_fixed_gimmick.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_fixed_gimmick[i].size(); j++)
		{
			target->mvp_fixed_gimmick[i][j]->SetFoldAngle(angle);
			target->mvp_fixed_gimmick[i][j]->SetAxis(&axis);
			target->mvp_fixed_gimmick[i][j]->SetNotRotate(false);
		}
	}

	for (size_t i = 0; i < target->mvp_un_fixed_gimmick.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_un_fixed_gimmick[i].size(); j++)
		{
			target->mvp_un_fixed_gimmick[i][j]->SetFoldAngle(angle);
			target->mvp_un_fixed_gimmick[i][j]->SetAxis(&axis);
			target->mvp_un_fixed_gimmick[i][j]->SetNotRotate(false);
		}
	}

	for (size_t i = 0; i < target->mvp_ball.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_ball[i].size(); j++)
		{
			target->mvp_ball[i][j]->SetFoldAngle(angle);
			target->mvp_ball[i][j]->SetAxis(&axis);
			target->mvp_ball[i][j]->SetNotRotate(false);
		}
	}

	for (size_t i = 0; i < target->mavi_label_index.size(); i++)
	{
		for (size_t j = 0; j < target->mavi_label_index[i].size(); j++)
		{
			this->m_object.map_label[target->mavi_label_index[i][j]]->SetFoldAngle(angle);
			this->m_object.map_label[target->mavi_label_index[i][j]]->SetAxis(&axis);
			this->m_object.map_label[target->mavi_label_index[i][j]]->SetNotRotate(false);
		}
	}
}

void CStageManager::SetFrontAxis(PageNest * target)
{
	this->SetFrontAxis(target, false, false);
}

void CStageManager::SetFrontAxisOpen(PageNest * target)
{
	this->SetFrontAxis(target, this->m_fold.mb_reverse_open, true);
}

void CStageManager::SetFrontAxis(PageNest * target, bool reverse, bool is_open)
{
	Translate axis = { static_cast<float>(this->m_fold.m_data.mvvi_fold.size() * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH, 0.0f, 0.0f };
	float angle = 0.0f;

	if (this->m_fold.m_data.m_left_right == IS_LEFT) axis.x *= -1.0f;
	if (reverse) axis.x *= -1.0f;
	if (is_open) axis.x *= -1.0f;

	{
		Transform transform = target->mp_page->GetTransform();
		Translate new_axis = { transform.position.x + axis.x, 0.0f, 0.0f };
		target->mp_page->SetFoldAngle(angle);
		target->mp_page->SetAxis(&new_axis);
		target->mp_page->SetNotRotate(true);
	}

	for (size_t i = 0; i < target->maavp_chip.size(); i++)
	{
		for (size_t x = 0; x < target->maavp_chip[i].size(); x++)
		{
			for (size_t y = 0; y < target->maavp_chip[i][x].size(); y++)
			{
				Transform transform = target->maavp_chip[i][x][y]->GetTransform();
				Translate new_axis = { transform.position.x + axis.x, 0.0f, 0.0f };
				target->maavp_chip[i][x][y]->SetFoldAngle(angle);
				target->maavp_chip[i][x][y]->SetAxis(&new_axis);
				target->maavp_chip[i][x][y]->SetNotRotate(true);
			}
		}
	}

	for (size_t i = 0; i < target->map_player.size(); i++)
	{
		if (target->map_player[i] != nullptr) {
			Transform transform = target->map_player[i]->GetTransform();
			Translate new_axis = { transform.position.x + axis.x, 0.0f, 0.0f };
			target->map_player[i]->SetFoldAngle(angle);
			target->map_player[i]->SetAxis(&new_axis);
			target->map_player[i]->SetNotRotate(true);
		}
	}

	for (size_t i = 0; i < target->mvp_fixed_gimmick.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_fixed_gimmick[i].size(); j++)
		{
			Transform transform = target->mvp_fixed_gimmick[i][j]->GetTransform();
			Translate new_axis = { transform.position.x + axis.x, 0.0f, 0.0f };
			target->mvp_fixed_gimmick[i][j]->SetFoldAngle(angle);
			target->mvp_fixed_gimmick[i][j]->SetAxis(&new_axis);
			target->mvp_fixed_gimmick[i][j]->SetNotRotate(true);
		}
	}

	for (size_t i = 0; i < target->mvp_un_fixed_gimmick.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_un_fixed_gimmick[i].size(); j++)
		{
			Transform transform = target->mvp_un_fixed_gimmick[i][j]->GetTransform();
			Translate new_axis = { transform.position.x + axis.x, 0.0f, 0.0f };
			target->mvp_un_fixed_gimmick[i][j]->SetFoldAngle(angle);
			target->mvp_un_fixed_gimmick[i][j]->SetAxis(&new_axis);
			target->mvp_un_fixed_gimmick[i][j]->SetNotRotate(true);
		}
	}

	for (size_t i = 0; i < target->mvp_ball.size(); i++)
	{
		for (size_t j = 0; j < target->mvp_ball[i].size(); j++)
		{
			Transform transform = target->mvp_ball[i][j]->GetTransform();
			Translate new_axis = { transform.position.x + axis.x, 0.0f, 0.0f };
			target->mvp_ball[i][j]->SetFoldAngle(angle);
			target->mvp_ball[i][j]->SetAxis(&new_axis);
			target->mvp_ball[i][j]->SetNotRotate(true);
		}
	}

	for (size_t i = 0; i < target->mavi_label_index.size(); i++)
	{
		for (size_t j = 0; j < target->mavi_label_index[i].size(); j++)
		{
			Transform transform = this->m_object.map_label[target->mavi_label_index[i][j]]->GetTransform();
			Translate new_axis = { transform.position.x + axis.x, 0.0f, 0.0f };
			this->m_object.map_label[target->mavi_label_index[i][j]]->SetFoldAngle(angle);
			this->m_object.map_label[target->mavi_label_index[i][j]]->SetAxis(&new_axis);
			this->m_object.map_label[target->mavi_label_index[i][j]]->SetNotRotate(true);
		}
	}
}

void CStageManager::NormalUpdate()
{
	for (size_t i = 0; i < this->m_object.m_active.mvp_fixed_gimmick.size(); i++)
	{
		this->m_object.m_active.mvp_fixed_gimmick[i]->Update();
	}

	for (size_t i = 0; i < this->m_object.mvp_rb_block.size(); i++)
	{
		this->m_object.mvp_rb_block[i]->Update();
	}

	bool is_ground = false;

	for (size_t i = 0; i < this->m_object.m_active.mvp_un_fixed_gimmick.size(); i++)
	{
		this->m_object.m_active.mvp_un_fixed_gimmick[i]->Update();

		this->MapChipTest(this->m_object.m_active.mvp_un_fixed_gimmick[i]);
		this->FakeGimmickTest(this->m_object.m_active.mvp_un_fixed_gimmick[i]);

		this->FixedGimmickTest(this->m_object.m_active.mvp_un_fixed_gimmick[i]);
	}

	for (size_t i = 0; i < this->m_object.m_active.mvp_un_fixed_gimmick.size(); i++)
	{
		this->UnFixedGimmickTest(this->m_object.m_active.mvp_un_fixed_gimmick[i]);
	}

	for (size_t i = 0; i < this->m_object.m_active.mvp_ball.size(); i++)
	{
		this->m_object.m_active.mvp_ball[i]->Update();
		this->MapChipTest(this->m_object.m_active.mvp_ball[i]);
		this->FixedGimmickTest(this->m_object.m_active.mvp_ball[i]);
		this->UnFixedGimmickTest(this->m_object.m_active.mvp_ball[i]);
	}

	{
		this->m_object.mp_player->Update();

		this->MapChipTest(this->m_object.mp_player);
		this->FakeGimmickTest(this->m_object.mp_player);

		this->FixedGimmickTest(this->m_object.mp_player);

		this->UnFixedGimmickTest(this->m_object.mp_player);

		this->BallTest(this->m_object.mp_player);

		this->WarpTest();

		this->m_object.mp_player->UpdateTransform();

		is_ground = this->m_object.mp_player->GetGroundFlg();
	}

	for (size_t i = 0; i < this->m_object.m_active.mvp_un_fixed_gimmick.size(); i++)
	{
		this->m_object.m_active.mvp_un_fixed_gimmick[i]->UpdateTransform();
	}

	for (size_t i = 0; i < this->m_object.m_active.mvp_ball.size(); i++)
	{
		if (!this->m_object.m_active.mvp_ball[i]->IsActive()) {
			this->m_object.m_active.mvp_un_ball.emplace_back(this->m_object.m_active.mvp_ball[i]);
			this->m_object.m_active.mvp_ball.erase(this->m_object.m_active.mvp_ball.begin() + i);
			i--;
			continue;
		}

		this->m_object.m_active.mvp_ball[i]->UpdateTransform();
	}

	CInput* input = CSingleton<CInput>::GetInstance();
	CTimer* timer = CSingleton<CTimer>::GetInstance();
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// カーソルの更新
	{
		POINT point = input->GetCursorClientDefference();

		CCursor* cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();

		if (point.x != 0 || point.y != 0) {
			for (size_t i = this->m_fold.mi_top_x; i < this->m_fold.mi_tail_x + 3; i++)
			{
				// それぞれのコライダーを取得
				Collider obj_collider = cursor->GetConvertedCollider();
				Collider other_collider = this->m_object.m_ui.map_check_image[i]->GetConvertedCollider();

				// 当たり判定結果
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true のとき当たってます。
				if (hit.is_hit) {
					this->mi_cursor_pos = static_cast<int>(i);
					break;
				}
			}
		}
		else if (input->GetButtonTrigger(XI_BUTTON_LEFT_SHOULDER)) {
			if (this->mi_cursor_pos > static_cast<LONG>(this->m_fold.mi_top_x) - 1L) this->mi_cursor_pos--;
		}
		else if (input->GetButtonTrigger(XI_BUTTON_RIGHT_SHOULDER)) {
			if (this->mi_cursor_pos < static_cast<LONG>(this->m_fold.mi_tail_x) + 2L) this->mi_cursor_pos++;
		}

		Transform transform = this->m_object.m_ui.mp_cursor_frame->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-4.0f + static_cast<float>(this->mi_cursor_pos));
		this->m_object.m_ui.mp_cursor_frame->SetTransform(&transform);

		if (is_ground) {
			if (input->GetKeyTrigger(VK_LBUTTON) || input->GetButtonTrigger(XI_BUTTON_B)) {
				if (this->m_next_condition == this->m_condition) this->m_next_condition = FOLD_PAGE_SELECT_PREPARE;
			}
		}
	}

	// 開き関係
	if (is_ground) {
		if (input->GetKeyPress(VK_RBUTTON) || input->GetButtonPress(XI_BUTTON_X)) {
			this->m_fold.mf_push_time += timer->GetDeltaTime();
		}
		else if (input->GetKeyUp(VK_RBUTTON) || input->GetButtonUp(XI_BUTTON_X)) {
			if (this->m_fold.mf_push_time > this->m_fold.mf_push_time_limit) {
				if (this->m_next_condition == this->m_condition) this->m_next_condition = FOLD_OPEN_ALL_PREPARE;
			}
			else {
				if (this->m_next_condition == this->m_condition) this->m_next_condition = FOLD_OPEN_PREPARE;
			}

			this->m_fold.mf_push_time = 0.0f;
		}
		else {
			this->m_fold.mf_push_time = 0.0f;
		}
	}

	// 裏確認関係
	{
		if (input->GetKeyPress('E') || input->GetButtonPress(XI_BUTTON_Y)) {
			if (this->m_next_condition == this->m_condition) this->m_next_condition = FOLD_TURN_VIEW_PREPARE;
		}
	}

	{
		if (input->GetKeyPress(VK_ESCAPE) || input->GetButtonPress(XI_BUTTON_START) || input->GetButtonPress(XI_BUTTON_BACK)) {
			menu_manager->ChangeMenuSceneEX(MENU_PAUSE, false);
		}
	}

	if (this->m_object.mp_player->GetTransform().position.y <= this->m_object.m_border.map_border[BO_BOTTOM]->GetTransform().position.y) {
		game_manager->SetHP(HP_MIN);
	}

	int player_hp = game_manager->GetHP();
	{
		UV uv = this->m_object.m_ui.mp_hp->GetAnim();
		uv.v = static_cast<float>(3 - player_hp);
		this->m_object.m_ui.mp_hp->SetAnim(&uv);
	}

	for (size_t i = 0; i < this->m_object.m_ui.map_get_piece.size(); i++) {
		if (game_manager->CheckGetPiece(i)) {
			this->m_object.m_ui.map_get_piece[i]->ChangeTexture(SPRITE_PIECE_1_com);
		}
		else {
			this->m_object.m_ui.map_get_piece[i]->ChangeTexture(SPRITE_PIECE_1_in);
		}
	}

	if (this->m_condition != this->m_next_condition) {
		Color color = this->m_object.mp_player->GetColor();
		color.a = 1.0f;
		this->m_object.mp_player->SetColor(&color);
	}

	if (game_manager->CheckGoal()) {
		this->m_next_condition = FOLD_GAMECLEAR_PREPARE;
	}
	else if (player_hp <= HP_MIN) {
		this->m_next_condition = FOLD_GAMEOVER_PREPARE;
		this->m_object.mp_player->StartGameOver();
	}
}

void CStageManager::NormalDraw()
{
	this->m_object.m_ui.map_back_color_image[BO_TOP]->Draw();
	this->m_object.m_ui.map_back_image[BO_TOP]->Draw();

	this->m_object.m_ui.mp_hp->Draw();
	this->m_object.m_ui.mp_piece_frame->Draw();

	for (size_t x = this->m_fold.mi_top_x; x <= this->m_fold.mi_tail_x; x++)
	{
		for (size_t y = this->m_fold.mi_tail_y + 1; y >= this->m_fold.mi_top_y + 1; y--)
		{
			int index = this->m_fold.maai_page_index[x][y - 1];

			if (index != NONE_PAGE) {

				if (this->m_fold.ma_page_nest[index].mb_is_active) {
					FRONT_BACK front_back = IS_FRONT;
					if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) front_back = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->Draw();
					}

					this->m_fold.ma_page_nest[index].mp_page->Draw();

					for (size_t z = 0; z < this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back].size(); z++)
					{
						for (size_t w = 0; w < this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back][z].size(); w++)
						{
							this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back][z][w]->Draw();
						}
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back][i]]->Draw();
					}
				}
				else {
					FRONT_BACK front_back = IS_FRONT;
					if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) front_back = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->Draw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back][i]]->Draw();
					}
				}

			}
		}
	}

	for (size_t i = 0; i < this->m_object.m_active.mvp_ball.size(); i++)
	{
		this->m_object.m_active.mvp_ball[i]->Draw();
	}
	for (size_t i = 0; i < this->m_object.m_active.mvp_fixed_gimmick.size(); i++)
	{
		this->m_object.m_active.mvp_fixed_gimmick[i]->Draw();
	}
	for (size_t i = 0; i < this->m_object.m_active.mvp_un_fixed_gimmick.size(); i++)
	{
		this->m_object.m_active.mvp_un_fixed_gimmick[i]->Draw();
	}
	if (this->m_object.m_active.mp_player != nullptr) this->m_object.m_active.mp_player->Draw();
	//this->m_object.mp_player->Draw();

	this->m_object.m_ui.map_back_color_image[BO_LEFT]->Draw();
	this->m_object.m_ui.map_back_image[BO_LEFT]->Draw();

	this->m_object.m_ui.map_back_color_image[BO_RIGHT]->Draw();
	this->m_object.m_ui.map_back_image[BO_RIGHT]->Draw();

	this->m_object.m_ui.map_back_color_image[BO_BOTTOM]->Draw();
	this->m_object.m_ui.map_back_image[BO_BOTTOM]->Draw();

	this->m_object.m_ui.mp_fixed_cursor_frame->Draw();
	this->m_object.m_ui.mp_cursor_frame->Draw();

	for (size_t i = 0; i < this->m_object.m_ui.map_get_piece.size(); i++) {
		this->m_object.m_ui.map_get_piece[i]->Draw();
	}

	this->m_object.m_ui.mp_fade->Draw();
}

void CStageManager::TurnUpdate()
{
	switch (this->m_condition)
	{
	case FOLD_TURN_PREPARE:
	{
		Color color = this->m_object.m_ui.mp_fade->GetColor();
		color.a += this->m_fold.mf_fade_speed;
		if (color.a >= 1.0f) {
			color.a = 1.0f;
			this->m_next_condition = FOLD_TURN_ACT;
		}
		this->m_object.m_ui.mp_fade->SetColor(&color);
	}
	break;
	case FOLD_TURN_ACT:
		this->mp_xaudio2->Start(SOUND_LABEL_SE_BACKSIDE);

		this->SortPageObject();

		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			this->m_fold.ma_page_nest[i].mi_index_x = AREA_NUM - 1 - this->m_fold.ma_page_nest[i].mi_index_x;
			this->m_fold.ma_page_nest[i].mi_index_y = this->m_fold.mi_tail_y - this->m_fold.ma_page_nest[i].mi_index_y;

			float angle_y = 0.0f;

			if (this->m_fold.ma_page_nest[i].m_front_back == IS_FRONT) angle_y = 180.0f;
			else  angle_y = 0.0f;
			for (size_t y = 0; y < this->m_fold.ma_page_nest[i].maavp_chip[IS_FRONT].size(); y++)
			{
				for (size_t x = 0; x < this->m_fold.ma_page_nest[i].maavp_chip[IS_FRONT][y].size(); x++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].maavp_chip[IS_FRONT][y][x]->GetTransform();
					transform.position.x *= -1.0f;
					transform.angle.y = angle_y;
					this->m_fold.ma_page_nest[i].maavp_chip[IS_FRONT][y][x]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index[IS_FRONT].size(); j++)
			{
				Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[IS_FRONT][j]]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[IS_FRONT][j]]->SetTransform(&transform);
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_ball[IS_FRONT].size(); j++)
			{
				Transform transform = this->m_fold.ma_page_nest[i].mvp_ball[IS_FRONT][j]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_fold.ma_page_nest[i].mvp_ball[IS_FRONT][j]->SetTransform(&transform);
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[IS_FRONT].size(); j++)
			{
				Transform transform = this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[IS_FRONT][j]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[IS_FRONT][j]->SetTransform(&transform);
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[IS_FRONT].size(); j++)
			{
				Transform transform = this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[IS_FRONT][j]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[IS_FRONT][j]->SetTransform(&transform);
			}

			{
				Transform transform = this->m_fold.ma_page_nest[i].mp_page->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_fold.ma_page_nest[i].mp_page->SetTransform(&transform);
			}

			if (this->m_fold.ma_page_nest[i].m_front_back == IS_BACK) angle_y = 180.0f;
			else  angle_y = 0.0f;
			for (size_t y = 0; y < this->m_fold.ma_page_nest[i].maavp_chip[IS_BACK].size(); y++)
			{
				for (size_t x = 0; x < this->m_fold.ma_page_nest[i].maavp_chip[IS_BACK][y].size(); x++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].maavp_chip[IS_BACK][y][x]->GetTransform();
					transform.position.x *= -1.0f;
					transform.angle.y = angle_y;
					this->m_fold.ma_page_nest[i].maavp_chip[IS_BACK][y][x]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index[IS_BACK].size(); j++)
			{
				Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[IS_BACK][j]]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[IS_BACK][j]]->SetTransform(&transform);
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_ball[IS_BACK].size(); j++)
			{
				Transform transform = this->m_fold.ma_page_nest[i].mvp_ball[IS_BACK][j]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_fold.ma_page_nest[i].mvp_ball[IS_BACK][j]->SetTransform(&transform);
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[IS_BACK].size(); j++)
			{
				Transform transform = this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[IS_BACK][j]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[IS_BACK][j]->SetTransform(&transform);
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[IS_BACK].size(); j++)
			{
				Transform transform = this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[IS_BACK][j]->GetTransform();
				transform.position.x *= -1.0f;
				transform.angle.y = angle_y;
				this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[IS_BACK][j]->SetTransform(&transform);
			}

			if (this->m_fold.ma_page_nest[i].m_front_back == IS_FRONT) this->m_fold.ma_page_nest[i].m_front_back = IS_BACK;
			else this->m_fold.ma_page_nest[i].m_front_back = IS_FRONT;
		}

		{
			Transform transform = this->m_object.mp_player->GetTransform();
			transform.position.x *= -1.0f;
			this->m_object.mp_player->SetTransform(&transform);
		}

		this->UpdateIndex();
		this->UpdatePageObject();
		this->UpdateIndex();

		if (this->m_fold.m_data.m_front_back == IS_BACK) {
			if (this->m_stage_back_sprite.mSRV != nullptr) {
				UV new_uv = { 0.0f, 0.0f };
				this->m_object.m_ui.mp_back_image->SetAnim(&new_uv);
				for (size_t i = 0; i < this->m_object.m_ui.map_back_image.size(); i++)
				{
					this->m_object.m_ui.map_back_image[i]->SetAnim(&new_uv);
				}
			}
		}
		else {
			if (this->m_stage_back_sprite.mSRV != nullptr) {
				UV new_uv = { 1.0f, 0.0f };
				this->m_object.m_ui.mp_back_image->SetAnim(&new_uv);
				for (size_t i = 0; i < this->m_object.m_ui.map_back_image.size(); i++)
				{
					this->m_object.m_ui.map_back_image[i]->SetAnim(&new_uv);
				}
			}
		}

		this->m_next_condition = FOLD_TURN_END;
		break;
	case FOLD_TURN_END:
	{
		Color color = this->m_object.m_ui.mp_fade->GetColor();
		color.a -= this->m_fold.mf_fade_speed;
		if (color.a <= 0.0f) {
			color.a = 0.0f;
			if (this->m_fold.m_data.m_front_back == IS_BACK) this->m_fold.m_data.m_front_back = IS_FRONT;
			else  this->m_fold.m_data.m_front_back = IS_BACK;
			this->m_next_condition = FOLD_NONE;
		}
		this->m_object.m_ui.mp_fade->SetColor(&color);
	}
	break;
	}
}

void CStageManager::TurnDraw()
{
	this->NormalDraw();
}

void CStageManager::PageSelectUpdate()
{
	switch (this->m_condition)
	{
	case FOLD_PAGE_SELECT_PREPARE:
	{
		for (int i = SOUND_LABEL_BGM_STAGE_1; i <= SOUND_LABEL_BGM_STAGE_3; i++)
		{
			this->mp_xaudio2->XA_SetFadeout(static_cast<SOUND_LABEL>(i), DEFAULT_MAX_VOLUME / 4.0f, DEFAULT_DIFF_VOLUME);
		}

		this->UpdatePageObject();

		int player_pos = 0;

		for (size_t x = this->m_fold.mi_top_x; x <= this->m_fold.mi_tail_x; x++)
		{
			for (size_t y = this->m_fold.mi_top_y; y <= this->m_fold.mi_tail_y; y++)
			{
				int index = this->m_fold.maai_page_index[x][y];
				if (index != NONE_PAGE) {

					if (this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back] != nullptr) {
						player_pos = static_cast<int>(x);
					}

					break;
				}
			}
		}

		this->SortPageObject();

		for (size_t i = 0; i < this->m_fold.mab_can_page.size(); i++)
		{
			this->m_fold.mab_can_page[i] = false;
		}

		for (int i = static_cast<int>(this->m_fold.mi_top_x); i <= static_cast<int>(this->m_fold.mi_tail_x); i++)
		{
			int check = (i + 1) - this->mi_cursor_pos;
			if (check % 2 == 0) {
				this->m_fold.mab_can_page[i + 1] = true;
			}
		}

		if (player_pos + 1 != this->mi_cursor_pos) {
			int min_x = static_cast<int>(this->m_fold.mi_top_x) + 1;
			int max_x = player_pos + 1;
			if (player_pos > this->mi_cursor_pos - 1) {
				max_x = static_cast<int>(this->m_fold.mi_tail_x) + 1;
				min_x = player_pos + 1;
			}

			for (int i = min_x; i <= max_x; i++)
			{
				this->m_fold.mab_can_page[i] = false;
			}
		}

		this->m_fold.mab_can_page[this->mi_cursor_pos] = true;

		Transform transform = this->m_object.m_ui.mp_fixed_cursor_frame->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-4.0f + static_cast<float>(this->mi_cursor_pos));
		this->m_object.m_ui.mp_fixed_cursor_frame->SetTransform(&transform);

		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_object.m_ui.mp_fixed_cursor_frame->SetColor(&color);

		this->mi_old_cursor_pos = this->mi_cursor_pos;

		this->m_next_condition = FOLD_PAGE_SELECT_ACT;
	}
	break;
	case FOLD_PAGE_SELECT_ACT:
		// カーソルの更新
	{
		CInput* input = CSingleton<CInput>::GetInstance();

		POINT point = input->GetCursorClientDefference();

		CCursor* cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();

		if (point.x != 0 || point.y != 0) {
			for (size_t i = this->m_fold.mi_top_x; i < this->m_fold.mi_tail_x + 3; i++)
			{
				// それぞれのコライダーを取得
				Collider obj_collider = cursor->GetConvertedCollider();
				Collider other_collider = this->m_object.m_ui.map_check_image[i]->GetConvertedCollider();

				// 当たり判定結果
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true のとき当たってます。
				if (hit.is_hit) {
					this->mi_cursor_pos = static_cast<int>(i);
					break;
				}
			}
		}
		else if (input->GetButtonTrigger(XI_BUTTON_LEFT_SHOULDER)) {
			if (this->mi_cursor_pos > static_cast<LONG>(this->m_fold.mi_top_x) - 1L) this->mi_cursor_pos--;
		}
		else if (input->GetButtonTrigger(XI_BUTTON_RIGHT_SHOULDER)) {
			if (this->mi_cursor_pos < static_cast<LONG>(this->m_fold.mi_tail_x) + 1L) this->mi_cursor_pos++;
		}

		Transform transform = this->m_object.m_ui.mp_cursor_frame->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-4.0f + static_cast<float>(this->mi_cursor_pos));
		this->m_object.m_ui.mp_cursor_frame->SetTransform(&transform);

		if (input->GetKeyTrigger(VK_RBUTTON) ||
			input->GetKeyTrigger(VK_ESCAPE) ||
			input->GetButtonTrigger(XI_BUTTON_X) ||
			input->GetButtonTrigger(XI_BUTTON_BACK) ||
			input->GetButtonTrigger(XI_BUTTON_START)) {
			Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
			m_object.m_ui.mp_fixed_cursor_frame->SetColor(&color);
			this->m_next_condition = FOLD_NONE;
			for (int i = SOUND_LABEL_BGM_STAGE_1; i <= SOUND_LABEL_BGM_STAGE_3; i++)
			{
				this->mp_xaudio2->XA_SetFadein(static_cast<SOUND_LABEL>(i), DEFAULT_MAX_VOLUME, DEFAULT_DIFF_VOLUME);
			}
		}
		else if (input->GetKeyTrigger(VK_LBUTTON) || input->GetButtonTrigger(XI_BUTTON_B)) {
			if (this->m_fold.mab_can_page[this->mi_cursor_pos]) {

				Color color = { 1.0f, 1.0f, 1.0f, 0.0f };
				m_object.m_ui.mp_fixed_cursor_frame->SetColor(&color);

				if (this->mi_cursor_pos != this->mi_old_cursor_pos) {
					this->m_next_condition = FOLD_FOLD_PREPARE;
				}
				else {
					this->m_next_condition = FOLD_NONE;
				}
				for (int i = SOUND_LABEL_BGM_STAGE_1; i <= SOUND_LABEL_BGM_STAGE_3; i++)
				{
					this->mp_xaudio2->XA_SetFadein(static_cast<SOUND_LABEL>(i), DEFAULT_MAX_VOLUME, DEFAULT_DIFF_VOLUME);
				}
			}
		}

	}
	break;
	}
}

void CStageManager::PageSelectDraw()
{
	this->NormalDraw();
	for (size_t i = this->m_fold.mi_top_x + 1; i <= this->m_fold.mi_tail_x + 1; i++)
	{
		if (!this->m_fold.mab_can_page[i]) {
			this->m_object.m_ui.map_check_image[i]->Draw();
		}
	}
}

void CStageManager::FoldUpdate()
{
	switch (this->m_condition)
	{
	case FOLD_FOLD_PREPARE:
	{
		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			this->m_fold.m_data.maf_page_x[i] = this->m_fold.ma_page_nest[i].mi_index_x;
			this->m_fold.m_data.maf_page_y[i] = this->m_fold.ma_page_nest[i].mi_index_y;
		}

		this->m_fold.m_data.mvvi_base.clear();
		this->m_fold.m_data.mvvi_fold.clear();
		this->m_fold.m_data.mvvi_front.clear();

		int i_base = this->mi_old_cursor_pos - 1;
		int i_target = this->mi_cursor_pos - 1;

		const int BASE = 0;
		const int FOLD = 1;
		const int FRONT = 2;

		// for文用
		int for_top[3] = { 0, 0, 0 };
		int for_tail[3] = { 0, 0, 0 };

		// 折り用
		int sort_top[3] = { AREA_NUM, AREA_NUM, AREA_NUM };
		int sort_tail[3] = { -1, -1, -1 };

		// 向かって左折り
		if (this->mi_cursor_pos < this->mi_old_cursor_pos) {
			this->m_fold.m_data.m_left_right = IS_LEFT;

			i_base = ((i_target - i_base) / 2) + i_base - 1;

			for_top[BASE] = static_cast<int>(this->m_fold.mi_top_x);
			for_tail[BASE] = i_base;

			for_top[FOLD] = i_base + 1;
			for_tail[FOLD] = this->mi_old_cursor_pos - 2;

			for_top[FRONT] = this->mi_old_cursor_pos - 1;
			for_tail[FRONT] = static_cast<int>(this->m_fold.mi_tail_x);

			// 軸設定
			{
				float axis = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(for_tail[BASE]));
				axis += MAPCHIP_WIDTH * 2.0f;
				this->m_fold.m_data.mf_axis_x = axis;
			}

			// 札設定
			{
				if (for_top[FRONT] > for_tail[FRONT]) {
					for (size_t y = this->m_fold.mi_tail_y + 1; y > this->m_fold.mi_top_y; y--)
					{
						int index = this->m_fold.maai_page_index[for_tail[FOLD]][y - 1];
						if (index != NONE_PAGE) {
							FRONT_BACK front_back = IS_FRONT;
							if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) front_back = IS_BACK;

							this->m_fold.ma_page_nest[index].mavi_label_index[front_back].emplace_back(this->m_fold.mi_label_num);

							Transform transform = this->m_object.map_label[this->m_fold.mi_label_num]->GetTransform();
							transform.position.x = this->m_fold.ma_page_nest[index].mp_page->GetTransform().position.x - (LABEL_WIDTH * this->m_fold.mi_label_num);
							transform.angle.y = 180.0f;
							this->m_object.map_label[this->m_fold.mi_label_num]->SetTransform(&transform);
							break;
						}
					}
				}
				else {
					for (size_t y = this->m_fold.mi_top_y; y <= this->m_fold.mi_tail_y; y++)
					{
						int index = this->m_fold.maai_page_index[for_top[FRONT]][y];
						if (index != NONE_PAGE) {
							this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back].emplace_back(this->m_fold.mi_label_num);

							Transform transform = this->m_object.map_label[this->m_fold.mi_label_num]->GetTransform();
							transform.position.x = this->m_fold.ma_page_nest[index].mp_page->GetTransform().position.x + (LABEL_WIDTH * this->m_fold.mi_label_num);
							transform.angle.y = 0.0f;
							this->m_object.map_label[this->m_fold.mi_label_num]->SetTransform(&transform);
							break;
						}
					}
				}
			}
		}
		// 向かって右折り
		else {
			this->m_fold.m_data.m_left_right = IS_RIGHT;

			i_base = i_base - ((i_base - i_target) / 2) + 1;

			for_top[BASE] = i_base;
			for_tail[BASE] = static_cast<int>(this->m_fold.mi_tail_x);

			for_top[FOLD] = this->mi_old_cursor_pos;
			for_tail[FOLD] = i_base - 1;

			for_top[FRONT] = static_cast<int>(this->m_fold.mi_top_x);
			for_tail[FRONT] = this->mi_old_cursor_pos - 1;

			// 軸設定
			{
				float axis = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(for_top[BASE]));
				axis -= MAPCHIP_WIDTH * 2.0f;
				this->m_fold.m_data.mf_axis_x = axis;
			}

			// 札設定
			{
				if (for_top[FRONT] > for_tail[FRONT]) {
					for (size_t y = this->m_fold.mi_tail_y + 1; y > this->m_fold.mi_top_y; y--)
					{
						int index = this->m_fold.maai_page_index[for_top[FOLD]][y - 1];
						if (index != NONE_PAGE) {
							FRONT_BACK front_back = IS_FRONT;
							if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) front_back = IS_BACK;

							this->m_fold.ma_page_nest[index].mavi_label_index[front_back].emplace_back(this->m_fold.mi_label_num);

							Transform transform = this->m_object.map_label[this->m_fold.mi_label_num]->GetTransform();
							transform.position.x = this->m_fold.ma_page_nest[index].mp_page->GetTransform().position.x + (LABEL_WIDTH * this->m_fold.mi_label_num);
							transform.angle.y = 180.0f;
							this->m_object.map_label[this->m_fold.mi_label_num]->SetTransform(&transform);
							break;
						}
					}
				}
				else {
					for (size_t y = this->m_fold.mi_top_y; y <= this->m_fold.mi_tail_y; y++)
					{
						int index = this->m_fold.maai_page_index[for_tail[FRONT]][y];
						if (index != NONE_PAGE) {
							this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back].emplace_back(this->m_fold.mi_label_num);

							Transform transform = this->m_object.map_label[this->m_fold.mi_label_num]->GetTransform();
							transform.position.x = this->m_fold.ma_page_nest[index].mp_page->GetTransform().position.x - (LABEL_WIDTH * this->m_fold.mi_label_num);
							transform.angle.y = 0.0f;
							this->m_object.map_label[this->m_fold.mi_label_num]->SetTransform(&transform);
							break;
						}
					}
				}
			}
		}

		std::vector<std::vector<int>>* tmp_vrctor[3];
		tmp_vrctor[BASE] = &this->m_fold.m_data.mvvi_base;
		tmp_vrctor[FOLD] = &this->m_fold.m_data.mvvi_fold;
		tmp_vrctor[FRONT] = &this->m_fold.m_data.mvvi_front;

		for (size_t i = 0; i < 3; i++)
		{
			for (int x = for_top[i]; x <= for_tail[i]; x++)
			{
				std::vector<int> tmp;
				for (int y = static_cast<int>(this->m_fold.mi_top_y); y <= static_cast<int>(m_fold.mi_tail_y); y++)
				{
					if (this->m_fold.maai_page_index[x][y] != NONE_PAGE) {
						tmp.emplace_back(this->m_fold.maai_page_index[x][y]);
						if (sort_top[i] > y) sort_top[i] = y;
						if (sort_tail[i] < y) sort_tail[i] = y;
					}
				}
				tmp_vrctor[i]->emplace_back(tmp);
			}
		}

		int fold_height = (sort_tail[FOLD] - sort_top[FOLD] + 1);
		int front_height = (sort_tail[FRONT] - sort_top[FRONT] + 1);

		if (fold_height < 0) fold_height = 0;
		if (front_height < 0) front_height = 0;

		for (size_t x = 0; x < tmp_vrctor[BASE]->size(); x++)
		{
			for (size_t y = 0; y < (*tmp_vrctor[BASE])[x].size(); y++)
			{
				this->m_fold.ma_page_nest[(*tmp_vrctor[BASE])[x][y]].mi_index_y = static_cast<int>(y) + fold_height + front_height;
				this->InitilizeFoldAxis(&this->m_fold.ma_page_nest[(*tmp_vrctor[BASE])[x][y]]);
			}
		}

		// 向かって左折り
		if (this->m_fold.m_data.m_left_right == IS_LEFT) {
			for (size_t x = 0; x < tmp_vrctor[FOLD]->size(); x++)
			{
				for (size_t y = 0; y < (*tmp_vrctor[FOLD])[x].size(); y++)
				{
					this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]].mi_index_y = static_cast<int>((*tmp_vrctor[FOLD])[x].size() - y - 1) + front_height;
					this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]].mi_index_x = (for_top[FOLD] - this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]].mi_index_x) + for_tail[BASE];
					this->SetFoldAxis(&this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]]);
				}
			}

			for (size_t x = 0; x < tmp_vrctor[FRONT]->size(); x++)
			{
				for (size_t y = 0; y < (*tmp_vrctor[FRONT])[x].size(); y++)
				{
					this->m_fold.ma_page_nest[(*tmp_vrctor[FRONT])[x][y]].mi_index_y = static_cast<int>(y);
					this->m_fold.ma_page_nest[(*tmp_vrctor[FRONT])[x][y]].mi_index_x -= (for_tail[FOLD] - for_top[FOLD] + 1) * 2;
					this->SetFrontAxis(&this->m_fold.ma_page_nest[(*tmp_vrctor[FRONT])[x][y]]);
				}
			}
		}
		// 向かって右折り
		else {
			for (size_t x = 0; x < tmp_vrctor[FOLD]->size(); x++)
			{
				for (size_t y = 0; y < (*tmp_vrctor[FOLD])[x].size(); y++)
				{
					this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]].mi_index_y = static_cast<int>((*tmp_vrctor[FOLD])[x].size() - y - 1) + front_height;
					this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]].mi_index_x = (for_tail[FOLD] - this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]].mi_index_x) + for_top[BASE];
					this->SetFoldAxis(&this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]]);
				}
			}

			for (size_t x = 0; x < tmp_vrctor[FRONT]->size(); x++)
			{
				for (size_t y = 0; y < (*tmp_vrctor[FRONT])[x].size(); y++)
				{
					this->m_fold.ma_page_nest[(*tmp_vrctor[FRONT])[x][y]].mi_index_y = static_cast<int>(y);
					this->m_fold.ma_page_nest[(*tmp_vrctor[FRONT])[x][y]].mi_index_x += (for_tail[FOLD] - for_top[FOLD] + 1) * 2;
					this->SetFrontAxis(&this->m_fold.ma_page_nest[(*tmp_vrctor[FRONT])[x][y]]);
				}
			}
		}

		this->mf_angle_y = 0.0f;
		this->m_fold.mi_label_num++;
		this->m_next_condition = FOLD_FOLD_ACT;
		this->mp_xaudio2->Start(SOUND_LABEL_SE_FOLD_2);
	}
	break;
	case FOLD_FOLD_ACT:
	{
		this->mf_angle_y += this->m_fold.mf_rotate_speed;
		if (this->mf_angle_y >= 180.0f) {
			this->mf_angle_y = 180.0f;
			this->m_next_condition = FOLD_FOLD_END;
		}

		float new_angle = this->mf_angle_y;
		if (this->m_fold.m_data.m_left_right == IS_RIGHT) new_angle = -this->mf_angle_y;

		const int FOLD = 0;
		const int FRONT = 1;
		std::vector<std::vector<int>>* tmp_vrctor[2];
		tmp_vrctor[FOLD] = &this->m_fold.m_data.mvvi_fold;
		tmp_vrctor[FRONT] = &this->m_fold.m_data.mvvi_front;

		for (size_t i = 0; i < 2; i++)
		{
			for (size_t x = 0; x < tmp_vrctor[i]->size(); x++)
			{
				for (size_t y = 0; y < (*tmp_vrctor[i])[x].size(); y++)
				{
					if ((*tmp_vrctor[i])[x][y] == NONE_PAGE) continue;
					PageNest* target = &this->m_fold.ma_page_nest[(*tmp_vrctor[i])[x][y]];

					{
						target->mp_page->SetFoldAngle(new_angle);
					}

					for (size_t j = 0; j < target->maavp_chip.size(); j++)
					{
						for (size_t xx = 0; xx < target->maavp_chip[j].size(); xx++)
						{
							for (size_t yy = 0; yy < target->maavp_chip[j][xx].size(); yy++)
							{
								target->maavp_chip[j][xx][yy]->SetFoldAngle(new_angle);
							}
						}
					}

					for (size_t j = 0; j < target->map_player.size(); j++)
					{
						if (target->map_player[j] != nullptr) {
							target->map_player[j]->SetFoldAngle(new_angle);
						}
					}

					for (size_t j = 0; j < target->mvp_ball.size(); j++)
					{
						for (size_t k = 0; k < target->mvp_ball[j].size(); k++)
						{
							target->mvp_ball[j][k]->SetFoldAngle(new_angle);
						}
					}
					for (size_t j = 0; j < target->mvp_fixed_gimmick.size(); j++)
					{
						for (size_t k = 0; k < target->mvp_fixed_gimmick[j].size(); k++)
						{
							target->mvp_fixed_gimmick[j][k]->SetFoldAngle(new_angle);
						}
					}
					for (size_t j = 0; j < target->mvp_un_fixed_gimmick.size(); j++)
					{
						for (size_t k = 0; k < target->mvp_un_fixed_gimmick[j].size(); k++)
						{
							target->mvp_un_fixed_gimmick[j][k]->SetFoldAngle(new_angle);
						}
					}

					for (size_t j = 0; j < target->mavi_label_index.size(); j++)
					{
						for (size_t k = 0; k < target->mavi_label_index[j].size(); k++)
						{
							this->m_object.map_label[target->mavi_label_index[j][k]]->SetFoldAngle(new_angle);
						}
					}
				}
			}
		}
	}
	break;
	case FOLD_FOLD_END:
		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			int distance = this->m_fold.ma_page_nest[i].mi_index_x - this->m_fold.m_data.maf_page_x[i];

			{
				Transform transform = this->m_fold.ma_page_nest[i].mp_page->GetTransform();
				transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
				this->m_fold.ma_page_nest[i].mp_page->SetTransform(&transform);
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].maavp_chip.size(); j++)
			{
				for (size_t y = 0; y < this->m_fold.ma_page_nest[i].maavp_chip[j].size(); y++)
				{
					for (size_t x = 0; x < this->m_fold.ma_page_nest[i].maavp_chip[j][y].size(); x++)
					{
						Transform transform = this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->GetTransform();
						transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
						this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->SetTransform(&transform);
					}
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mavi_label_index[j].size(); k++)
				{
					Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->SetTransform(&transform);
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].map_player.size(); j++)
			{
				if (this->m_fold.ma_page_nest[i].map_player[j] != nullptr) {
					Transform transform = this->m_fold.ma_page_nest[i].map_player[j]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].map_player[j]->SetTransform(&transform);
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_ball.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_ball[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_ball[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_ball[j][k]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->SetTransform(&transform);
				}
			}
		}

		for (size_t x = 0; x < this->m_fold.m_data.mvvi_fold.size(); x++)
		{
			for (size_t y = 0; y < this->m_fold.m_data.mvvi_fold[x].size(); y++)
			{
				int index = this->m_fold.m_data.mvvi_fold[x][y];

				float axis = this->m_fold.ma_page_nest[index].mp_page->GetTransform().position.x;

				float angle[2] = { 0.0f, 180.0f };
				if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) {
					angle[IS_FRONT] = 180.0f;
					angle[IS_BACK] = 0.0f;
				}

				{
					Transform transform = this->m_fold.ma_page_nest[index].mp_page->GetTransform();
					transform.angle.y = angle[IS_FRONT];
					this->m_fold.ma_page_nest[index].mp_page->SetTransform(&transform);
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].maavp_chip.size(); j++)
				{
					for (size_t y = 0; y < this->m_fold.ma_page_nest[index].maavp_chip[j].size(); y++)
					{
						for (size_t x = 0; x < this->m_fold.ma_page_nest[index].maavp_chip[j][y].size(); x++)
						{
							Transform transform = this->m_fold.ma_page_nest[index].maavp_chip[j][y][x]->GetTransform();
							float distance = axis - transform.position.x;
							transform.position.x += distance * 2;
							transform.angle.y = angle[j];
							this->m_fold.ma_page_nest[index].maavp_chip[j][y][x]->SetTransform(&transform);
						}
					}
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mavi_label_index.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mavi_label_index[j].size(); k++)
					{
						Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[j][k]]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[j][k]]->SetTransform(&transform);
					}
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mvp_ball.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mvp_ball[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[index].mvp_ball[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[index].mvp_ball[j][k]->SetTransform(&transform);
					}
				}
				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[j][k]->SetTransform(&transform);
					}
				}
				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[j][k]->SetTransform(&transform);
					}
				}

				if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) {
					this->m_fold.ma_page_nest[index].m_front_back = IS_BACK;
				}
				else {
					this->m_fold.ma_page_nest[index].m_front_back = IS_FRONT;
				}
			}
		}

		this->mp_game_manager->AddFoldNum();
		this->mp_game_manager->ChangeFoldSwitch();

		//for (size_t i = 0; i < this->m_object.mvp_rb_block.size(); i++)
		//{
		//	this->m_object.mvp_rb_block[i]->Update();
		//}

		this->UpdateIndex();

		this->m_fold.ms_old_data.push(this->m_fold.m_data);

		this->m_next_condition = FOLD_NONE;
		break;
	}
}

void CStageManager::FoldDraw()
{
	switch (this->m_condition)
	{
	case FOLD_FOLD_ACT:
	{
		this->m_object.m_ui.mp_back_color_image->Draw();
		this->m_object.m_ui.mp_back_image->Draw();
		this->m_object.m_ui.mp_hp->Draw();
		this->m_object.m_ui.mp_piece_frame->Draw();
		this->BasePageDraw();
		this->FoldPageDraw();
		this->FrontPageDraw();
		for (size_t i = 0; i < this->m_object.m_ui.map_get_piece.size(); i++) {
			this->m_object.m_ui.map_get_piece[i]->Draw();
		}
	}
	break;
	case FOLD_FOLD_PREPARE:
	case FOLD_FOLD_END:
		this->NormalDraw();
		break;
	}
}

void CStageManager::OpenUpadate()
{
	switch (this->m_condition)
	{
	case FOLD_OPEN_PREPARE:
	{
		if (this->m_fold.ms_old_data.empty()) {
			this->m_next_condition = FOLD_NONE;
			return;
		}

		this->SortPageObject();

		FOLD_DATA fold_data = this->m_fold.ms_old_data.top();

		if (this->m_fold.m_data.m_front_back != fold_data.m_front_back) {
			this->m_fold.mb_reverse_open = true;

			float tail_y = 0;
			for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++) {
				if (fold_data.maf_page_y[i] > tail_y) tail_y = fold_data.maf_page_y[i];
			}

			for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++) {
				fold_data.maf_page_x[i] = AREA_NUM - fold_data.maf_page_x[i] - 1;
				fold_data.maf_page_y[i] = tail_y - fold_data.maf_page_y[i];
			}
		}
		else {
			this->m_fold.mb_reverse_open = false;
		}

		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			this->m_fold.m_data.maf_page_x[i] = this->m_fold.ma_page_nest[i].mi_index_x;
			this->m_fold.m_data.maf_page_y[i] = this->m_fold.ma_page_nest[i].mi_index_y;
			this->m_fold.ma_page_nest[i].mi_index_x = fold_data.maf_page_x[i];
			this->m_fold.ma_page_nest[i].mi_index_y = fold_data.maf_page_y[i];
		}
		this->m_fold.m_data.mf_axis_x = fold_data.mf_axis_x;
		this->m_fold.m_data.m_left_right = fold_data.m_left_right;
		this->m_fold.m_data.mvvi_base = fold_data.mvvi_base;
		this->m_fold.m_data.mvvi_fold = fold_data.mvvi_fold;
		this->m_fold.m_data.mvvi_front = fold_data.mvvi_front;

		const int BASE = 0;
		const int FOLD = 1;
		const int FRONT = 2;
		std::vector<std::vector<int>>* tmp_vrctor[3];
		tmp_vrctor[BASE] = &this->m_fold.m_data.mvvi_base;
		tmp_vrctor[FOLD] = &this->m_fold.m_data.mvvi_fold;
		tmp_vrctor[FRONT] = &this->m_fold.m_data.mvvi_front;

		for (size_t x = 0; x < tmp_vrctor[FOLD]->size(); x++)
		{
			size_t size = (*tmp_vrctor[FOLD])[x].size();
			if (size < 1) continue;
			std::vector<int> tmp;
			tmp.resize(size);

			for (size_t y = 0; y < size; y++)
			{
				tmp[size - 1 - y] = (*tmp_vrctor[FOLD])[x][y];
			}

			for (size_t y = 0; y < size; y++)
			{
				(*tmp_vrctor[FOLD])[x][y] = tmp[y];
			}
		}

		if (this->m_fold.mb_reverse_open)
		{
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t x = 0; x < tmp_vrctor[i]->size(); x++)
				{
					size_t size = (*tmp_vrctor[i])[x].size();
					if (size < 1) continue;
					std::vector<int> tmp;
					tmp.resize(size);

					for (size_t y = 0; y < size; y++)
					{
						tmp[size - 1 - y] = (*tmp_vrctor[i])[x][y];
					}

					for (size_t y = 0; y < size; y++)
					{
						(*tmp_vrctor[i])[x][y] = tmp[y];
					}
				}
			}
		}

		this->m_fold.mi_label_num--;

		{
			Transform transform = this->m_object.map_label[this->m_fold.mi_label_num]->GetTransform();
			transform.position.x += -10000.0f;
			this->m_object.map_label[this->m_fold.mi_label_num]->SetTransform(&transform);
		}

		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mavi_label_index[j].size(); k++)
				{
					if (this->m_fold.ma_page_nest[i].mavi_label_index[j][k] == this->m_fold.mi_label_num) {
						this->m_fold.ma_page_nest[i].mavi_label_index[j].erase(this->m_fold.ma_page_nest[i].mavi_label_index[j].begin() + k);
						k--;
					}
				}
			}
		}

		for (size_t x = 0; x < tmp_vrctor[BASE]->size(); x++)
		{
			for (size_t y = 0; y < (*tmp_vrctor[BASE])[x].size(); y++)
			{
				this->InitilizeFoldAxis(&this->m_fold.ma_page_nest[(*tmp_vrctor[BASE])[x][y]]);
			}
		}
		for (size_t x = 0; x < tmp_vrctor[FOLD]->size(); x++)
		{
			for (size_t y = 0; y < (*tmp_vrctor[FOLD])[x].size(); y++)
			{
				this->SetFoldAxisOpen(&this->m_fold.ma_page_nest[(*tmp_vrctor[FOLD])[x][y]]);
			}
		}
		for (size_t x = 0; x < tmp_vrctor[FRONT]->size(); x++)
		{
			for (size_t y = 0; y < (*tmp_vrctor[FRONT])[x].size(); y++)
			{
				this->SetFrontAxisOpen(&this->m_fold.ma_page_nest[(*tmp_vrctor[FRONT])[x][y]]);
			}
		}

		this->mf_angle_y = 0.0f;
		this->m_fold.ms_old_data.pop();

	}
	this->m_next_condition = FOLD_OPEN_ACT;
	this->mp_xaudio2->Start(SOUND_LABEL_SE_FOLD_2_RE);
	break;
	case FOLD_OPEN_ACT:
	{
		this->mf_angle_y += this->m_fold.mf_rotate_speed;
		if (this->mf_angle_y >= 180.0f) {
			this->mf_angle_y = 180.0f;
			this->m_next_condition = FOLD_OPEN_END;
		}

		float new_angle = -this->mf_angle_y;
		if (this->m_fold.m_data.m_left_right == IS_LEFT) new_angle *= -1.0f;
		if (this->m_fold.mb_reverse_open) new_angle *= -1.0f;

		const int FOLD = 0;
		const int FRONT = 1;
		std::vector<std::vector<int>>* tmp_vrctor[2];
		tmp_vrctor[FOLD] = &this->m_fold.m_data.mvvi_fold;
		tmp_vrctor[FRONT] = &this->m_fold.m_data.mvvi_front;

		for (size_t i = 0; i < 2; i++)
		{
			for (size_t x = 0; x < tmp_vrctor[i]->size(); x++)
			{
				for (size_t y = 0; y < (*tmp_vrctor[i])[x].size(); y++)
				{
					if ((*tmp_vrctor[i])[x][y] == NONE_PAGE) continue;
					PageNest* target = &this->m_fold.ma_page_nest[(*tmp_vrctor[i])[x][y]];

					{
						target->mp_page->SetFoldAngle(new_angle);
					}

					for (size_t j = 0; j < target->maavp_chip.size(); j++)
					{
						for (size_t xx = 0; xx < target->maavp_chip[j].size(); xx++)
						{
							for (size_t yy = 0; yy < target->maavp_chip[j][xx].size(); yy++)
							{
								target->maavp_chip[j][xx][yy]->SetFoldAngle(new_angle);
							}
						}
					}

					for (size_t j = 0; j < target->map_player.size(); j++)
					{
						if (target->map_player[j] != nullptr) {
							target->map_player[j]->SetFoldAngle(new_angle);
						}
					}

					for (size_t j = 0; j < target->mvp_ball.size(); j++)
					{
						for (size_t k = 0; k < target->mvp_ball[j].size(); k++)
						{
							target->mvp_ball[j][k]->SetFoldAngle(new_angle);
						}
					}
					for (size_t j = 0; j < target->mvp_fixed_gimmick.size(); j++)
					{
						for (size_t k = 0; k < target->mvp_fixed_gimmick[j].size(); k++)
						{
							target->mvp_fixed_gimmick[j][k]->SetFoldAngle(new_angle);
						}
					}
					for (size_t j = 0; j < target->mvp_un_fixed_gimmick.size(); j++)
					{
						for (size_t k = 0; k < target->mvp_un_fixed_gimmick[j].size(); k++)
						{
							target->mvp_un_fixed_gimmick[j][k]->SetFoldAngle(new_angle);
						}
					}

					for (size_t j = 0; j < target->mavi_label_index.size(); j++)
					{
						for (size_t k = 0; k < target->mavi_label_index[j].size(); k++)
						{
							this->m_object.map_label[target->mavi_label_index[j][k]]->SetFoldAngle(new_angle);
						}
					}
				}
			}
		}
	}
	break;
	case FOLD_OPEN_END:
		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			int distance = this->m_fold.ma_page_nest[i].mi_index_x - this->m_fold.m_data.maf_page_x[i];

			{
				Transform transform = this->m_fold.ma_page_nest[i].mp_page->GetTransform();
				transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
				this->m_fold.ma_page_nest[i].mp_page->SetTransform(&transform);
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].maavp_chip.size(); j++)
			{
				for (size_t y = 0; y < this->m_fold.ma_page_nest[i].maavp_chip[j].size(); y++)
				{
					for (size_t x = 0; x < this->m_fold.ma_page_nest[i].maavp_chip[j][y].size(); x++)
					{
						Transform transform = this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->GetTransform();
						transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
						this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->SetTransform(&transform);
					}
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mavi_label_index[j].size(); k++)
				{
					Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->SetTransform(&transform);
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].map_player.size(); j++)
			{
				if (this->m_fold.ma_page_nest[i].map_player[j] != nullptr) {
					Transform transform = this->m_fold.ma_page_nest[i].map_player[j]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].map_player[j]->SetTransform(&transform);
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_ball.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_ball[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_ball[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_ball[j][k]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->SetTransform(&transform);
				}
			}
		}

		for (size_t x = 0; x < this->m_fold.m_data.mvvi_fold.size(); x++)
		{
			for (size_t y = 0; y < this->m_fold.m_data.mvvi_fold[x].size(); y++)
			{
				int index = this->m_fold.m_data.mvvi_fold[x][y];

				float axis = this->m_fold.ma_page_nest[index].mp_page->GetTransform().position.x;

				float angle[2] = { 0.0f, 180.0f };
				if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) {
					angle[IS_FRONT] = 180.0f;
					angle[IS_BACK] = 0.0f;
				}

				{
					Transform transform = this->m_fold.ma_page_nest[index].mp_page->GetTransform();
					transform.angle.y = angle[IS_FRONT];
					this->m_fold.ma_page_nest[index].mp_page->SetTransform(&transform);
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].maavp_chip.size(); j++)
				{
					for (size_t y = 0; y < this->m_fold.ma_page_nest[index].maavp_chip[j].size(); y++)
					{
						for (size_t x = 0; x < this->m_fold.ma_page_nest[index].maavp_chip[j][y].size(); x++)
						{
							Transform transform = this->m_fold.ma_page_nest[index].maavp_chip[j][y][x]->GetTransform();
							float distance = axis - transform.position.x;
							transform.position.x += distance * 2;
							transform.angle.y = angle[j];
							this->m_fold.ma_page_nest[index].maavp_chip[j][y][x]->SetTransform(&transform);
						}
					}
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mavi_label_index.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mavi_label_index[j].size(); k++)
					{
						Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[j][k]]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[j][k]]->SetTransform(&transform);
					}
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].map_player.size(); j++)
				{
					if (this->m_fold.ma_page_nest[index].map_player[j] != nullptr) {
						Transform transform = this->m_fold.ma_page_nest[index].map_player[j]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[index].map_player[j]->SetTransform(&transform);
					}
				}


				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mvp_ball.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mvp_ball[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[index].mvp_ball[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[index].mvp_ball[j][k]->SetTransform(&transform);
					}
				}
				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[j][k]->SetTransform(&transform);
					}
				}
				for (size_t j = 0; j < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[j][k]->SetTransform(&transform);
					}
				}

				if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) {
					this->m_fold.ma_page_nest[index].m_front_back = IS_BACK;
				}
				else {
					this->m_fold.ma_page_nest[index].m_front_back = IS_FRONT;
				}
			}
		}

		this->mp_game_manager->ChangeFoldSwitch();

		//for (size_t i = 0; i < this->m_object.mvp_rb_block.size(); i++)
		//{
		//	this->m_object.mvp_rb_block[i]->Update();
		//}

		this->UpdateIndex();

		if (this->m_object.m_active.mp_player != nullptr) {
			this->m_next_condition = FOLD_NONE;
		}
		else {
			this->m_next_condition = FOLD_TURN_PREPARE;
		}

		break;
	}
}

void CStageManager::OpenDraw()
{
	switch (this->m_condition)
	{
	case FOLD_OPEN_ACT:
	{
		this->m_object.m_ui.mp_back_color_image->Draw();
		this->m_object.m_ui.mp_back_image->Draw();
		this->m_object.m_ui.mp_hp->Draw();
		this->m_object.m_ui.mp_piece_frame->Draw();

		if (this->m_fold.mb_reverse_open) {
			this->FrontPageDraw();
			this->FoldPageDraw();
			this->BasePageDraw();
		}
		else {
			this->BasePageDraw();
			this->FoldPageDraw();
			this->FrontPageDraw();
		}

		for (size_t i = 0; i < this->m_object.m_ui.map_get_piece.size(); i++) {
			this->m_object.m_ui.map_get_piece[i]->Draw();
		}
	}
	break;
	case FOLD_OPEN_PREPARE:
	case FOLD_OPEN_END:
		this->NormalDraw();
		break;
	}
}

void CStageManager::OpenAllUpadate()
{
	switch (this->m_condition)
	{
	case FOLD_OPEN_ALL_PREPARE:
	{
		Color color = this->m_object.m_ui.mp_fade->GetColor();
		color.a += this->m_fold.mf_fade_speed;
		if (color.a >= 1.0f) {
			color.a = 1.0f;
			this->m_next_condition = FOLD_OPEN_ALL_ACT;
		}
		this->m_object.m_ui.mp_fade->SetColor(&color);
	}
	break;
	case FOLD_OPEN_ALL_ACT:
		this->m_fold.mi_label_num = 0;

		this->SortPageObject();

		for (size_t i = 0; i < this->m_object.map_label.size(); i++)
		{
			Transform transform = this->m_object.map_label[i]->GetTransform();
			transform.position.x += -10000.0f;
			transform.angle.y = 0.0f;
			this->m_object.map_label[i]->SetTransform(&transform);
		}

		while (!this->m_fold.ms_old_data.empty())
		{
			this->m_fold.ms_old_data.pop();
			this->mp_game_manager->ChangeFoldSwitch();
			//for (size_t i = 0; i < this->m_object.mvp_rb_block.size(); i++)
			//{
			//	this->m_object.mvp_rb_block[i]->Update();
			//}
		}

		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index.size(); j++)
			{
				this->m_fold.ma_page_nest[i].mavi_label_index[j].clear();
			}

			this->m_fold.m_data.maf_page_x[i] = this->m_fold.ma_page_nest[i].mi_index_x;
			this->m_fold.m_data.maf_page_y[i] = this->m_fold.ma_page_nest[i].mi_index_y;
			if (this->m_fold.m_data.m_front_back == IS_FRONT) this->m_fold.ma_page_nest[i].mi_index_x = i;
			else this->m_fold.ma_page_nest[i].mi_index_x = this->m_fold.ma_page_nest.size() - i - 1;
			this->m_fold.ma_page_nest[i].mi_index_y = 0;
		}

		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			int distance = this->m_fold.ma_page_nest[i].mi_index_x - this->m_fold.m_data.maf_page_x[i];

			{
				Transform transform = this->m_fold.ma_page_nest[i].mp_page->GetTransform();
				transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
				this->m_fold.ma_page_nest[i].mp_page->SetTransform(&transform);
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].maavp_chip.size(); j++)
			{
				for (size_t y = 0; y < this->m_fold.ma_page_nest[i].maavp_chip[j].size(); y++)
				{
					for (size_t x = 0; x < this->m_fold.ma_page_nest[i].maavp_chip[j][y].size(); x++)
					{
						Transform transform = this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->GetTransform();
						transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
						this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->SetTransform(&transform);
					}
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mavi_label_index[j].size(); k++)
				{
					Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->SetTransform(&transform);
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].map_player.size(); j++)
			{
				if (this->m_fold.ma_page_nest[i].map_player[j] != nullptr) {
					Transform transform = this->m_fold.ma_page_nest[i].map_player[j]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].map_player[j]->SetTransform(&transform);
				}
			}

			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_ball.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_ball[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_ball[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_ball[j][k]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->SetTransform(&transform);
				}
			}
			for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j].size(); k++)
				{
					Transform transform = this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->GetTransform();
					transform.position.x += static_cast<float>(distance * MAPCHIP_NUM_WIDTH) * MAPCHIP_WIDTH;
					this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->SetTransform(&transform);
				}
			}

			if (this->m_fold.ma_page_nest[i].m_front_back != this->m_fold.m_data.m_front_back) {
				float axis = this->m_fold.ma_page_nest[i].mp_page->GetTransform().position.x;

				float angle[2] = { 0.0f, 180.0f };
				if (this->m_fold.ma_page_nest[i].m_front_back == IS_FRONT) {
					angle[IS_FRONT] = 180.0f;
					angle[IS_BACK] = 0.0f;
				}

				{
					Transform transform = this->m_fold.ma_page_nest[i].mp_page->GetTransform();
					transform.angle.y = angle[IS_FRONT];
					this->m_fold.ma_page_nest[i].mp_page->SetTransform(&transform);
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[i].maavp_chip.size(); j++)
				{
					for (size_t y = 0; y < this->m_fold.ma_page_nest[i].maavp_chip[j].size(); y++)
					{
						for (size_t x = 0; x < this->m_fold.ma_page_nest[i].maavp_chip[j][y].size(); x++)
						{
							Transform transform = this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->GetTransform();
							float distance = axis - transform.position.x;
							transform.position.x += distance * 2;
							transform.angle.y = angle[j];
							this->m_fold.ma_page_nest[i].maavp_chip[j][y][x]->SetTransform(&transform);
						}
					}
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mavi_label_index.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mavi_label_index[j].size(); k++)
					{
						Transform transform = this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_object.map_label[this->m_fold.ma_page_nest[i].mavi_label_index[j][k]]->SetTransform(&transform);
					}
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[i].map_player.size(); j++)
				{
					if (this->m_fold.ma_page_nest[i].map_player[j] != nullptr) {
						Transform transform = this->m_fold.ma_page_nest[i].map_player[j]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[i].map_player[j]->SetTransform(&transform);
					}
				}

				for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_ball.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_ball[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[i].mvp_ball[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[i].mvp_ball[j][k]->SetTransform(&transform);
					}
				}
				for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[i].mvp_fixed_gimmick[j][k]->SetTransform(&transform);
					}
				}
				for (size_t j = 0; j < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j].size(); k++)
					{
						Transform transform = this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->GetTransform();
						float distance = axis - transform.position.x;
						transform.position.x += distance * 2;
						transform.angle.y = angle[j];
						this->m_fold.ma_page_nest[i].mvp_un_fixed_gimmick[j][k]->SetTransform(&transform);
					}
				}

				if (this->m_fold.ma_page_nest[i].m_front_back == IS_FRONT) {
					this->m_fold.ma_page_nest[i].m_front_back = IS_BACK;
				}
				else {
					this->m_fold.ma_page_nest[i].m_front_back = IS_FRONT;
				}
			}
		}

		this->UpdateIndex();

		if (this->m_object.m_active.mp_player != nullptr) {
			this->m_next_condition = FOLD_OPEN_ALL_END;
		}
		else {
			this->m_next_condition = FOLD_TURN_PREPARE;
		}
		break;
	case FOLD_OPEN_ALL_END:
	{
		Color color = this->m_object.m_ui.mp_fade->GetColor();
		color.a -= this->m_fold.mf_fade_speed;
		if (color.a <= 0.0f) {
			color.a = 0.0f;
			this->m_next_condition = FOLD_NONE;
		}
		this->m_object.m_ui.mp_fade->SetColor(&color);
	}
	break;
	}
}

void CStageManager::OpenAllDraw()
{
	this->NormalDraw();
}

void CStageManager::ViewUpdate()
{
	switch (this->m_condition)
	{
	case FOLD_TURN_VIEW_PREPARE:
		this->SortPageObject();

		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			PageNest* target = &this->m_fold.ma_page_nest[i];

			target->mp_page->InitilizeFold();
			for (size_t j = 0; j < target->maavp_chip.size(); j++)
			{
				for (size_t x = 0; x < target->maavp_chip[j].size(); x++)
				{
					for (size_t y = 0; y < target->maavp_chip[j][x].size(); y++)
					{
						target->maavp_chip[j][x][y]->InitilizeFold();
					}
				}
			}
			for (size_t j = 0; j < target->map_player.size(); j++)
			{
				if (target->map_player[j] != nullptr) {
					target->map_player[j]->InitilizeFold();
				}
			}
			for (size_t j = 0; j < target->mvp_ball.size(); j++)
			{
				for (size_t k = 0; k < target->mvp_ball[j].size(); k++)
				{
					target->mvp_ball[j][k]->InitilizeFold();
				}
			}
			for (size_t j = 0; j < target->mvp_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < target->mvp_fixed_gimmick[j].size(); k++)
				{
					target->mvp_fixed_gimmick[j][k]->InitilizeFold();
				}
			}
			for (size_t j = 0; j < target->mvp_un_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < target->mvp_un_fixed_gimmick[j].size(); k++)
				{
					target->mvp_un_fixed_gimmick[j][k]->InitilizeFold();
				}
			}
			for (size_t j = 0; j < target->mavi_label_index.size(); j++)
			{
				for (size_t k = 0; k < target->mavi_label_index[j].size(); k++)
				{
					this->m_object.map_label[target->mavi_label_index[j][k]]->InitilizeFold();
				}
			}
		}

		this->mf_angle_y = 0.0f;

		this->m_next_condition = FOLD_TURN_VIEW_ACT;
		break;
	case FOLD_TURN_VIEW_ACT:
		if (this->mf_angle_y < 180.0f) {
			this->mf_angle_y += this->m_fold.mf_rotate_speed;

			if (this->mf_angle_y >= 180.0f) {
				this->mf_angle_y = 180.0f;
			}

			for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
			{
				PageNest* target = &this->m_fold.ma_page_nest[i];

				target->mp_page->SetFoldAngle(this->mf_angle_y);
				for (size_t j = 0; j < target->maavp_chip.size(); j++)
				{
					for (size_t x = 0; x < target->maavp_chip[j].size(); x++)
					{
						for (size_t y = 0; y < target->maavp_chip[j][x].size(); y++)
						{
							target->maavp_chip[j][x][y]->SetFoldAngle(this->mf_angle_y);
						}
					}
				}
				for (size_t j = 0; j < target->map_player.size(); j++)
				{
					if (target->map_player[j] != nullptr) {
						target->map_player[j]->SetFoldAngle(this->mf_angle_y);
					}
				}
				for (size_t j = 0; j < target->mvp_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < target->mvp_fixed_gimmick[j].size(); k++)
					{
						target->mvp_fixed_gimmick[j][k]->SetFoldAngle(this->mf_angle_y);
					}
				}
				for (size_t j = 0; j < target->mvp_ball.size(); j++)
				{
					for (size_t k = 0; k < target->mvp_ball[j].size(); k++)
					{
						target->mvp_ball[j][k]->SetFoldAngle(this->mf_angle_y);
					}
				}
				for (size_t j = 0; j < target->mvp_un_fixed_gimmick.size(); j++)
				{
					for (size_t k = 0; k < target->mvp_un_fixed_gimmick[j].size(); k++)
					{
						target->mvp_un_fixed_gimmick[j][k]->SetFoldAngle(this->mf_angle_y);
					}
				}
				for (size_t j = 0; j < target->mavi_label_index.size(); j++)
				{
					for (size_t k = 0; k < target->mavi_label_index[j].size(); k++)
					{
						this->m_object.map_label[target->mavi_label_index[j][k]]->SetFoldAngle(this->mf_angle_y);
					}
				}
			}
		}
		else {
			CInput* input = CSingleton<CInput>::GetInstance();
			if (input->GetKeyTrigger('E') || input->GetButtonTrigger(XI_BUTTON_Y)) {
				this->m_next_condition = FOLD_TURN_VIEW_END;
			}
		}
		break;
	case FOLD_TURN_VIEW_END:
		this->mf_angle_y += this->m_fold.mf_rotate_speed;

		if (this->mf_angle_y >= 360.0f) {
			this->mf_angle_y = 0.0f;
			this->m_next_condition = FOLD_NONE;
		}

		for (size_t i = 0; i < this->m_fold.ma_page_nest.size(); i++)
		{
			PageNest* target = &this->m_fold.ma_page_nest[i];

			target->mp_page->SetFoldAngle(this->mf_angle_y);
			for (size_t j = 0; j < target->maavp_chip.size(); j++)
			{
				for (size_t x = 0; x < target->maavp_chip[j].size(); x++)
				{
					for (size_t y = 0; y < target->maavp_chip[j][x].size(); y++)
					{
						target->maavp_chip[j][x][y]->SetFoldAngle(this->mf_angle_y);
					}
				}
			}
			for (size_t j = 0; j < target->map_player.size(); j++)
			{
				if (target->map_player[j] != nullptr) {
					target->map_player[j]->SetFoldAngle(this->mf_angle_y);
				}
			}
			for (size_t j = 0; j < target->mvp_ball.size(); j++)
			{
				for (size_t k = 0; k < target->mvp_ball[j].size(); k++)
				{
					target->mvp_ball[j][k]->SetFoldAngle(this->mf_angle_y);
				}
			}
			for (size_t j = 0; j < target->mvp_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < target->mvp_fixed_gimmick[j].size(); k++)
				{
					target->mvp_fixed_gimmick[j][k]->SetFoldAngle(this->mf_angle_y);
				}
			}
			for (size_t j = 0; j < target->mvp_un_fixed_gimmick.size(); j++)
			{
				for (size_t k = 0; k < target->mvp_un_fixed_gimmick[j].size(); k++)
				{
					target->mvp_un_fixed_gimmick[j][k]->SetFoldAngle(this->mf_angle_y);
				}
			}
			for (size_t j = 0; j < target->mavi_label_index.size(); j++)
			{
				for (size_t k = 0; k < target->mavi_label_index[j].size(); k++)
				{
					this->m_object.map_label[target->mavi_label_index[j][k]]->SetFoldAngle(this->mf_angle_y);
				}
			}
		}
		break;
	}
}

void CStageManager::ViewDraw()
{
	this->m_object.m_ui.mp_back_color_image->Draw();
	this->m_object.m_ui.mp_back_image->Draw();
	this->m_object.m_ui.mp_hp->Draw();
	this->m_object.m_ui.mp_piece_frame->Draw();

	for (size_t x = this->m_fold.mi_top_x; x <= this->m_fold.mi_tail_x; x++)
	{
		if (this->mf_angle_y >= 90.0f && this->mf_angle_y <= 270.0f) {
			int target_index = 0;
			for (size_t y = this->m_fold.mi_tail_y + 1; y >= this->m_fold.mi_top_y + 1; y--)
			{
				int index = this->m_fold.maai_page_index[x][y - 1];
				if (index == NONE_PAGE) continue;
				target_index = y - 1;
				break;
			}
			for (size_t y = this->m_fold.mi_top_y; y <= this->m_fold.mi_tail_y; y++)
			{
				int index = this->m_fold.maai_page_index[x][y];
				if (index == NONE_PAGE) continue;

				if (y == target_index) {
					FRONT_BACK front_back = this->m_fold.ma_page_nest[index].m_front_back;
					FRONT_BACK my_frontback = IS_FRONT;
					if (front_back == IS_FRONT) my_frontback = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}

					this->m_fold.ma_page_nest[index].mp_page->FakeFoldDraw();

					for (size_t z = 0; z < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback].size(); z++)
					{
						for (size_t w = 0; w < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z].size(); w++)
						{
							this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z][w]->FakeFoldDraw();
						}
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_ball[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}

					if (this->m_fold.ma_page_nest[index].map_player[my_frontback] != nullptr) {
						this->m_fold.ma_page_nest[index].map_player[my_frontback]->FakeFoldDraw();
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
				else {
					FRONT_BACK front_back = this->m_fold.ma_page_nest[index].m_front_back;
					FRONT_BACK my_frontback = IS_FRONT;
					if (front_back == IS_FRONT) my_frontback = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
			}
		}
		else {
			int target_index = 0;
			for (size_t y = this->m_fold.mi_top_y; y <= this->m_fold.mi_tail_y; y++)
			{
				int index = this->m_fold.maai_page_index[x][y];
				if (index == NONE_PAGE) continue;
				target_index = y;
				break;
			}
			for (size_t y = this->m_fold.mi_tail_y + 1; y >= this->m_fold.mi_top_y + 1; y--)
			{
				int index = this->m_fold.maai_page_index[x][y - 1];
				if (index == NONE_PAGE) continue;

				if (y == target_index + 1) {
					FRONT_BACK front_back = IS_FRONT;
					FRONT_BACK my_frontback = this->m_fold.ma_page_nest[index].m_front_back;
					if (my_frontback == IS_FRONT) front_back = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}

					this->m_fold.ma_page_nest[index].mp_page->FakeFoldDraw();

					for (size_t z = 0; z < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback].size(); z++)
					{
						for (size_t w = 0; w < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z].size(); w++)
						{
							this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z][w]->FakeFoldDraw();
						}
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_ball[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}

					if (this->m_fold.ma_page_nest[index].map_player[my_frontback] != nullptr) {
						this->m_fold.ma_page_nest[index].map_player[my_frontback]->FakeFoldDraw();
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
				else {
					FRONT_BACK front_back = IS_FRONT;
					FRONT_BACK my_frontback = this->m_fold.ma_page_nest[index].m_front_back;
					if (my_frontback == IS_FRONT) front_back = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
			}
		}
	}

	for (size_t i = 0; i < this->m_object.m_ui.map_get_piece.size(); i++) {
		this->m_object.m_ui.map_get_piece[i]->Draw();
	}
}

void CStageManager::ClearUpdate()
{
	switch (this->m_condition)
	{
	case FOLD_GAMECLEAR_PREPARE:
	{
		CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();

		this->mp_xaudio2->XA_SetFadeout(SOUND_LABEL_BGM_STAGE_1, 0.0f, 0.1f);
		this->mp_xaudio2->XA_SetFadeout(SOUND_LABEL_BGM_STAGE_2, 0.0f, 0.1f);
		this->mp_xaudio2->XA_SetFadeout(SOUND_LABEL_BGM_STAGE_3, 0.0f, 0.1f);

		float world = game_manager->GetWorldIndex();
		float stage = game_manager->GetStageIndex();

		if (world >= 2 && stage >= 2) {
			for (size_t i = 0; i < 3; i++)
			{
				for (size_t j = 0; j < SMB_MAX; j++)
				{
					CUnit* new_image = new CUnit;
					new_image->Initilize();

					Transform transform;

					switch (i)
					{
					case 0:
						transform = this->m_object.m_ui.mvp_result_button[j]->GetTransform();
						if (j == 0) transform.position.x = -332.0f + 172.0f + (427.0f * 0.5f);
						else
						{
							transform.position.x = -1000.0f;
							Translate new_pos = { -1000.0f, 0.0f, 0.0f };
							this->m_object.m_ui.mvp_result_button[j]->SetAxis(&new_pos);
						}
						this->m_object.m_ui.mvp_result_button[j]->SetTransform(&transform);
						break;
					case 1:
						transform = this->m_object.m_ui.mvp_result_frame[j]->GetTransform();
						if (j == 0) transform.position.x = -332.0f + 172.0f + (427.0f * 0.5f);
						else
						{
							transform.position.x = -1000.0f;
							Translate new_pos = { -1000.0f, 0.0f, 0.0f };
							this->m_object.m_ui.mvp_result_frame[j]->SetAxis(&new_pos);
						}
						this->m_object.m_ui.mvp_result_frame[j]->SetTransform(&transform);
						break;
					case 2:
						transform = this->m_object.m_ui.mvp_result_cursor[j]->GetTransform();
						if (j == 0) {
							transform.position.x = -332.0f + 172.0f + (427.0f * 0.5f);
							transform.position.x -= (172.0f + 25.0f + 10.0f);
						}
						else
						{
							transform.position.x = -1000.0f;
							Translate new_pos = { -1000.0f, 0.0f, 0.0f };
							this->m_object.m_ui.mvp_result_cursor[j]->SetAxis(&new_pos);
						}
						this->m_object.m_ui.mvp_result_cursor[j]->SetTransform(&transform);
						break;
					}
				}
			}
		}

		{
			UV new_uv = { 3.0f, 0.0f };
			this->m_object.m_ui.mvp_result_button[SMB_BUTTON_1]->SetAnim(&new_uv);
		}

		{
			UV new_uv = { 0.0f, 1.0f };
			this->m_object.m_ui.mvp_result_button[SMB_BUTTON_2]->SetAnim(&new_uv);
		}

		Translate new_axis = { 0.0f, 0.0f, 0.0f };
		float new_angle = 90.0f;

		this->m_object.m_ui.mp_clear_frame->SetFoldAngle(new_angle);
		this->m_object.m_ui.mp_clear_frame->SetAxis(&new_axis);
		this->m_object.m_ui.mp_clear_frame->SetNotRotate(false);

		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
		{
			if (world >= 2 && stage >= 2 && i != 0) continue;
			this->m_object.m_ui.mvp_result_frame[i]->SetFoldAngle(new_angle);
			this->m_object.m_ui.mvp_result_frame[i]->SetAxis(&new_axis);
			this->m_object.m_ui.mvp_result_frame[i]->SetNotRotate(false);
		}
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
		{
			if (world >= 2 && stage >= 2 && i != 0) continue;
			this->m_object.m_ui.mvp_result_button[i]->SetFoldAngle(new_angle);
			this->m_object.m_ui.mvp_result_button[i]->SetAxis(&new_axis);
			this->m_object.m_ui.mvp_result_button[i]->SetNotRotate(false);
		}
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
		{
			if (world >= 2 && stage >= 2 && i != 0) continue;
			this->m_object.m_ui.mvp_result_cursor[i]->SetFoldAngle(new_angle);
			this->m_object.m_ui.mvp_result_cursor[i]->SetAxis(&new_axis);
			this->m_object.m_ui.mvp_result_cursor[i]->SetNotRotate(false);
		}

		for (size_t i = 0; i < this->m_object.m_ui.map_result_get_piece.size(); i++)
		{
			if (game_manager->CheckGetPiece(i)) {
				this->m_object.m_ui.map_result_get_piece[i]->ChangeTexture(SPRITE_PIECE_1_com);
			}
			else {
				this->m_object.m_ui.map_result_get_piece[i]->ChangeTexture(SPRITE_PIECE_1_in);
			}

			this->m_object.m_ui.map_result_get_piece[i]->SetFoldAngle(new_angle);
			this->m_object.m_ui.map_result_get_piece[i]->SetAxis(&new_axis);
			this->m_object.m_ui.map_result_get_piece[i]->SetNotRotate(false);
		}
		for (size_t i = 0; i < this->m_object.m_ui.map_grand_get_piece.size(); i++)
		{
			if (game_manager->CheckGetPiece(i)) {
				this->m_object.m_ui.map_grand_get_piece[i]->ChangeTexture(SPRITE_PIECE_1_com);
			}
			else {
				this->m_object.m_ui.map_grand_get_piece[i]->ChangeTexture(SPRITE_PIECE_1_in);
			}

			this->m_object.m_ui.map_grand_get_piece[i]->SetFoldAngle(new_angle);
			this->m_object.m_ui.map_grand_get_piece[i]->SetAxis(&new_axis);
			this->m_object.m_ui.map_grand_get_piece[i]->SetNotRotate(false);
		}

		this->m_object.m_ui.mp_clear_book->SetFoldAngle(new_angle);
		this->m_object.m_ui.mp_clear_book->SetAxis(&new_axis);
		this->m_object.m_ui.mp_clear_book->SetNotRotate(true);

		Transform transform = this->m_object.m_ui.mp_clear_book->GetTransform();
		transform.scale.x = 2.0f;
		transform.scale.y = 2.0f;
		this->m_object.m_ui.mp_clear_book->SetTransform(&transform);

		this->mf_angle_y = 90.0f;

		this->mi_cursor_line = SMB_BUTTON_1;
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->mf_old_axis = 0.0f;
		game_manager->SavePiece();
		this->m_next_condition = FOLD_GAMECLEAR_ACT;
	}
	break;
	case FOLD_GAMECLEAR_ACT:
	{
		Color color = this->m_object.m_ui.mp_clear_book->GetColor();
		Color back_color = this->m_object.m_ui.mp_clear_frame->GetColor();
		Transform transform = this->m_object.m_ui.mp_clear_book->GetTransform();

		if (color.a < 1.0f) {
			color.a += 0.05f;
			transform.scale.x = 2.0f - color.a;
			transform.scale.y = 2.0f - color.a;

			if (color.a >= 1.0f) {
				color.a = 1.0f;
				transform.scale.x = 1.0f;
				transform.scale.y = 1.0f;

				this->mp_xaudio2->Start(SOUND_LABEL_SE_CLEAR);
				this->mp_xaudio2->Start(SOUND_LABEL_BGM_RESULT);
				this->mp_xaudio2->XA_Setvolume(SOUND_LABEL_BGM_RESULT, 0.0f);
				this->mp_xaudio2->XA_SetFadein(SOUND_LABEL_BGM_RESULT, 1.0f, 0.05f);
			}

			this->m_object.m_ui.mp_clear_book->SetColor(&color);
			this->m_object.m_ui.mp_clear_book->SetTransform(&transform);

			color.r = 0.0f;
			color.g = 0.0f;
			color.b = 0.0f;
			color.a /= 2.0f;
			this->m_object.m_ui.mp_fade->SetColor(&color);
		}
		else if (back_color.a < 1.0f) {
			back_color.a += 0.05f;
			if (back_color.a >= 1.0f) {
				back_color.a = 1.0f;
			}

			this->m_object.m_ui.mp_clear_frame->SetColor(&back_color);
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
			{
				this->m_object.m_ui.mvp_result_frame[i]->SetColor(&back_color);
			}
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
			{
				this->m_object.m_ui.mvp_result_button[i]->SetColor(&back_color);
			}
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
			{
				this->m_object.m_ui.mvp_result_cursor[i]->SetColor(&back_color);
			}
			for (size_t i = 0; i < this->m_object.m_ui.map_result_get_piece.size(); i++)
			{
				this->m_object.m_ui.map_result_get_piece[i]->SetColor(&back_color);
			}
			for (size_t i = 0; i < this->m_object.m_ui.map_grand_get_piece.size(); i++)
			{
				this->m_object.m_ui.map_grand_get_piece[i]->SetColor(&back_color);
			}
		}
		else {
			this->mf_angle_y -= 3.0f;
			if (this->mf_angle_y <= 0.0f) {
				this->mf_angle_y = 0.0f;

				this->m_next_condition = FOLD_GAMECLEAR_END;
			}

			this->m_object.m_ui.mp_clear_frame->SetFoldAngle(this->mf_angle_y);
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
			{
				this->m_object.m_ui.mvp_result_frame[i]->SetFoldAngle(this->mf_angle_y);
			}
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
			{
				this->m_object.m_ui.mvp_result_button[i]->SetFoldAngle(this->mf_angle_y);
			}
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
			{
				this->m_object.m_ui.mvp_result_cursor[i]->SetFoldAngle(this->mf_angle_y);
			}
			for (size_t i = 0; i < this->m_object.m_ui.map_result_get_piece.size(); i++)
			{
				this->m_object.m_ui.map_result_get_piece[i]->SetFoldAngle(this->mf_angle_y);
			}
			for (size_t i = 0; i < this->m_object.m_ui.map_grand_get_piece.size(); i++)
			{
				this->m_object.m_ui.map_grand_get_piece[i]->SetFoldAngle(this->mf_angle_y);
			}
			this->m_object.m_ui.mp_clear_book->SetFoldAngle(this->mf_angle_y);
		}

		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
		{
			this->m_object.m_ui.mvp_result_frame[i]->SetColor(&new_color);
		}
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
		{
			this->m_object.m_ui.mvp_result_cursor[i]->SetColor(&new_color);
		}

		new_color.a = 1.0f;

		this->m_object.m_ui.mvp_result_frame[this->mi_cursor_line]->SetColor(&new_color);
		this->m_object.m_ui.mvp_result_cursor[this->mi_cursor_line]->SetColor(&new_color);

	}
	break;
	case FOLD_GAMECLEAR_END:
	{
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
		{
			this->m_object.m_ui.mvp_result_cursor[i]->SetColor(&new_color);
		}
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
		{
			this->m_object.m_ui.mvp_result_frame[i]->SetColor(&new_color);
		}

		CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();
		CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
		CInput* input = CSingleton<CInput>::GetInstance();

		float axis = input->GetAxisRawX(true);
		float check_axis = 0.0f;
		if (this->mf_old_axis != axis) check_axis = axis;
		if (input->GetKeyTrigger('A') || input->GetButtonTrigger(XI_BUTTON_LEFT) || check_axis < 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line--;
			if (this->mi_cursor_line < 0) {
				this->mi_cursor_line = SMB_MAX - 1;
			}
		}
		else if (input->GetKeyTrigger('D') || input->GetButtonTrigger(XI_BUTTON_RIGHT) || check_axis > 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line++;
			this->mi_cursor_line %= SMB_MAX;
		}

		CCursor* cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = cursor->GetConvertedCollider();
			Collider other_collider = this->m_object.m_ui.mvp_result_button[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_line = static_cast<STAGE_MANAGER_BUTTON>(i);
				break;
			}
		}

		float world = game_manager->GetWorldIndex();
		float stage = game_manager->GetStageIndex();

		if (world >= 2 && stage >= 2) {
			this->mi_cursor_line = SMB_BUTTON_1;
		}

		if (input->GetKeyTrigger(VK_LBUTTON) || input->GetButtonTrigger(XI_BUTTON_B)) {
			this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
			if (this->mi_cursor_line == SMB_BUTTON_1) {
				scene_manager->ChangeScene(SCENE_STAGE_SELECT);
			}
			if (this->mi_cursor_line == SMB_BUTTON_2) {
				int world = game_manager->GetWorldIndex();
				int stage = game_manager->GetStageIndex();
				stage++;
				if (stage >= GAME_WORLD_MAX) {
					stage = 0;
					world++;
				}
				if (world >= GAME_WORLD_MAX) {
					world = 0;
				}
				game_manager->SetWorldIndex(world);
				game_manager->SetStageIndex(stage);
				scene_manager->ChangeScene(SCENE_STAGE_PLAY);
			}
		}
		this->mf_old_axis = axis;

		if (this->mi_old_cursor_line != this->mi_cursor_line) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_MOVE);
		}

		new_color.a = 1.0f;
		this->m_object.m_ui.mvp_result_cursor[this->mi_cursor_line]->SetColor(&new_color);
		this->m_object.m_ui.mvp_result_frame[this->mi_cursor_line]->SetColor(&new_color);
	}
	break;
	}
}

void CStageManager::ClearDraw()
{
	this->NormalDraw();

	this->m_object.m_ui.mp_clear_frame->FakeFoldDraw();
	for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
	{
		this->m_object.m_ui.mvp_result_frame[i]->FakeFoldDraw();
	}
	for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
	{
		this->m_object.m_ui.mvp_result_button[i]->FakeFoldDraw();
	}
	for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
	{
		this->m_object.m_ui.mvp_result_cursor[i]->FakeFoldDraw();
	}

	for (size_t i = 0; i < this->m_object.m_ui.map_result_get_piece.size(); i++)
	{
		this->m_object.m_ui.map_result_get_piece[i]->FakeFoldDraw();
	}
	for (size_t i = 0; i < this->m_object.m_ui.map_grand_get_piece.size(); i++)
	{
		this->m_object.m_ui.map_grand_get_piece[i]->FakeFoldDraw();
	}

	this->m_object.m_ui.mp_clear_book->FakeFoldDraw();
}

void CStageManager::OverUpdate()
{
	switch (this->m_condition)
	{
	case FOLD_GAMEOVER_PREPARE:
	{
		this->mp_xaudio2->XA_SetFadeout(SOUND_LABEL_BGM_STAGE_1, 0.0f, 0.05f);
		this->mp_xaudio2->XA_SetFadeout(SOUND_LABEL_BGM_STAGE_2, 0.0f, 0.05f);
		this->mp_xaudio2->XA_SetFadeout(SOUND_LABEL_BGM_STAGE_3, 0.0f, 0.05f);

		{
			UV new_uv = { 3.0f, 0.0f };
			this->m_object.m_ui.mvp_result_button[SMB_BUTTON_1]->SetAnim(&new_uv);
		}

		{
			UV new_uv = { 1.0f, 1.0f };
			this->m_object.m_ui.mvp_result_button[SMB_BUTTON_2]->SetAnim(&new_uv);
		}

		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
		{
			Transform transform = this->m_object.m_ui.mvp_result_frame[i]->GetTransform();
			transform.position.x = -18.0f - 382.0f + 172.0f + (476.0f * static_cast<float>(i));
			transform.position.y = -32.0f - 220.0f + 37.0f;
			this->m_object.m_ui.mvp_result_frame[i]->SetTransform(&transform);
		}
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
		{
			Transform transform = this->m_object.m_ui.mvp_result_button[i]->GetTransform();
			transform.position.x = -18.0f - 382.0f + 172.0f + (476.0f * static_cast<float>(i));
			transform.position.y = -32.0f - 220.0f + 37.0f;
			this->m_object.m_ui.mvp_result_button[i]->SetTransform(&transform);
		}
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
		{
			Transform transform = this->m_object.m_ui.mvp_result_cursor[i]->GetTransform();
			transform.position.x = -18.0f - 382.0f - 25.0f - 10.0f + (476.0f * static_cast<float>(i));
			transform.position.y = -32.0f - 220.0f + 37.0f;
			this->m_object.m_ui.mvp_result_cursor[i]->SetTransform(&transform);
		}


		this->mi_cursor_line = SMB_BUTTON_1;
		this->mi_old_cursor_line = this->mi_cursor_line;
		this->mf_old_axis = 0.0f;
		this->m_next_condition = FOLD_GAMEOVER_ACT;
	}
	break;
	case FOLD_GAMEOVER_ACT:
		if (this->m_object.mp_player->CheckFinishGameOver()) {
			Color color = this->m_object.m_ui.mp_over_back->GetColor();

			color.a += 0.05f;

			if (color.a >= 1.0f) {
				color.a = 1.0f;
				this->m_next_condition = FOLD_GAMEOVER_END;
			}

			this->m_object.m_ui.mp_over_back->SetColor(&color);
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
			{
				this->m_object.m_ui.mvp_result_button[i]->SetColor(&color);
			}

			Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };

			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
			{
				this->m_object.m_ui.mvp_result_frame[i]->SetColor(&new_color);
			}
			for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
			{
				this->m_object.m_ui.mvp_result_cursor[i]->SetColor(&new_color);
			}
			this->m_object.m_ui.mvp_result_cursor[this->mi_cursor_line]->SetColor(&color);
			this->m_object.m_ui.mvp_result_frame[this->mi_cursor_line]->SetColor(&color);

			color.r = 0.0f;
			color.g = 0.0f;
			color.b = 0.0f;
			color.a /= 2.0f;
			this->m_object.m_ui.mp_fade->SetColor(&color);
		}
		else {
			this->m_object.mp_player->Animation();
			if (this->m_object.mp_player->CheckFinishGameOver()) {
				this->mp_xaudio2->Start(SOUND_LABEL_SE_GAMEOVER);
				//this->mp_xaudio2->XA_Setvolume(SOUND_LABEL_BGM_GAMEOVER, 0.0f);
				//this->mp_xaudio2->XA_SetFadein(SOUND_LABEL_BGM_GAMEOVER, 1.0f, 0.05f);
			}
		}
		break;
	case FOLD_GAMEOVER_END:
	{
		Color new_color = { 1.0f, 1.0f, 1.0f, 0.0f };
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
		{
			this->m_object.m_ui.mvp_result_cursor[i]->SetColor(&new_color);
		}
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
		{
			this->m_object.m_ui.mvp_result_frame[i]->SetColor(&new_color);
		}

		CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
		CInput* input = CSingleton<CInput>::GetInstance();

		float axis = input->GetAxisRawX(true);
		float check_axis = 0.0f;
		if (this->mf_old_axis != axis) check_axis = axis;
		if (input->GetKeyTrigger('A') || input->GetButtonTrigger(XI_BUTTON_LEFT) || check_axis < 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line--;
			if (this->mi_cursor_line < 0) {
				this->mi_cursor_line = SMB_MAX - 1;
			}
		}
		else if (input->GetKeyTrigger('D') || input->GetButtonTrigger(XI_BUTTON_RIGHT) || check_axis > 0.0f) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mi_cursor_line++;
			this->mi_cursor_line %= SMB_MAX;
		}
		else if (input->GetKeyTrigger(VK_LBUTTON) || input->GetButtonTrigger(XI_BUTTON_B)) {
			this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_DECISION);
			if (this->mi_cursor_line == SMB_BUTTON_1) {
				scene_manager->ChangeScene(SCENE_STAGE_SELECT);
			}
			if (this->mi_cursor_line == SMB_BUTTON_2) {
				scene_manager->ChangeScene(SCENE_STAGE_PLAY);
			}
		}
		this->mf_old_axis = axis;

		CCursor* cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();
		for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
		{
			// それぞれのコライダーを取得
			Collider obj_collider = cursor->GetConvertedCollider();
			Collider other_collider = this->m_object.m_ui.mvp_result_button[i]->GetConvertedCollider();

			// 当たり判定結果
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true のとき当たってます。
			if (hit.is_hit) {
				this->mi_cursor_line = static_cast<STAGE_MANAGER_BUTTON>(i);
				break;
			}
		}

		if (this->mi_old_cursor_line != this->mi_cursor_line) {
			this->mi_old_cursor_line = this->mi_cursor_line;
			this->mp_xaudio2->Start(SOUND_LABEL_SE_CURSOR_MOVE);
		}

		new_color.a = 1.0f;
		this->m_object.m_ui.mvp_result_cursor[this->mi_cursor_line]->SetColor(&new_color);
		this->m_object.m_ui.mvp_result_frame[this->mi_cursor_line]->SetColor(&new_color);
	}
	break;
	}
}

void CStageManager::OverDraw()
{
	this->NormalDraw();
	this->m_object.m_ui.mp_over_back->Draw();
	for (size_t i = 0; i < this->m_object.m_ui.mvp_result_frame.size(); i++)
	{
		this->m_object.m_ui.mvp_result_frame[i]->Draw();
	}
	for (size_t i = 0; i < this->m_object.m_ui.mvp_result_button.size(); i++)
	{
		this->m_object.m_ui.mvp_result_button[i]->Draw();
	}
	for (size_t i = 0; i < this->m_object.m_ui.mvp_result_cursor.size(); i++)
	{
		this->m_object.m_ui.mvp_result_cursor[i]->Draw();
	}
}

void CStageManager::BasePageDraw()
{
	// ベースの描画
	for (size_t x = 0; x < this->m_fold.m_data.mvvi_base.size(); x++)
	{
		for (size_t y = this->m_fold.m_data.mvvi_base[x].size(); y >= 1; y--)
		{
			int index = this->m_fold.m_data.mvvi_base[x][y - 1];
			if (index == NONE_PAGE) continue;

			if (y == 1) {
				FRONT_BACK front_back = IS_FRONT;
				if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) front_back = IS_BACK;

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->Draw();
				}

				this->m_fold.ma_page_nest[index].mp_page->Draw();

				for (size_t z = 0; z < this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back].size(); z++)
				{
					for (size_t w = 0; w < this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back][z].size(); w++)
					{
						this->m_fold.ma_page_nest[index].maavp_chip[this->m_fold.ma_page_nest[index].m_front_back][z][w]->Draw();
					}
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_fold.ma_page_nest[index].mvp_ball[this->m_fold.ma_page_nest[index].m_front_back][i]->FakeFoldDraw();
				}
				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back][i]->FakeFoldDraw();
				}
				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[this->m_fold.ma_page_nest[index].m_front_back][i]->FakeFoldDraw();
				}

				if (this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back] != nullptr) {
					this->m_fold.ma_page_nest[index].map_player[this->m_fold.ma_page_nest[index].m_front_back]->Draw();
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back][i]]->Draw();
				}
			}
			else {
				FRONT_BACK front_back = IS_FRONT;
				if (this->m_fold.ma_page_nest[index].m_front_back == IS_FRONT) front_back = IS_BACK;

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->Draw();
				}
				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[this->m_fold.ma_page_nest[index].m_front_back][i]]->Draw();
				}
			}
		}
	}
}

void CStageManager::FoldPageDraw()
{
	// 折り部分の描画
	for (size_t x = 0; x < this->m_fold.m_data.mvvi_fold.size(); x++)
	{
		if (this->mf_angle_y > 90.0f || this->mf_angle_y < -90.0f) {
			for (size_t y = 0; y < this->m_fold.m_data.mvvi_fold[x].size(); y++)
			{
				int index = this->m_fold.m_data.mvvi_fold[x][y];
				if (index == NONE_PAGE) continue;

				if (y == this->m_fold.m_data.mvvi_fold[x].size() - 1) {
					FRONT_BACK front_back = this->m_fold.ma_page_nest[index].m_front_back;
					FRONT_BACK my_frontback = IS_FRONT;
					if (front_back == IS_FRONT) my_frontback = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}

					this->m_fold.ma_page_nest[index].mp_page->FakeFoldDraw();

					for (size_t z = 0; z < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback].size(); z++)
					{
						for (size_t w = 0; w < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z].size(); w++)
						{
							this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z][w]->FakeFoldDraw();
						}
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_ball[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}

					if (this->m_fold.ma_page_nest[index].map_player[my_frontback] != nullptr) {
						this->m_fold.ma_page_nest[index].map_player[my_frontback]->FakeFoldDraw();
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
				else {
					FRONT_BACK front_back = this->m_fold.ma_page_nest[index].m_front_back;
					FRONT_BACK my_frontback = IS_FRONT;
					if (front_back == IS_FRONT) my_frontback = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
			}
		}
		else {
			for (size_t y = this->m_fold.m_data.mvvi_fold[x].size(); y >= 1; y--)
			{
				int index = this->m_fold.m_data.mvvi_fold[x][y - 1];
				if (index == NONE_PAGE) continue;

				if (y == 1) {
					FRONT_BACK front_back = IS_FRONT;
					FRONT_BACK my_frontback = this->m_fold.ma_page_nest[index].m_front_back;
					if (my_frontback == IS_FRONT) front_back = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}

					this->m_fold.ma_page_nest[index].mp_page->FakeFoldDraw();

					for (size_t z = 0; z < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback].size(); z++)
					{
						for (size_t w = 0; w < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z].size(); w++)
						{
							this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z][w]->FakeFoldDraw();
						}
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_ball[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback].size(); i++)
					{
						this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
					}

					if (this->m_fold.ma_page_nest[index].map_player[my_frontback] != nullptr) {
						this->m_fold.ma_page_nest[index].map_player[my_frontback]->FakeFoldDraw();
					}

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
				else {
					FRONT_BACK front_back = IS_FRONT;
					FRONT_BACK my_frontback = this->m_fold.ma_page_nest[index].m_front_back;
					if (my_frontback == IS_FRONT) front_back = IS_BACK;

					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
					}
					for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
					{
						this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
					}
				}
			}
		}
	}
}

void CStageManager::FrontPageDraw()
{
	// 前面部分の描画
	for (size_t x = 0; x < this->m_fold.m_data.mvvi_front.size(); x++)
	{
		for (size_t y = this->m_fold.m_data.mvvi_front[x].size(); y >= 1; y--)
		{
			int index = this->m_fold.m_data.mvvi_front[x][y - 1];
			if (index == NONE_PAGE) continue;

			if (y == 1) {
				FRONT_BACK front_back = IS_FRONT;
				FRONT_BACK my_frontback = this->m_fold.ma_page_nest[index].m_front_back;
				if (my_frontback == IS_FRONT) front_back = IS_BACK;

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
				}

				this->m_fold.ma_page_nest[index].mp_page->FakeFoldDraw();

				for (size_t z = 0; z < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback].size(); z++)
				{
					for (size_t w = 0; w < this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z].size(); w++)
					{
						this->m_fold.ma_page_nest[index].maavp_chip[my_frontback][z][w]->FakeFoldDraw();
					}
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_ball[my_frontback].size(); i++)
				{
					this->m_fold.ma_page_nest[index].mvp_ball[my_frontback][i]->FakeFoldDraw();
				}
				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback].size(); i++)
				{
					this->m_fold.ma_page_nest[index].mvp_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
				}
				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback].size(); i++)
				{
					this->m_fold.ma_page_nest[index].mvp_un_fixed_gimmick[my_frontback][i]->FakeFoldDraw();
				}

				if (this->m_fold.ma_page_nest[index].map_player[my_frontback] != nullptr) {
					this->m_fold.ma_page_nest[index].map_player[my_frontback]->FakeFoldDraw();
				}

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
				}
			}
			else {
				FRONT_BACK front_back = IS_FRONT;
				FRONT_BACK my_frontback = this->m_fold.ma_page_nest[index].m_front_back;
				if (my_frontback == IS_FRONT) front_back = IS_BACK;

				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[front_back].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[front_back][i]]->FakeFoldDraw();
				}
				for (size_t i = 0; i < this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback].size(); i++)
				{
					this->m_object.map_label[this->m_fold.ma_page_nest[index].mavi_label_index[my_frontback][i]]->FakeFoldDraw();
				}
			}
		}
	}
}
