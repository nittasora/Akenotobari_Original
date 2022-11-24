#include "CStageEditor.h"

#include <array>
#include <fstream>
#include <iostream>
#include <string>

#include "CDirect3D.h"
#include "CInput.h"
#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"

#include "StageData.h"

#include "CImage.h"
#include "CButton.h"
#include "CCursor.h"
#include "CPage.h"
#include "CPlayer.h"
#include "CEditorText.h"
#include "CInputParameter.h"

#include "CSingleton.h"

namespace
{
	// ���C����ʂ̃{�^���̖���
	enum STAGE_EDITOR_BUTTON
	{
		SEB_SAVE,				// �X�e�[�W�ۑ�
		SEB_BACK,				// �I����ʂ�
		SEB_WORLD_LEFT,			// ���[���h�ԍ�--
		SEB_WORLD_MAX_LEFT,		// �ő像�[���h�ԍ�--
		SEB_WORLD_RIGHT,		// ���[���h�ԍ�++
		SEB_WORLD_MAX_RIGHT,	// �ő像�[���h�ԍ�++
		SEB_STAGE_LEFT,			// �X�e�[�W�ԍ�--
		SEB_STAGE_MAX_LEFT,		// �ő�X�e�[�W�ԍ�--
		SEB_STAGE_RIGHT,		// �X�e�[�W�ԍ�++
		SEB_STAGE_MAX_RIGHT,	// �ő�X�e�[�W�ԍ�++
		SEB_ADD_SPRITE,			// �摜�ǉ�
		SEB_SPRITE_OPTION,		// �摜�ݒ�
		SEB_MAPCHIP,			// �}�b�v�`�b�v�I��
		SEB_GOAL,				// �S�[���I�� -> �M�~�b�N�����ɂ��p�~
		SEB_BACK_PAGE,			// �X�e�[�W�̔w�i�ݒ�
		SEB_BACK_OBJECT,		// �X�e�[�W�̔w�i�I�u�W�F�N�g�I�� -> �d�l�ύX�ɂ��p�~
		SEB_PLAYER,				// �v���C���[�̐ݒ�
		SEB_GIMMICK,			// �M�~�b�N�̑I��
		SEB_PIECE,				// �J�P���̐ݒ� -> �M�~�b�N�����ɂ��p�~
		SEB_FRONT_BACK,			// �X�e�[�W�𗠕Ԃ�
		SEB_ROTATE_RIGHT,		// �A�C�e�����E��90����]
		SEB_ROTATE_LEFT,		// �A�C�e��������90����]
		SEB_PAINT,				// �ǉ����[�h
		SEB_ERASE,				// �폜���[�h
		SEB_MAX
	};

	// �I����ʂ̃{�^���̖���
	enum SPRITE_SELECT_BUTTON
	{
		SSB_BACK,				// ���C����ʂɖ߂�
		SSB_MAX
	};

	// �����̖���
	enum VELUE_TEXT
	{
		VT_WORLD_10,			// ���[���h�ԍ���10�̈�
		VT_WORLD_01,			// ���[���h�ԍ���1�̈�
		VT_WORLD_MAX_10,		// �ő像�[���h�ԍ���10�̈�
		VT_WORLD_MAX_01,		// �ő像�[���h�ԍ���1�̈�
		VT_STAGE_10,			// �X�e�[�W�ԍ���10�̈�
		VT_STAGE_01,			// �X�e�[�W�ԍ���1�̈�
		VT_STAGE_MAX_10,		// �ő�X�e�[�W�ԍ���10�̈�
		VT_STAGE_MAX_01,		// �ő�X�e�[�W�ԍ���1�̈�
		VT_MAX
	};

	// �`�惂�[�h
	enum EDIT_VIEW
	{
		EV_NONE = 0,			// �ʏ�
		EV_SPRITE_SELECT,		// �摜�I��
		EV_GIMMIK_SELECT,		// �M�~�b�N�I��
		EV_MAX
	};

	// �G�f�B�b�g���[�h
	enum EDIT_MODE
	{
		EM_NONE = 0,			// ����
		EM_SPRITE_OPTION,		// �摜�ݒ�
		EM_MAPCHIP_SELECT,		// �}�b�v�`�b�v�I��
		EM_MAPCHIP,				// �}�b�v�`�b�v�z�u
		EM_BACK_SELECT,			// �w�i�I��
		EM_GIMMIK,				// �M�~�b�N�z�u
		EM_GIMMIK_SELECT,		// �M�~�b�N�I��
		EM_MAX
	};

	// �M�~�b�N�I��p
	enum GIMMIK_SELECT_IMAGE
	{
		GSI_GOAL,				// �S�[��
		GSI_PIECE_1,			// �J�P��1��
		GSI_PIECE_2,			// �J�P��2��
		GSI_PIECE_3,			// �J�P��3��
		GSI_RED_BLOCK,			// �ԃu���b�N
		GSI_BLUE_BLOCK,			// �u���b�N
		GSI_TORNADO,			// ����
		GSI_DHARMA,				// �_���}
		GSI_CANNON,				// ��C
		GSI_LION_TOP,			// ���t�g��
		GSI_LION_BOTTOM,		//���t�g��
		GSI_MAX
	};

	// ���[���h�E�X�e�[�W�ő吔
	constexpr int WORLD_MAX = 25;
	constexpr int STAGE_MAX = 25;

	// �f�t�H���g�̃}�b�v�`�b�v�Ȃǂ̉摜�ԍ�
	constexpr int DEFAULT_CHIP_SPRITE = SPRITE_TEST_3;
	constexpr int DEFAULT_FRONT_SPRITE = SPRITE_TEST_2;
	constexpr int DEFAULT_BACK_SPRITE = SPRITE_TEST_1;

	// �f�t�H���g�̃}�b�v�`�b�v�Ȃǂ̉摜
	SpriteData g_default_chip;
	SpriteData g_default_front;
	SpriteData g_default_back;

	// �摜�f�[�^
	std::vector <IO_SpriteDataEX> gv_io_sprite_data;
	std::vector <SpriteData> gv_sprite_data;

	// �X�e�[�W�̃f�[�^
	std::array<std::array<StageData, STAGE_MAX>, WORLD_MAX> ga_stage_data;

	// "WORLD"�ɂȂ�UV
	std::array<UV, 5> ga_world_text = {
		2.0f, 2.0f,
		14.0f, 1.0f,
		17.0f, 1.0f,
		11.0f, 1.0f,
		3.0f, 1.0f
	};

	// "STAGE"�ɂȂ�UV
	std::array<UV, 5> ga_stage_text = {
		18.0f, 1.0f,
		19.0f, 1.0f,
		0.0f, 1.0f,
		6.0f, 1.0f,
		4.0f, 1.0f
	};

	// " MAX "�ɂȂ�UV
	std::array<UV, 5> ga_max_text = {
		19.0f, 5.0f,
		12.0f, 1.0f,
		0.0f, 1.0f,
		3.0f, 2.0f,
		19.0f, 5.0f
	};

	// ���݂̃��[���h�E�X�e�[�W�ԍ�
	int gi_world_index;
	int gi_stage_index;

	// ���݂̃��[���h�E�X�e�[�W�ő�l
	int gi_world_max;
	int gi_stage_max;

	// �\��������������
	bool gb_is_front;

	// �`��E�G�f�B�b�g���[�h
	EDIT_VIEW g_view_mode;
	EDIT_MODE g_edit_mode;

	// �z�u����}�b�v�`�b�v�̃f�[�^
	MapChipData g_chip_brush;
}

CStageEditor::CStageEditor()
{
}

CStageEditor::~CStageEditor()
{
}

void CStageEditor::Initialize()
{
	// ���[�h�͒ʏ�
	g_view_mode = EV_NONE;
	g_edit_mode = EM_NONE;

	// �z�u�͂Ȃ�
	g_chip_brush.mi_chip_id = MAPCHIP_SPRITE_NONE;
	g_chip_brush.mf_angle_z = 0.0f;
	g_chip_brush.m_tag = TAG_MAPCHIP;

	// �ꉞ
	this->mvp_images.clear();
	this->mvp_main_buttons.clear();

	// CDirect3D�擾
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// �X�N���[���T�C�Y�i�[
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	// �w�i�摜�̒ǉ�
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CImage* new_image = new CImage;
		new_image->Initilize();

		// �e�N�X�`���ǉ�
		new_image->AddTexture(SPRITE_TEST_2);
		Color image_color = { 0.0f, 0.0f, 0.0f, 1.0f };
		new_image->SetColor(&image_color);

		// �摜�̑傫����ς���
		new_image->SetDefaultPosAndColl(screen_width, screen_height);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_images.emplace_back(new_image);
	}

	// �}�b�v�`�b�v�摜�I�u�W�F�N�g�̐���
	for (int y = 0; y < MAPCHIP_NUM_HEIGHT; y++) {
		for (int x = 0; x < MAPCHIP_NUM_WIDTH * AREA_NUM; x++) {
			// �摜�p�I�u�W�F�N�g�̐���
			CImage* new_image = new CImage;
			new_image->Initilize();

			// �e�N�X�`���ǉ�
			new_image->AddTexture(SPRITE_TEST_3);

			// �摜�̑傫����ς���
			new_image->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

			// ���W�ύX
			Transform transform = new_image->GetTransform();
			transform.position.x = MAPCHIP_WIDTH * (-(static_cast<float>(MAPCHIP_NUM_WIDTH) * static_cast<float>(AREA_NUM)) / 2.0f + 0.5f + static_cast<float>(x));
			transform.position.y = MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f + 0.5f + static_cast<float>(y));
			new_image->SetTransform(&transform);

			// �摜�I�u�W�F�N�g�̒ǉ�
			this->mvp_mapchip.emplace_back(new_image);
		}
	}

	// "WORLD"��������
	for (size_t i = 0; i < ga_world_text.size(); i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// �e�N�X�`���ǉ�
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_world_text[i]);

		// ���W�ύX
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - 10.0f;
		new_image->SetTransform(&transform);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_text.emplace_back(new_image);
	}

	// " MAX "��������
	for (size_t i = 0; i < ga_max_text.size(); i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// �e�N�X�`���ǉ�
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_max_text[i]);

		// ���W�ύX
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 1.5f) - 20.0f;
		new_image->SetTransform(&transform);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_text.emplace_back(new_image);
	}

	// "STAGE"��������
	for (size_t i = 0; i < ga_stage_text.size(); i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// �e�N�X�`���ǉ�
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_stage_text[i]);

		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 5.0f) + (BUTTON_WIDTH * 4.0f) + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - 10.0f;
		new_image->SetTransform(&transform);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_text.emplace_back(new_image);
	}

	// " MAX "��������
	for (size_t i = 0; i < ga_max_text.size(); i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CEditorText* new_image = new CEditorText;
		new_image->Initilize();

		// �e�N�X�`���ǉ�
		new_image->SetSprite(SPRITE_TEST_CHAR_0, ga_max_text[i]);

		// ���W�ύX
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 5.0f) + (BUTTON_WIDTH * 4.0f) + (CHAR_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 1.5f) - 20.0f;
		new_image->SetTransform(&transform);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_text.emplace_back(new_image);
	}

	// �X�e�[�W�w�i�摜�̒ǉ�
	for (int i = 0; i < AREA_NUM; i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CPage* new_image = new CPage;
		new_image->Initilize();

		// �e�N�X�`���ǉ�
		new_image->SetSprite(SPRITE_TEST_2, SPRITE_TEST_1);
		new_image->SetDefaultUV(i);

		// ���W�ύX
		Transform transform = new_image->GetTransform();
		transform.position.x = MAPCHIP_WIDTH * static_cast<float>(MAPCHIP_NUM_WIDTH) * (-3.0f + static_cast<float>(i));
		new_image->SetTransform(&transform);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_pages.emplace_back(new_image);
	}

	// �{�^�������炩���ߗp��
	this->mvp_main_buttons.resize(SEB_MAX);
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		// �{�^���I�u�W�F�N�g����
		this->mvp_main_buttons[i] = new CButton;
		this->mvp_main_buttons[i]->Initilize();

		// �{�^���̃T�C�Y
		float width = BUTTON_W_WIDTH;
		float height = BUTTON_W_HEIGHT;

		// �{�^���̑傫����ς���
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(width, height);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_main_buttons[i]->SetButton(VK_LBUTTON, XI_BUTTON_B);
	}

	// �{�^����p�̃e�N�X�`���ݒ�֐�(�ۑ��A��߂�)
	this->mvp_main_buttons[SEB_SAVE]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 4.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_BACK]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 3.0f, 0.0f }, true);

	// �{�^����p�̃e�N�X�`���ݒ�֐�(���[���h�֌W�E���E)
	this->mvp_main_buttons[SEB_WORLD_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_WORLD_MAX_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_WORLD_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_WORLD_MAX_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);

	// �{�^����p�̃e�N�X�`���ݒ�֐�(�X�e�[�W�֌W�E���E)
	this->mvp_main_buttons[SEB_STAGE_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_STAGE_MAX_LEFT]->SetSprite(SPRITE_TEST_BUTTON_0, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_STAGE_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_STAGE_MAX_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_0, { 2.0f, 4.0f }, true);

	// �{�^����p�̃e�N�X�`���ݒ�֐�(���[�h�ύX�֌W)
	this->mvp_main_buttons[SEB_ADD_SPRITE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 3.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_SPRITE_OPTION]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 0.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_MAPCHIP]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 0.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_GOAL]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 3.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_BACK_PAGE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 0.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_BACK_OBJECT]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 1.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_PLAYER]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 4.0f, 0.0f }, true);
	this->mvp_main_buttons[SEB_GIMMICK]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 2.0f, 2.0f }, true);
	this->mvp_main_buttons[SEB_PIECE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 2.0f, 0.0f }, true);

	// �{�^����p�̃e�N�X�`���ݒ�֐�(�G�f�B�b�g�⏕�֌W)
	this->mvp_main_buttons[SEB_FRONT_BACK]->SetSprite(SPRITE_TEST_BUTTON_0, { 9.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_ROTATE_RIGHT]->SetSprite(SPRITE_TEST_BUTTON_1, { 0.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_ROTATE_LEFT]->SetSprite(SPRITE_TEST_BUTTON_1, { 1.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_PAINT]->SetSprite(SPRITE_TEST_BUTTON_0, { 6.0f, 4.0f }, true);
	this->mvp_main_buttons[SEB_ERASE]->SetSprite(SPRITE_TEST_BUTTON_0, { 7.0f, 4.0f }, true);

	// SEB_PAINT �� SEB_ERASE �̓��W�I�{�^����
	this->mvp_main_buttons[SEB_PAINT]->SetOtherButtonObject(this->mvp_main_buttons[SEB_ERASE]);
	this->mvp_main_buttons[SEB_ERASE]->SetOtherButtonObject(this->mvp_main_buttons[SEB_PAINT]);

	// ������PAINT���[�h
	this->mvp_main_buttons[SEB_PAINT]->SetONOFF(true);

	// �{�^���̈ʒu���킹
	for (int i = SEB_SAVE; i <= SEB_BACK; i++)
	{
		// ���W�ύX
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 0.5f) + 10.0f; //((10.0f) * static_cast<float>(i + 1)) + (BUTTON_W_WIDTH * static_cast<float>(i));
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// �{�^���̈ʒu���킹
	for (int i = SEB_WORLD_LEFT; i <= SEB_WORLD_MAX_LEFT; i++)
	{
		// �傫����ς���
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// ���W�ύX
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 4.0f) + (BUTTON_WIDTH);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_WORLD_LEFT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_WORLD_LEFT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// �{�^���̈ʒu���킹
	for (int i = SEB_WORLD_RIGHT; i <= SEB_WORLD_MAX_RIGHT; i++)
	{
		// �傫����ς���
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// ���W�ύX
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 5.0f) + (BUTTON_WIDTH * 3.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_WORLD_RIGHT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_WORLD_RIGHT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// �{�^���̈ʒu���킹
	for (int i = SEB_STAGE_LEFT; i <= SEB_STAGE_MAX_LEFT; i++)
	{
		// �傫����ς���
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// ���W�ύX
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 9.0f) + (BUTTON_WIDTH * 5.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_STAGE_LEFT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_STAGE_LEFT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// �{�^���̈ʒu���킹
	for (int i = SEB_STAGE_RIGHT; i <= SEB_STAGE_MAX_RIGHT; i++)
	{
		// �傫����ς���
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// ���W�ύX
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 10.0f) + (BUTTON_WIDTH * 7.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - ((10.0f) * static_cast<float>(i - SEB_STAGE_RIGHT + 1)) - (BUTTON_W_HEIGHT * static_cast<float>(i - SEB_STAGE_RIGHT));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// �{�^���̈ʒu���킹
	for (int i = SEB_ADD_SPRITE; i <= SEB_PIECE; i++)
	{
		float width = static_cast<float>((i - SEB_ADD_SPRITE) % 6);
		float height = static_cast<float>((i - SEB_ADD_SPRITE) / 6);

		// ���W�ύX
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * (2.5f + width)) + (10.0f * (width + 1.0f)) + (CHAR_WIDTH * 10.0f) + (BUTTON_WIDTH * 7.0f);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * height);
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// �{�^���̈ʒu���킹
	for (int i = SEB_FRONT_BACK; i <= SEB_ERASE; i++)
	{
		float width = static_cast<float>((i - SEB_FRONT_BACK) % 10);
		float height = static_cast<float>((i - SEB_FRONT_BACK) / 10);

		// �傫����ς���
		this->mvp_main_buttons[i]->SetDefaultPosAndColl(BUTTON_WIDTH, BUTTON_HEIGHT);

		// ���W�ύX
		Transform transform = this->mvp_main_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (BUTTON_WIDTH * (width + 1.0f));
		transform.position.y = (-screen_height * 0.5f) + (10.0f * (height + 1.0f)) + (BUTTON_HEIGHT * (height + 1.0f));
		this->mvp_main_buttons[i]->SetTransform(&transform);
	}

	// �����̍쐬
	this->mvp_value_text.resize(VT_MAX);
	for (size_t i = 0; i < mvp_value_text.size(); i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		mvp_value_text[i] = new CEditorText;
		mvp_value_text[i]->Initilize();
	}

	// �����̈ʒu���킹
	for (int i = VT_WORLD_10; i <= VT_WORLD_MAX_01; i++)
	{
		float width = static_cast<float>((i - VT_WORLD_10) % 2);
		float height = static_cast<float>((i - VT_WORLD_10) / 2);

		// ���W�ύX
		Transform transform = this->mvp_value_text[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 4.0f) + (BUTTON_WIDTH * 2.0f) + (CHAR_WIDTH * width);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * height);
		this->mvp_value_text[i]->SetTransform(&transform);
	}

	// �����̈ʒu���킹
	for (int i = VT_STAGE_10; i <= VT_STAGE_MAX_01; i++)
	{
		float width = static_cast<float>((i - VT_STAGE_10) % 2);
		float height = static_cast<float>((i - VT_STAGE_10) / 2);

		// ���W�ύX
		Transform transform = this->mvp_value_text[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (BUTTON_W_WIDTH * 1.5f) + 10.0f + (CHAR_WIDTH * 9.0f) + (BUTTON_WIDTH * 6.0f) + (CHAR_WIDTH * width);
		transform.position.y = (screen_height * 0.5f) - (BUTTON_W_HEIGHT * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * height);
		this->mvp_value_text[i]->SetTransform(&transform);
	}

	// �{�^�������炩���ߗp��
	this->mvp_select_buttons.resize(SSB_MAX);
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// �{�^���I�u�W�F�N�g����
		this->mvp_select_buttons[i] = new CButton;
		this->mvp_select_buttons[i]->Initilize();

		// �{�^���̑傫����ς���
		this->mvp_select_buttons[i]->SetDefaultPosAndColl(BUTTON_W_WIDTH, BUTTON_W_HEIGHT);

		float width = static_cast<float>(i % 10);
		float height = static_cast<float>(i / 10);

		// ���W�ύX
		Transform transform = this->mvp_select_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (BUTTON_W_WIDTH * (width + 1.0f));
		transform.position.y = (screen_height * 0.5f) - (10.0f * (height + 1.0f)) - (BUTTON_W_HEIGHT * (height + 1.0f));
		this->mvp_select_buttons[i]->SetTransform(&transform);

		// �{�^���̐ݒ�
		this->mvp_select_buttons[i]->SetButton(VK_LBUTTON, XI_BUTTON_B);
	}
	this->mvp_select_buttons[SSB_BACK]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 3.0f, 0.0f }, true);

	this->mvp_gimmik_image.resize(GSI_MAX);
	for (size_t i = 0; i < this->mvp_gimmik_image.size(); i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CImage* new_image = new CImage;
		new_image->Initilize();

		// �摜�̑傫����ς���
		new_image->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

		float width = static_cast<float>(i % 30);
		float height = static_cast<float>(i / 30);

		// ���W�ύX
		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (MAPCHIP_WIDTH * (width + 1.0f));
		transform.position.y = (screen_height * 0.5f) - (10.0f * (height + 2.0f)) - (BUTTON_W_HEIGHT)-(MAPCHIP_HEIGHT * (height + 1.0f));
		new_image->SetTransform(&transform);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_gimmik_image[i] = new_image;
	}

	// �摜�ݒ�
	this->mvp_gimmik_image[GSI_GOAL]->AddTexture(SPRITE_GOAL);
	this->mvp_gimmik_image[GSI_PIECE_1]->AddTexture(SPRITE_PIECE_0);
	this->mvp_gimmik_image[GSI_PIECE_2]->AddTexture(SPRITE_PIECE_0);
	this->mvp_gimmik_image[GSI_PIECE_3]->AddTexture(SPRITE_PIECE_0);
	this->mvp_gimmik_image[GSI_RED_BLOCK]->AddTexture(SPRITE_RED_BULE);
	this->mvp_gimmik_image[GSI_BLUE_BLOCK]->AddTexture(SPRITE_RED_BULE);
	this->mvp_gimmik_image[GSI_TORNADO]->AddTexture(SPRITE_TORNADO);
	this->mvp_gimmik_image[GSI_DHARMA]->AddTexture(SPRITE_DHARMA);
	this->mvp_gimmik_image[GSI_CANNON]->AddTexture(SPRITE_CANNON);
	this->mvp_gimmik_image[GSI_LION_TOP]->AddTexture(SPRITE_LION_TOP);
	this->mvp_gimmik_image[GSI_LION_BOTTOM]->AddTexture(SPRITE_LION_BOTTOM);

	// UV�ݒ�
	{
		UV new_uv = { 1.0f, 0.0f };
		this->mvp_gimmik_image[GSI_PIECE_2]->SetAnim(&new_uv);
	}
	{
		UV new_uv = { 2.0f, 0.0f };
		this->mvp_gimmik_image[GSI_PIECE_3]->SetAnim(&new_uv);
	}
	{
		UV new_uv = { 1.0f, 1.0f };
		this->mvp_gimmik_image[GSI_BLUE_BLOCK]->SetAnim(&new_uv);
	}

	// �v���C���[�̍쐬
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CPlayer* new_image = new CPlayer;
		new_image->Initilize();

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_player.emplace_back(new_image);
	}

	// �J�[�\����̉摜�p�I�u�W�F�N�g�쐬
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CCursor* new_image = new CCursor;
		new_image->Initilize();

		Color color = { 1.0f, 1.0f, 1.0f, 0.5f };
		new_image->SetColor(&color);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_cursor_image.emplace_back(new_image);
	}

	// �X�e�[�W�ǂݍ���
	this->LoadStage();

	// �J�[�\��
	this->mp_cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();
}

void CStageEditor::Finalize()
{
	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->UnInitilize();
		delete this->mvp_images[i];
	}
	this->mvp_images.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_mapchip.size(); i++)
	{
		this->mvp_mapchip[i]->UnInitilize();
		delete this->mvp_mapchip[i];
	}
	this->mvp_mapchip.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
	{
		this->mvp_mapchip_image[i]->UnInitilize();
		delete this->mvp_mapchip_image[i];
	}
	this->mvp_mapchip_image.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_text.size(); i++)
	{
		this->mvp_text[i]->UnInitilize();
		delete this->mvp_text[i];
	}
	this->mvp_text.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_value_text.size(); i++)
	{
		this->mvp_value_text[i]->UnInitilize();
		delete this->mvp_value_text[i];
	}
	this->mvp_value_text.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_pages.size(); i++)
	{
		this->mvp_pages[i]->UnInitilize();
		delete this->mvp_pages[i];
	}
	this->mvp_pages.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_player.size(); i++)
	{
		this->mvp_player[i]->UnInitilize();
		delete this->mvp_player[i];
	}
	this->mvp_player.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->UnInitilize();
		delete this->mvp_main_buttons[i];
	}
	this->mvp_main_buttons.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->UnInitilize();
		delete this->mvp_select_buttons[i];
	}
	this->mvp_select_buttons.clear();

	// �I�u�W�F�N�g���
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->UnInitilize();
		delete this->mvp_cursor_image[i];
	}
	this->mvp_cursor_image.clear();

	// �M�~�b�N���܂Ƃ߂ĉ��
	std::vector<CImage*>* vp_ap_gimmik[6];
	vp_ap_gimmik[0] = &this->mvp_goal;
	vp_ap_gimmik[1] = &this->mvp_piece;
	vp_ap_gimmik[2] = &this->mvp_tornado;
	vp_ap_gimmik[3] = &this->mvp_dharma;
	vp_ap_gimmik[4] = &this->mvp_cannon;
	vp_ap_gimmik[5] = &this->mvp_lion;
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < vp_ap_gimmik[i]->size(); j++)
		{
			(*vp_ap_gimmik[i])[j]->UnInitilize();
			delete (*vp_ap_gimmik[i])[j];
		}
		vp_ap_gimmik[i]->clear();
	}

	// �摜���\�[�X�̊J��
	for (size_t i = 0; i < gv_sprite_data.size(); i++)
	{
		COM_SAFE_RELEASE(gv_sprite_data[i].mSRV);
	}

	this->mp_cursor = nullptr;
}

void CStageEditor::Update()
{
	// �e��X�V������
	switch (g_edit_mode)
	{
	case EM_NONE:
		// �J�[�\���̉摜�͏������
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		this->NormalUpdate();
		break;
	case EM_SPRITE_OPTION:
		this->SpriteOptionUpdate();
		break;
	case EM_MAPCHIP_SELECT:
		this->MapChipSelectUpdate();
		break;
	case EM_MAPCHIP:
		this->MapChipUpdate();
		break;
	case EM_GIMMIK_SELECT:
		this->GimmikSelectUpdate();
		break;
	case EM_GIMMIK:
		this->GimmikUpdate();
		break;
	case EM_BACK_SELECT:
		// �J�[�\���̉摜�͏������
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		this->BackSelectUpdate();
		break;
	}
}

void CStageEditor::Draw()
{
	// �e��`�揈����
	switch (g_view_mode)
	{
	case EV_NONE:
		this->NormalDraw();
		break;
	case EV_SPRITE_SELECT:
		this->SpriteOptionDraw();
		break;
	case EV_GIMMIK_SELECT:
		this->GimmikSelectDraw();
		break;
	}
}

void CStageEditor::PauseUpdate()
{
	// Pause���Ƀ{�^����߂�
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->SetONOFF(false);
	}
}

void CStageEditor::PauseDraw()
{
	this->Draw();
}

void CStageEditor::ColliderTest()
{
	// �e�퓖���蔻�菈����
	switch (g_edit_mode)
	{
	case EM_NONE:
		this->NormalColliderTest();
		break;
	case EM_SPRITE_OPTION:
		this->SpriteOptionColliderTest();
		break;
	case EM_MAPCHIP_SELECT:
		this->MapChipSelectColliderTest();
		break;
	case EM_MAPCHIP:
		this->MapChipColliderTest();
		break;
	case EM_GIMMIK_SELECT:
		this->GimmikSelectColliderTest();
		break;
	case EM_GIMMIK:
		this->GimmikColliderTest();
		break;
	case EM_BACK_SELECT:
		this->BackSelectColliderTest();
		break;
	}
}

void CStageEditor::LoadStage()
{
	// �Ƃ肠�����ݒ���Œ�l�ɂ���
	gi_world_index = 0;
	gi_stage_index = 0;
	gi_world_max = 1;
	gi_stage_max = 1;
	gb_is_front = true;

	// �X�v���C�g�t�@�N�g���[�������Ɏg��
	CSpriteFactory* sprite_factory = CSingleton<CSpriteFactory>::GetInstance();

	// �f�t�H���g�X�v���C�g�𐶐�
	sprite_factory->CreateSprite(&g_default_chip, DEFAULT_CHIP_SPRITE);
	sprite_factory->CreateSprite(&g_default_front, DEFAULT_FRONT_SPRITE);
	sprite_factory->CreateSprite(&g_default_back, DEFAULT_BACK_SPRITE);

	// �X�v���C�g�̏�����
	gv_io_sprite_data.clear();
	gv_sprite_data.clear();

	// �X�e�[�W������
	for (size_t w = 0; w < ga_stage_data.size(); w++)
	{
		for (size_t s = 0; s < ga_stage_data[w].size(); s++)
		{
			// �X�v���C�g����
			ga_stage_data[w][s].mi_front_back = MAPCHIP_SPRITE_NONE;
			ga_stage_data[w][s].mi_back_back = MAPCHIP_SPRITE_NONE;

			// �v���C���[�͍���
			ga_stage_data[w][s].m_player_pos_x = 0;
			ga_stage_data[w][s].m_player_pos_y = 1;

			// �S�[���f�[�^������
			ga_stage_data[w][s].mv_front_goal_obj.clear();
			ga_stage_data[w][s].mv_back_goal_obj.clear();

			// �J�P���f�[�^������
			ga_stage_data[w][s].mv_front_piece_obj.clear();
			ga_stage_data[w][s].mv_back_piece_obj.clear();

			// �����f�[�^������
			ga_stage_data[w][s].mv_front_tornade_obj.clear();
			ga_stage_data[w][s].mv_back_tornade_obj.clear();

			// �_���}�f�[�^������
			ga_stage_data[w][s].mv_front_dharma_obj.clear();
			ga_stage_data[w][s].mv_back_dharma_obj.clear();

			// ��C�f�[�^������
			ga_stage_data[w][s].mv_front_cannon_obj.clear();
			ga_stage_data[w][s].mv_back_cannon_obj.clear();

			// ���t�g�f�[�^������
			ga_stage_data[w][s].mv_front_lion_obj.clear();
			ga_stage_data[w][s].mv_back_lion_obj.clear();

			// �}�b�v�`�b�v�f�[�^������
			for (size_t x = 0; x < ga_stage_data[w][s].mv_front_data.size(); x++)
			{
				for (size_t y = 0; y < ga_stage_data[w][s].mv_front_data[x].size(); y++)
				{
					ga_stage_data[w][s].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
					ga_stage_data[w][s].mv_front_data[x][y].mf_angle_z = 0.0f;
					ga_stage_data[w][s].mv_front_data[x][y].m_tag = TAG_MAPCHIP;
				}
			}
			// �}�b�v�`�b�v�f�[�^������
			for (size_t x = 0; x < ga_stage_data[w][s].mv_back_data.size(); x++)
			{
				for (size_t y = 0; y < ga_stage_data[w][s].mv_back_data[x].size(); y++)
				{
					ga_stage_data[w][s].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
					ga_stage_data[w][s].mv_back_data[x][y].mf_angle_z = 0.0f;
					ga_stage_data[w][s].mv_back_data[x][y].m_tag = TAG_MAPCHIP;
				}
			}
		}
	}

	{
		std::ifstream ifs("assets/Game/Stage/StageData.dat");

		if (!ifs.fail()) {
			size_t size;

			// �摜�E�X�e�[�W���ǂݍ���
			ifs >> size;
			ifs >> gi_world_max;
			ifs >> gi_stage_max;

			gv_io_sprite_data.resize(size);

			// �摜�f�[�^�ǂݍ���
			for (size_t i = 0; i < gv_io_sprite_data.size(); i++)
			{
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.filename;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.anim_max;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.slice_x;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.slice_y;
				ifs >> gv_io_sprite_data[i].m_io_sprite_data.count_check;
				ifs >> gv_io_sprite_data[i].m_width;
				ifs >> gv_io_sprite_data[i].m_height;
			}

			// �摜����
			for (size_t i = 0; i < gv_io_sprite_data.size(); i++)
			{
				SpriteData new_sprite;

				// CDirect3D�擾
				CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

				// �摜�ǂݍ���
				HRESULT hr = direct3d->LoadTexture(gv_io_sprite_data[i].m_io_sprite_data.filename.c_str(), &new_sprite.mSRV);

				if (FAILED(hr)) {
					// �ꉞ nullptr ��
					new_sprite.mSRV = nullptr;
					gv_sprite_data.emplace_back(new_sprite);
				}
				else {
					// �摜�̐ݒ�����A�o�^
					new_sprite.anim_max = gv_io_sprite_data[i].m_io_sprite_data.anim_max;
					new_sprite.slice_x = gv_io_sprite_data[i].m_io_sprite_data.slice_x;
					new_sprite.slice_y = gv_io_sprite_data[i].m_io_sprite_data.slice_y;

					new_sprite.slice.u = 1.0f / static_cast<float>(new_sprite.slice_x);
					new_sprite.slice.v = 1.0f / static_cast<float>(new_sprite.slice_y);

					new_sprite.count_check = gv_io_sprite_data[i].m_io_sprite_data.count_check;

					gv_sprite_data.emplace_back(new_sprite);
				}
			}
		}
	}

	// �摜�I�u�W�F�N�g����
	this->CreateImage();

	{
		for (size_t w = 0; w < static_cast<size_t>(gi_world_max); w++)
		{
			for (size_t s = 0; s < static_cast<size_t>(gi_stage_max); s++)
			{
				// �f�B���N�g����
				std::string dir_name = "assets/Game/Stage/Stage_";

				// �g���q��
				std::string ex_name = ".dat";

				// �t�@�C����
				std::string file_name = dir_name + std::to_string(w) + "_" + std::to_string(s) + ex_name;

				std::ifstream ifs(file_name);

				if (!ifs.fail()) {


					// �}�b�v�`�b�v�f�[�^�ǂݍ���
					for (size_t x = 0; x < ga_stage_data[w][s].mv_front_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_front_data[x].size(); y++)
						{
							int tag;
							ifs >> ga_stage_data[w][s].mv_front_data[x][y].mi_chip_id;
							ifs >> ga_stage_data[w][s].mv_front_data[x][y].mf_angle_z;
							ifs >> tag;
							ga_stage_data[w][s].mv_front_data[x][y].m_tag = static_cast<ObjectTag>(tag);
						}
					}
					// �}�b�v�`�b�v�f�[�^�ǂݍ���
					for (size_t x = 0; x < ga_stage_data[w][s].mv_back_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_back_data[x].size(); y++)
						{
							int tag;
							ifs >> ga_stage_data[w][s].mv_back_data[x][y].mi_chip_id;
							ifs >> ga_stage_data[w][s].mv_back_data[x][y].mf_angle_z;
							ifs >> tag;
							ga_stage_data[w][s].mv_back_data[x][y].m_tag = static_cast<ObjectTag>(tag);
						}
					}

					// �d�l�ύX�ł���Ȃ��Ȃ����f�[�^���i�[����
					int trash;

					// �w�i�摜�ԍ��ǂݍ���
					ifs >> ga_stage_data[w][s].mi_front_back;
					ifs >> ga_stage_data[w][s].mi_back_back;

					// �v���C���[�ʒu�ǂݍ���
					ifs >> ga_stage_data[w][s].m_player_pos_x;
					ifs >> ga_stage_data[w][s].m_player_pos_y;

					// �c�����S�~
					ifs >> trash;
					ifs >> trash;

					// �S�[���f�[�^�ǂݍ���
					int size;
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						GoalData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_front_goal_obj.emplace_back(data);
					}

					// �S�[���f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						GoalData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_back_goal_obj.emplace_back(data);
					}

					// �J�P���f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						PieceData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.i_piece_num;
						ga_stage_data[w][s].mv_front_piece_obj.emplace_back(data);
					}

					// �J�P���f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						PieceData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.i_piece_num;
						ga_stage_data[w][s].mv_back_piece_obj.emplace_back(data);
					}

					// �����f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						TornadeData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_front_tornade_obj.emplace_back(data);
					}

					// �����f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						TornadeData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ga_stage_data[w][s].mv_back_tornade_obj.emplace_back(data);
					}

					// �_���}�f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						DharmaData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_max_speed;
						ifs >> data.mb_is_left;
						ga_stage_data[w][s].mv_front_dharma_obj.emplace_back(data);
					}

					// �_���}�f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						DharmaData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_max_speed;
						ifs >> data.mb_is_left;
						ga_stage_data[w][s].mv_back_dharma_obj.emplace_back(data);
					}

					// ��C�f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						CannonData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_angle_z;
						ifs >> data.mf_max_speed;
						ifs >> data.mf_wait;
						ga_stage_data[w][s].mv_front_cannon_obj.emplace_back(data);
					}

					// ��C�f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						CannonData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_angle_z;
						ifs >> data.mf_max_speed;
						ifs >> data.mf_wait;
						ga_stage_data[w][s].mv_back_cannon_obj.emplace_back(data);
					}

					// ���t�g�f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						LionData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_up_max_speed;
						ifs >> data.mf_down_max_speed;
						ifs >> data.mf_up_wait;
						ifs >> data.mf_down_wait;
						ga_stage_data[w][s].mv_front_lion_obj.emplace_back(data);
					}

					// ���t�g�f�[�^�ǂݍ���
					ifs >> size;
					if (size == END_OF_STAGE_FILE) continue;
					for (size_t i = 0; i < static_cast<size_t>(size); i++)
					{
						LionData data;
						ifs >> data.position.x;
						ifs >> data.position.y;
						ifs >> data.position.z;
						ifs >> data.mf_up_max_speed;
						ifs >> data.mf_down_max_speed;
						ifs >> data.mf_up_wait;
						ifs >> data.mf_down_wait;
						ga_stage_data[w][s].mv_back_lion_obj.emplace_back(data);
					}
				}
			}
		}
	}
}

void CStageEditor::SaveStage()
{
	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::cout << "�Z�[�u���܂��B�������Ȃ��ł��������B" << std::endl;

	{
		std::ofstream ofs("assets/Game/Stage/StageData.dat");

		// �摜�E�X�e�[�W�������o��
		ofs << gv_io_sprite_data.size() << " "
			<< gi_world_max << " "
			<< gi_stage_max << std::endl;

		// �摜�ݒ菑���o��
		for (size_t i = 0; i < gv_io_sprite_data.size(); i++)
		{
			ofs << gv_io_sprite_data[i].m_io_sprite_data.filename << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.anim_max << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.slice_x << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.slice_y << " "
				<< gv_io_sprite_data[i].m_io_sprite_data.count_check << " "
				<< gv_io_sprite_data[i].m_width << " "
				<< gv_io_sprite_data[i].m_height << std::endl;
		}
	}

	{
		// �S�X�e�[�W�����o��
		for (size_t w = 0; w < static_cast<size_t>(gi_world_max); w++)
		{
			for (size_t s = 0; s < static_cast<size_t>(gi_stage_max); s++)
			{
				std::string dir_name = "assets/Game/Stage/Stage_";
				std::string ex_name = ".dat";

				std::string file_name = dir_name + std::to_string(w) + "_" + std::to_string(s) + ex_name;

				std::ofstream ofs(file_name);

				if (!ofs.fail()) {

					// �}�b�v�`�b�v�f�[�^�����o��
					for (size_t x = 0; x < ga_stage_data[w][s].mv_front_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_front_data[x].size(); y++)
						{
							ofs << ga_stage_data[w][s].mv_front_data[x][y].mi_chip_id << " "
								<< ga_stage_data[w][s].mv_front_data[x][y].mf_angle_z << " "
								<< static_cast<int>(ga_stage_data[w][s].mv_front_data[x][y].m_tag) << std::endl;
						}
					}

					// �}�b�v�`�b�v�f�[�^�����o��
					for (size_t x = 0; x < ga_stage_data[w][s].mv_back_data.size(); x++)
					{
						for (size_t y = 0; y < ga_stage_data[w][s].mv_back_data[x].size(); y++)
						{
							ofs << ga_stage_data[w][s].mv_back_data[x][y].mi_chip_id << " "
								<< ga_stage_data[w][s].mv_back_data[x][y].mf_angle_z << " "
								<< static_cast<int>(ga_stage_data[w][s].mv_back_data[x][y].m_tag) << std::endl;
						}
					}

					// �w�i�ԍ��E�v���C���[�ʒu�E�̖̂��c�������o��
					ofs << ga_stage_data[w][s].mi_front_back << " "
						<< ga_stage_data[w][s].mi_back_back << " "
						<< ga_stage_data[w][s].m_player_pos_x << " "
						<< ga_stage_data[w][s].m_player_pos_y << " "
						<< 0 << " "
						<< 0 << std::endl;

					// �S�[���f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_front_goal_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_goal_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_goal_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_goal_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_goal_obj[i].position.z << std::endl;
					}

					// �S�[���f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_back_goal_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_goal_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_goal_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_goal_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_goal_obj[i].position.z << std::endl;
					}

					// �J�P���f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_front_piece_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_piece_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_piece_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_piece_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_piece_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_piece_obj[i].i_piece_num << std::endl;
					}

					// �J�P���f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_back_piece_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_piece_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_piece_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_piece_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_piece_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_piece_obj[i].i_piece_num << std::endl;
					}

					// �����f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_front_tornade_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_tornade_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_tornade_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_tornade_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_tornade_obj[i].position.z << std::endl;
					}

					// �����f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_back_tornade_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_tornade_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_tornade_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_tornade_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_tornade_obj[i].position.z << std::endl;
					}

					// �_���}�f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_front_dharma_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_dharma_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_dharma_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_front_dharma_obj[i].mb_is_left << std::endl;
					}

					// �_���}�f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_back_dharma_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_dharma_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_dharma_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_back_dharma_obj[i].mb_is_left << std::endl;
					}

					// ��C�f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_front_cannon_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_cannon_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_cannon_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].mf_angle_z << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_front_cannon_obj[i].mf_wait << std::endl;
					}

					// ��C�f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_back_cannon_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_cannon_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_cannon_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].mf_angle_z << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].mf_max_speed << " "
							<< ga_stage_data[w][s].mv_back_cannon_obj[i].mf_wait << std::endl;
					}

					// ���t�g�f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_front_lion_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_front_lion_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_front_lion_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_up_max_speed << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_down_max_speed << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_up_wait << " "
							<< ga_stage_data[w][s].mv_front_lion_obj[i].mf_down_wait << std::endl;
					}

					// ���t�g�f�[�^�����o��
					ofs << ga_stage_data[w][s].mv_back_lion_obj.size() << std::endl;
					for (size_t i = 0; i < ga_stage_data[w][s].mv_back_lion_obj.size(); i++)
					{
						ofs << ga_stage_data[w][s].mv_back_lion_obj[i].position.x << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].position.y << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].position.z << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_up_max_speed << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_down_max_speed << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_up_wait << " "
							<< ga_stage_data[w][s].mv_back_lion_obj[i].mf_down_wait << std::endl;
					}


					ofs << END_OF_STAGE_FILE << std::endl;

				}
			}
		}
	}

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

}

void CStageEditor::UpdateStage()
{
	// �X�v���C�g�t�@�N�g���[�������Ɏg��
	CSpriteFactory* sprite_factory = CSingleton<CSpriteFactory>::GetInstance();

	// �\����
	if (gb_is_front) {
		// �w�i�ԍ�
		int front_id = ga_stage_data[gi_world_index][gi_stage_index].mi_front_back;
		int back_id = ga_stage_data[gi_world_index][gi_stage_index].mi_back_back;

		// �摜��������Ԃ̂��̂�
		SpriteData front_sprite = g_default_front;
		SpriteData back_sprite = g_default_back;

		// �摜���w�肳�ꑶ�݂���Ȃ����
		if (front_id > MAPCHIP_SPRITE_NONE && front_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[front_id].mSRV != nullptr) front_sprite = gv_sprite_data[front_id];
		}
		if (back_id > MAPCHIP_SPRITE_NONE && back_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[back_id].mSRV != nullptr) back_sprite = gv_sprite_data[back_id];
		}

		// �w�i�摜�̒ǉ�
		for (size_t i = 0; i < this->mvp_pages.size(); i++)
		{
			// �e�N�X�`���ǉ�
			this->mvp_pages[i]->SetSprite(&front_sprite, &back_sprite);
		}

		// �}�b�v�`�b�v������
		for (size_t x = 0; x < ga_stage_data[gi_world_index][gi_stage_index].mv_front_data.size(); x++)
		{
			for (size_t y = 0; y < ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x].size(); y++)
			{
				UV default_uv = { 0.0f, 0.0f };
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&default_uv);
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);

				if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag == TAG_RED_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag == TAG_BLUE_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
					UV new_uv = { 1.0f, 1.0f };
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&new_uv);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag == TAG_LION_BOTTOM)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_LION_BOTTOM);

					switch (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id)
					{
					case LEFT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f / 3.0f, 1.0f, 0.0f);
						break;
					case CENTER_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(1.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f);
						break;
					case RIGHT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(2.0f / 3.0f, 1.0f, 1.0f, 0.0f);
						break;
					default:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);
						break;
					}
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id <= MAPCHIP_SPRITE_NONE) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id >= static_cast<int>(gv_sprite_data.size()))
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id].mSRV == nullptr) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id]);
				}
				Transform tr = this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->GetTransform();
				tr.angle.z = ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z;
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTransform(&tr);
			}
		}

		// �S�[��
		this->UpdateGoal(ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj);

		// �J�P��
		this->UpdatePiece(ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj);

		// ����
		this->UpdateTornade(ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj);

		// �_���}
		this->UpdateDharma(ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj);

		// ��C
		this->UpdateCannon(ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj);

		// ���t�g
		this->UpdateLion(ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj);

		for (size_t i = 0; i < this->mvp_player.size(); i++)
		{
			Transform tr = this->mvp_player[i]->GetTransform();
			Collider collider = this->mvp_player[i]->GetCollider();
			tr.position.x = (MAPCHIP_WIDTH * (-(static_cast<float>(MAPCHIP_NUM_WIDTH) * static_cast<float>(AREA_NUM)) / 2.0f + 0.5f) + (MAPCHIP_WIDTH * static_cast<float>(ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_x))) - (collider.center_x * 2.0f);
			tr.position.y = (MAPCHIP_HEIGHT * (-static_cast<float>(MAPCHIP_NUM_HEIGHT) / 2.0f - 1.0f) + (PLAYER_COLLIDER_HEIGHT / 2.0f) + (PLAYER_COLLIDER_HEIGHT / 2.0f) + (MAPCHIP_HEIGHT * static_cast<float>(ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_y))) - (collider.center_y * 2.0f);
			this->mvp_player[i]->SetTransform(&tr);
		}
	}
	else {
		int front_id = ga_stage_data[gi_world_index][gi_stage_index].mi_back_back;
		int back_id = ga_stage_data[gi_world_index][gi_stage_index].mi_front_back;

		SpriteData front_sprite = g_default_back;
		SpriteData back_sprite = g_default_front;

		if (front_id > MAPCHIP_SPRITE_NONE && front_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[front_id].mSRV != nullptr) front_sprite = gv_sprite_data[front_id];
		}
		if (back_id > MAPCHIP_SPRITE_NONE && back_id < static_cast<int>(gv_sprite_data.size())) {
			if (gv_sprite_data[back_id].mSRV != nullptr) back_sprite = gv_sprite_data[back_id];
		}

		// �w�i�摜�̒ǉ�
		for (size_t i = 0; i < this->mvp_pages.size(); i++)
		{
			// �e�N�X�`���ǉ�
			this->mvp_pages[i]->SetSprite(&front_sprite, &back_sprite);
		}

		// �}�b�v�`�b�v������
		for (size_t x = 0; x < ga_stage_data[gi_world_index][gi_stage_index].mv_back_data.size(); x++)
		{
			for (size_t y = 0; y < ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x].size(); y++)
			{
				UV default_uv = { 0.0f, 0.0f };
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&default_uv);
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);

				if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag == TAG_RED_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag == TAG_BLUE_BLOCK)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_RED_BULE);
					UV new_uv = { 1.0f, 1.0f };
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetAnim(&new_uv);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag == TAG_LION_BOTTOM)
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->ChangeTexture(SPRITE_LION_BOTTOM);

					switch (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id)
					{
					case LEFT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f / 3.0f, 1.0f, 0.0f);
						break;
					case CENTER_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(1.0f / 3.0f, 2.0f / 3.0f, 1.0f, 0.0f);
						break;
					case RIGHT_LION_BOTTOM:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(2.0f / 3.0f, 1.0f, 1.0f, 0.0f);
						break;
					default:
						this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetDefaultUV(0.0f, 1.0f, 1.0f, 0.0f);
						break;
					}
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id <= MAPCHIP_SPRITE_NONE) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id >= static_cast<int>(gv_sprite_data.size()))
				{
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else if (gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id].mSRV == nullptr) {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&g_default_chip);
				}
				else {
					this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTexture(&gv_sprite_data[ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id]);
				}

				Transform tr = this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->GetTransform();
				tr.angle.z = ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z;
				this->mvp_mapchip[y * MAPCHIP_NUM_WIDTH * AREA_NUM + x]->SetTransform(&tr);
			}
		}

		// �S�[��
		this->UpdateGoal(ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj);

		// �J�P��
		this->UpdatePiece(ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj);

		// ����
		this->UpdateTornade(ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj);

		// �_���}
		this->UpdateDharma(ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj);

		// ��C
		this->UpdateCannon(ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj);

		// ���t�g
		this->UpdateLion(ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj);

	}
}

void CStageEditor::CreateImage()
{
	for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
	{
		this->mvp_mapchip_image[i]->UnInitilize();
		delete this->mvp_mapchip_image[i];
	}
	this->mvp_mapchip_image.clear();

	// CDirect3D�擾
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// �X�N���[���T�C�Y�i�[
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	for (size_t i = 0; i < gv_sprite_data.size(); i++)
	{
		// �摜�p�I�u�W�F�N�g�̐���
		CImage* new_image = new CImage;
		new_image->Initilize();

		// �e�N�X�`���ǉ�
		new_image->AddTexture(SPRITE_TEST_2);
		if (gv_sprite_data[i].mSRV != nullptr) new_image->SetTexture(&gv_sprite_data[i]);

		// �摜�̑傫����ς���
		new_image->SetDefaultPosAndColl(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

		float width = static_cast<float>(i % 30);
		float height = static_cast<float>(i / 30);

		Transform transform = new_image->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (10.0f * (width + 1.0f)) + (MAPCHIP_WIDTH * (width + 1.0f));
		transform.position.y = (screen_height * 0.5f) - (10.0f * (height + 2.0f)) - (BUTTON_W_HEIGHT)-(MAPCHIP_HEIGHT * (height + 1.0f));
		new_image->SetTransform(&transform);

		// �摜�I�u�W�F�N�g�̒ǉ�
		this->mvp_mapchip_image.emplace_back(new_image);
	}
}

void CStageEditor::AddSprite()
{
	IO_SpriteDataEX new_data;

	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::string dir_name = "assets/Game/Sprites/";
	std::string file_name;

	std::cout << "�ǉ�����摜�̃p�X����͂��Ă������� : \n" << dir_name;
	std::cin >> file_name;
	std::cout << std::endl;

	new_data.m_io_sprite_data.filename = dir_name + file_name;

	std::cout << "�摜�̉�������͂��Ă������� : \n";
	std::cin >> new_data.m_width;
	std::cout << std::endl;

	std::cout << "�摜�̏c������͂��Ă������� : \n";
	std::cin >> new_data.m_height;
	std::cout << std::endl;

	std::cout << "�摜�̉��̕���������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.slice_x;
	std::cout << std::endl;

	std::cout << "�摜�̏c�̕���������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.slice_y;
	std::cout << std::endl;

	std::cout << "�g�p���閇������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.anim_max;
	std::cout << std::endl;

	std::cout << "�؂�ւ��̑҂��b������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.count_check;
	std::cout << std::endl;

	if (new_data.m_width < 1.0f) {
		new_data.m_width = 1.0f;
	}
	if (new_data.m_height < 1.0f) {
		new_data.m_height = 1.0f;
	}

	if (new_data.m_io_sprite_data.slice_x < 1) {
		new_data.m_io_sprite_data.slice_x = 1;
	}
	if (new_data.m_io_sprite_data.slice_y < 1) {
		new_data.m_io_sprite_data.slice_y = 1;
	}
	if (new_data.m_io_sprite_data.anim_max < 1) {
		new_data.m_io_sprite_data.anim_max = 1;
	}
	if (new_data.m_io_sprite_data.count_check < 0) {
		new_data.m_io_sprite_data.count_check = 1;
	}

	{
		SpriteData new_sprite;

		// CDirect3D�擾
		CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

		HRESULT hr = direct3d->LoadTexture(new_data.m_io_sprite_data.filename.c_str(), &new_sprite.mSRV);

		if (FAILED(hr)) {
			std::cout << "�t�@�C����������Ȃ��ł�" << std::endl;
		}
		else {
			new_sprite.anim_max = new_data.m_io_sprite_data.anim_max;
			new_sprite.slice_x = new_data.m_io_sprite_data.slice_x;
			new_sprite.slice_y = new_data.m_io_sprite_data.slice_y;

			new_sprite.slice.u = 1.0f / static_cast<float>(new_sprite.slice_x);
			new_sprite.slice.v = 1.0f / static_cast<float>(new_sprite.slice_y);

			new_sprite.count_check = new_data.m_io_sprite_data.count_check;

			gv_io_sprite_data.emplace_back(new_data);
			gv_sprite_data.emplace_back(new_sprite);
		}
	}

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

	this->CreateImage();
}

void CStageEditor::ChangeSprite(int id)
{
	if (id < 0) return;
	if (static_cast<size_t>(id) >= gv_io_sprite_data.size()) return;

	IO_SpriteDataEX new_data;

	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::string dir_name = "assets/Game/Sprites/";
	std::string file_name;

	std::cout << gv_io_sprite_data[id].m_io_sprite_data.filename << "��ݒ肵�Ȃ����܂��B\n" << std::endl;

	int check = 0;
	std::cout << "�摜��ς���Ȃ� 1 ����͂��Ă������� : \n";
	std::cin >> check;
	std::cout << std::endl;

	new_data.m_io_sprite_data.filename = gv_io_sprite_data[id].m_io_sprite_data.filename;

	if (check == 1) {
		std::string dir_name = "assets/Game/Sprites/";
		std::string file_name;
		std::string final_file_name;

		std::cout << "�ύX����摜�̃p�X����͂��Ă������� : \n" << dir_name;
		std::cin >> file_name;
		std::cout << std::endl;

		final_file_name = dir_name + file_name;

		SpriteData new_sprite;

		// CDirect3D�擾
		CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

		HRESULT hr = direct3d->LoadTexture(final_file_name.c_str(), &new_sprite.mSRV);

		if (FAILED(hr)) {
			std::cout << "�t�@�C����������Ȃ��ł�" << std::endl;
		}
		else {
			new_sprite.anim_max = new_data.m_io_sprite_data.anim_max;
			new_sprite.slice_x = new_data.m_io_sprite_data.slice_x;
			new_sprite.slice_y = new_data.m_io_sprite_data.slice_y;

			new_sprite.slice.u = 1.0f / static_cast<float>(new_sprite.slice_x);
			new_sprite.slice.v = 1.0f / static_cast<float>(new_sprite.slice_y);

			new_sprite.count_check = new_data.m_io_sprite_data.count_check;

			COM_SAFE_RELEASE(gv_sprite_data[id].mSRV);

			gv_sprite_data[id] = new_sprite;

			new_data.m_io_sprite_data.filename = final_file_name;
		}
	}

	std::cout << "�摜�̉�������͂��Ă������� : \n";
	std::cin >> new_data.m_width;
	std::cout << std::endl;

	std::cout << "�摜�̏c������͂��Ă������� : \n";
	std::cin >> new_data.m_height;
	std::cout << std::endl;

	std::cout << "�摜�̉��̕���������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.slice_x;
	std::cout << std::endl;

	std::cout << "�摜�̏c�̕���������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.slice_y;
	std::cout << std::endl;

	std::cout << "�g�p���閇������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.anim_max;
	std::cout << std::endl;

	std::cout << "�؂�ւ��̑҂��b������͂��Ă������� : \n";
	std::cin >> new_data.m_io_sprite_data.count_check;
	std::cout << std::endl;

	if (new_data.m_width < 1.0f) {
		new_data.m_width = 1.0f;
	}
	if (new_data.m_height < 1.0f) {
		new_data.m_height = 1.0f;
	}

	if (new_data.m_io_sprite_data.slice_x < 1) {
		new_data.m_io_sprite_data.slice_x = 1;
	}
	if (new_data.m_io_sprite_data.slice_y < 1) {
		new_data.m_io_sprite_data.slice_y = 1;
	}
	if (new_data.m_io_sprite_data.anim_max < 1) {
		new_data.m_io_sprite_data.anim_max = 1;
	}
	if (new_data.m_io_sprite_data.count_check < 0) {
		new_data.m_io_sprite_data.count_check = 1;
	}

	{
		gv_sprite_data[id].anim_max = new_data.m_io_sprite_data.anim_max;
		gv_sprite_data[id].slice_x = new_data.m_io_sprite_data.slice_x;
		gv_sprite_data[id].slice_y = new_data.m_io_sprite_data.slice_y;

		gv_sprite_data[id].slice.u = 1.0f / static_cast<float>(gv_sprite_data[id].slice_x);
		gv_sprite_data[id].slice.v = 1.0f / static_cast<float>(gv_sprite_data[id].slice_y);

		gv_sprite_data[id].count_check = new_data.m_io_sprite_data.count_check;
	}

	gv_io_sprite_data[id] = new_data;

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

	this->CreateImage();
}

void CStageEditor::MovePlayer()
{
	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	std::cout << "�v���C���[�̈ʒu��ݒ肵�Ȃ����܂��B\n" << std::endl;

	int new_x;
	std::cout << "���[�� 0 �Ƃ��āA���̈ʒu����͂��Ă������� : \n";
	std::cin >> new_x;
	std::cout << std::endl;

	int new_y;
	std::cout << "���[�� 0 �Ƃ��āA�c�̈ʒu����͂��Ă������� : \n";
	std::cin >> new_y;
	std::cout << std::endl;

	if (0 <= new_x && new_x < AREA_NUM * MAPCHIP_NUM_WIDTH) {
		ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_x = new_x;
	}

	if (0 <= new_y && new_y < MAPCHIP_NUM_HEIGHT) {
		ga_stage_data[gi_world_index][gi_stage_index].m_player_pos_y = new_y;
	}

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::UpdateGoal(std::vector<GoalData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_goal.size()) {
			Transform transform = this->mvp_goal[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_goal[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_GOAL);

			new_image->SetDefaultPosAndColl(GOAL_WIDTH, GOAL_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_goal.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_goal.size(); )
	{
		this->mvp_goal[i]->UnInitilize();
		delete this->mvp_goal[i];
		this->mvp_goal.erase(this->mvp_goal.begin() + i);
	}
}

void CStageEditor::UpdatePiece(std::vector<PieceData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_piece.size()) {
			float new_u = static_cast<float>(data[i].i_piece_num % 3);
			if (new_u < 0.0f) new_u *= -1.0f;

			UV new_uv = { new_u, 0.0f };
			this->mvp_piece[i]->SetAnim(&new_uv);

			Transform transform = this->mvp_piece[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_piece[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_PIECE_0);

			new_image->SetDefaultPosAndColl(PIECE_0_WIDTH, PIECE_0_HEIGHT);

			float new_u = static_cast<float>(data[i].i_piece_num % 3);
			if (new_u < 0.0f) new_u *= -1.0f;

			UV new_uv = { new_u, 0.0f };
			new_image->SetAnim(&new_uv);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_piece.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_piece.size(); )
	{
		this->mvp_piece[i]->UnInitilize();
		delete this->mvp_piece[i];
		this->mvp_piece.erase(this->mvp_piece.begin() + i);
	}
}

void CStageEditor::UpdateTornade(std::vector<TornadeData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_tornado.size()) {
			Transform transform = this->mvp_tornado[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_tornado[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_TORNADO);

			new_image->SetDefaultPosAndColl(TORNADE_WIDTH, TORNADE_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_tornado.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_tornado.size(); )
	{
		this->mvp_tornado[i]->UnInitilize();
		delete this->mvp_tornado[i];
		this->mvp_tornado.erase(this->mvp_tornado.begin() + i);
	}
}

void CStageEditor::UpdateDharma(std::vector<DharmaData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_dharma.size()) {
			Transform transform = this->mvp_dharma[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_dharma[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_DHARMA);

			new_image->SetDefaultPosAndColl(DHARMA_WIDTH, DHARMA_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_dharma.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_dharma.size(); )
	{
		this->mvp_dharma[i]->UnInitilize();
		delete this->mvp_dharma[i];
		this->mvp_dharma.erase(this->mvp_dharma.begin() + i);
	}
}

void CStageEditor::UpdateCannon(std::vector<CannonData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_cannon.size()) {
			Transform transform = this->mvp_cannon[i]->GetTransform();
			transform.position = data[i].position;
			transform.angle.z = data[i].mf_angle_z;
			this->mvp_cannon[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_CANNON);

			new_image->SetDefaultPosAndColl(CANNON_WIDTH, CANNON_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			transform.angle.z = data[i].mf_angle_z;
			new_image->SetTransform(&transform);

			this->mvp_cannon.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_cannon.size(); )
	{
		this->mvp_cannon[i]->UnInitilize();
		delete this->mvp_cannon[i];
		this->mvp_cannon.erase(this->mvp_cannon.begin() + i);
	}
}

void CStageEditor::UpdateLion(std::vector<LionData> data)
{
	size_t size = data.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < this->mvp_lion.size()) {
			Transform transform = this->mvp_lion[i]->GetTransform();
			transform.position = data[i].position;
			this->mvp_lion[i]->SetTransform(&transform);
		}
		else {
			CImage* new_image = new CImage;
			new_image->Initilize();
			new_image->AddTexture(SPRITE_LION_TOP);

			new_image->SetDefaultPosAndColl(LION_WIDTH, LION_HEIGHT);

			Transform transform = new_image->GetTransform();
			transform.position = data[i].position;
			new_image->SetTransform(&transform);

			this->mvp_lion.emplace_back(new_image);
		}
	}

	for (size_t i = size; i < this->mvp_lion.size(); )
	{
		this->mvp_lion[i]->UnInitilize();
		delete this->mvp_lion[i];
		this->mvp_lion.erase(this->mvp_lion.begin() + i);
	}
}

void CStageEditor::InputDharmaData(DharmaData * data)
{
	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	float mf_max_speed;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	std::cout << "�_���}�̍ő呬�x����͂��Ă��������B(0.0�����ŏ����l�A0.0�Ō��̒l)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_dharma_max_speed;
	if (mf_max_speed > 0.0f) data->mf_max_speed = mf_max_speed;

	int dir;

	std::cout << "�_���}���ŏ��ɓ���������I��ł��������B(0 �ō��A1 �ŉE)\n : ";
	std::cin >> dir;
	if (dir == 0) data->mb_is_left = true;
	if (dir == 1) data->mb_is_left = false;

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::InputCannonData(CannonData * data)
{
	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	float mf_max_speed;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	std::cout << "�e�̍ő呬�x����͂��Ă��������B(0.0�����ŏ����l�A0.0�Ō��̒l)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_cannon_max_speed;
	if (mf_max_speed > 0.0f) data->mf_max_speed = mf_max_speed;

	float wait;

	std::cout << "�ʂ̔��ˊ��o����͂��Ă��������B(0.0�����ŏ����l�A0.0�Ō��̒l)\n<float �b> : ";
	std::cin >> wait;
	if (wait < 0.0f) wait = cip_data.M_DATA.mf_cannon_wait;
	if (wait > 0.0f) data->mf_wait = wait;

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::InputLionTopData(LionData * data)
{
	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	float mf_max_speed;
	float wait;

	CIP_Data cip_data = CSingleton<CInputParameter>::GetInstance()->GetCIP_Data();

	std::cout << "���t�g�̉���ő呬�x����͂��Ă��������B(0.0�����ŏ����l�A0.0�Ō��̒l)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_lion_down_max_speed;
	if (mf_max_speed > 0.0f) data->mf_down_max_speed = mf_max_speed;

	std::cout << "���t�g�̉���܂ł̑҂����Ԃ���͂��Ă��������B(0.0�����ŏ����l�A0.0�Ō��̒l)\n<float �b> : ";
	std::cin >> wait;
	if (wait < 0.0f) wait = cip_data.M_DATA.mf_lion_down_wait;
	if (wait > 0.0f) data->mf_down_wait = wait;

	std::cout << "���t�g�̏オ��ő呬�x����͂��Ă��������B(0.0�����ŏ����l�A0.0�Ō��̒l)\n<float> : ";
	std::cin >> mf_max_speed;
	if (mf_max_speed < 0.0f) mf_max_speed = cip_data.M_DATA.mf_lion_up_max_speed;
	if (mf_max_speed > 0.0f) data->mf_up_max_speed = mf_max_speed;

	std::cout << "���t�g�̏オ��܂ł̑҂����Ԃ���͂��Ă��������B(0.0�����ŏ����l�A0.0�Ō��̒l)\n<float �b> : ";
	std::cin >> wait;
	if (wait < 0.0f) wait = cip_data.M_DATA.mf_lion_up_wait;
	if (wait > 0.0f) data->mf_up_wait = wait;

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();
}

void CStageEditor::NormalUpdate()
{
	// �{�^���̍X�V
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->Update();
	}

	// �y�[�W�X�V
	for (size_t i = 0; i < this->mvp_pages.size(); i++)
	{
		this->mvp_pages[i]->Update();
	}

	// �J�[�\���X�V
	for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
	{
		this->mvp_cursor_image[j]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// �Z�[�u�{�^��
	if (this->mvp_main_buttons[SEB_SAVE]->GetONOFF()) {
		this->mvp_main_buttons[SEB_SAVE]->SetONOFF(false);
		this->SaveStage();
	}

	// �I���{�^���������ꂽ
	if (this->mvp_main_buttons[SEB_BACK]->GetONOFF()) {
		scene_manager->ChangeScene(SCENE_EDITOR_SELECT);
	}

	// �X�v���C�g�ǉ��{�^��
	if (this->mvp_main_buttons[SEB_ADD_SPRITE]->GetONOFF()) {
		this->mvp_main_buttons[SEB_ADD_SPRITE]->SetONOFF(false);
		this->AddSprite();
	}

	// �X�v���C�g�ݒ�{�^��
	if (this->mvp_main_buttons[SEB_SPRITE_OPTION]->GetONOFF()) {
		this->mvp_main_buttons[SEB_SPRITE_OPTION]->SetONOFF(false);
		g_edit_mode = EM_SPRITE_OPTION;
		g_view_mode = EV_SPRITE_SELECT;
	}

	// �v���C���[�ړ��{�^��
	if (this->mvp_main_buttons[SEB_PLAYER]->GetONOFF()) {
		this->mvp_main_buttons[SEB_PLAYER]->SetONOFF(false);
		this->MovePlayer();
	}

	// �}�b�v�`�b�v�I���{�^��
	if (this->mvp_main_buttons[SEB_MAPCHIP]->GetONOFF()) {
		this->mvp_main_buttons[SEB_MAPCHIP]->SetONOFF(false);
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		g_chip_brush.mi_chip_id = MAPCHIP_SPRITE_NONE;
		g_chip_brush.mf_angle_z = 0.0f;
		g_chip_brush.m_tag = TAG_MAPCHIP;
		g_edit_mode = EM_MAPCHIP_SELECT;
		g_view_mode = EV_SPRITE_SELECT;
	}

	// �M�~�b�N�I���{�^��
	if (this->mvp_main_buttons[SEB_GIMMICK]->GetONOFF()) {
		this->mvp_main_buttons[SEB_GIMMICK]->SetONOFF(false);
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			this->mvp_cursor_image[i]->SetTexture(&g_default_chip);
			this->mvp_cursor_image[i]->SetDefaultPosition(4.0f, 4.0f);

			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = 0.0f;
			this->mvp_cursor_image[i]->SetTransform(&transform);

			UV new_uv = { 0.0f, 0.0f };
			this->mvp_cursor_image[i]->SetAnim(&new_uv);
		}
		g_chip_brush.mi_chip_id = MAPCHIP_SPRITE_NONE;
		g_chip_brush.mf_angle_z = 0.0f;
		g_chip_brush.m_tag = TAG_MAPCHIP;
		g_edit_mode = EM_GIMMIK_SELECT;
		g_view_mode = EV_GIMMIK_SELECT;
	}

	// �w�i�I���{�^��
	if (this->mvp_main_buttons[SEB_BACK_PAGE]->GetONOFF()) {
		this->mvp_main_buttons[SEB_BACK_PAGE]->SetONOFF(false);
		g_edit_mode = EM_BACK_SELECT;
		g_view_mode = EV_SPRITE_SELECT;
	}

	// ���[���h�I���{�^���������ꂽ
	if (this->mvp_main_buttons[SEB_WORLD_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_LEFT]->SetONOFF(false);
		if (gi_world_index > 0) gi_world_index--;
	}
	if (this->mvp_main_buttons[SEB_WORLD_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_RIGHT]->SetONOFF(false);
		if (gi_world_index < gi_world_max - 1) gi_world_index++;
	}
	if (this->mvp_main_buttons[SEB_WORLD_MAX_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_MAX_LEFT]->SetONOFF(false);
		if (gi_world_max > 1) gi_world_max--;
		if (gi_world_max - 1 < gi_world_index) gi_world_index = gi_world_max;
	}
	if (this->mvp_main_buttons[SEB_WORLD_MAX_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_WORLD_MAX_RIGHT]->SetONOFF(false);
		if (gi_world_max < WORLD_MAX) gi_world_max++;
	}

	// ���\�{�^��
	if (this->mvp_main_buttons[SEB_FRONT_BACK]->GetONOFF()) {
		if (gb_is_front) gb_is_front = false;
	}
	else {
		if (!gb_is_front) gb_is_front = true;
	}

	// ����]�{�^��
	if (this->mvp_main_buttons[SEB_ROTATE_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_ROTATE_LEFT]->SetONOFF(false);
		g_chip_brush.mf_angle_z += 90.0f;
		if (g_chip_brush.mf_angle_z >= 360.0f) g_chip_brush.mf_angle_z -= 360.0f;
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = g_chip_brush.mf_angle_z;
			this->mvp_cursor_image[i]->SetTransform(&transform);
		}
	}

	// �E��]�{�^��
	if (this->mvp_main_buttons[SEB_ROTATE_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_ROTATE_RIGHT]->SetONOFF(false);
		g_chip_brush.mf_angle_z -= 90.0f;
		if (g_chip_brush.mf_angle_z < 0.0f) g_chip_brush.mf_angle_z += 360.0f;
		for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
		{
			Transform transform = this->mvp_cursor_image[i]->GetTransform();
			transform.angle.z = g_chip_brush.mf_angle_z;
			this->mvp_cursor_image[i]->SetTransform(&transform);
		}
	}

	// �X�e�[�W�I���{�^���������ꂽ
	if (this->mvp_main_buttons[SEB_STAGE_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_LEFT]->SetONOFF(false);
		if (gi_stage_index > 0) gi_stage_index--;
	}
	if (this->mvp_main_buttons[SEB_STAGE_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_RIGHT]->SetONOFF(false);
		if (gi_stage_index < gi_stage_max - 1) gi_stage_index++;
	}
	if (this->mvp_main_buttons[SEB_STAGE_MAX_LEFT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_MAX_LEFT]->SetONOFF(false);
		if (gi_stage_max > 1) gi_stage_max--;
		if (gi_stage_max - 1 < gi_stage_index) gi_stage_index = gi_stage_max;
	}
	if (this->mvp_main_buttons[SEB_STAGE_MAX_RIGHT]->GetONOFF()) {
		this->mvp_main_buttons[SEB_STAGE_MAX_RIGHT]->SetONOFF(false);
		if (gi_stage_max < WORLD_MAX) gi_stage_max++;
	}

	{
		int world_index = gi_world_index + 1;
		UV value_10 = { static_cast<float>((world_index % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(world_index % 10), 0.0f };
		this->mvp_value_text[VT_WORLD_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_WORLD_01]->SetAnim(&value_01);
	}

	{
		UV value_10 = { static_cast<float>((gi_world_max % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(gi_world_max % 10), 0.0f };
		this->mvp_value_text[VT_WORLD_MAX_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_WORLD_MAX_01]->SetAnim(&value_01);
	}

	{
		int stage_index = gi_stage_index + 1;
		UV value_10 = { static_cast<float>((stage_index % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(stage_index % 10), 0.0f };
		this->mvp_value_text[VT_STAGE_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_STAGE_01]->SetAnim(&value_01);
	}

	{
		UV value_10 = { static_cast<float>((gi_stage_max % 100) / 10), 0.0f };
		UV value_01 = { static_cast<float>(gi_stage_max % 10), 0.0f };
		this->mvp_value_text[VT_STAGE_MAX_10]->SetAnim(&value_10);
		this->mvp_value_text[VT_STAGE_MAX_01]->SetAnim(&value_01);
	}

	this->UpdateStage();

	this->ColliderTest();
}

void CStageEditor::NormalColliderTest()
{
	// ��������I�u�W�F�N�g
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		// ���ꂼ��̃R���C�_�[���擾
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_main_buttons[i]->GetConvertedCollider();

		// �����蔻�茋��
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true �̂Ƃ��������Ă܂��B
		if (hit.is_hit) {
			this->mvp_main_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::NormalDraw()
{
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_main_buttons.size(); i++)
	{
		this->mvp_main_buttons[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_pages.size(); i++)
	{
		this->mvp_pages[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_mapchip.size(); i++)
	{
		this->mvp_mapchip[i]->Draw();
	}

	std::vector<CImage*>* vp_ap_gimmik[6];
	vp_ap_gimmik[0] = &this->mvp_goal;
	vp_ap_gimmik[1] = &this->mvp_piece;
	vp_ap_gimmik[2] = &this->mvp_tornado;
	vp_ap_gimmik[3] = &this->mvp_dharma;
	vp_ap_gimmik[4] = &this->mvp_cannon;
	vp_ap_gimmik[5] = &this->mvp_lion;
	for (size_t i = 0; i < 6; i++)
	{
		for (size_t j = 0; j < vp_ap_gimmik[i]->size(); j++)
		{
			(*vp_ap_gimmik[i])[j]->Draw();
		}
	}

	if (gb_is_front) {
		for (size_t i = 0; i < this->mvp_player.size(); i++)
		{
			this->mvp_player[i]->Draw();
		}
	}
	for (size_t i = 0; i < this->mvp_text.size(); i++)
	{
		this->mvp_text[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_value_text.size(); i++)
	{
		this->mvp_value_text[i]->Draw();
	}
	this->mp_cursor->Draw();
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->Draw();
	}
}

void CStageEditor::SpriteOptionUpdate()
{
	// �{�^���̍X�V
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// �߂�{�^��
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		g_edit_mode = EM_NONE;
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::SpriteOptionColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// ��������I�u�W�F�N�g
		for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
		{
			// ���ꂼ��̃R���C�_�[���擾
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip_image[i]->GetConvertedCollider();

			// �����蔻�茋��
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true �̂Ƃ��������Ă܂��B
			if (hit.is_hit) {
				this->ChangeSprite(i);
			}
		}
	}

	// ��������I�u�W�F�N�g
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// ���ꂼ��̃R���C�_�[���擾
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// �����蔻�茋��
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true �̂Ƃ��������Ă܂��B
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::SpriteOptionDraw()
{
	for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
	{
		this->mvp_mapchip_image[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Draw();
	}
	this->mp_cursor->Draw();
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->Draw();
	}
}

void CStageEditor::MapChipSelectUpdate()
{
	// �{�^���̍X�V
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	// �J�[�\���X�V
	for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
	{
		this->mvp_cursor_image[j]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// �߂�{�^��
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		if (g_chip_brush.mi_chip_id == MAPCHIP_SPRITE_NONE) {
			g_edit_mode = EM_NONE;
		}
		else {
			g_edit_mode = EM_MAPCHIP;
		}
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::MapChipSelectColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// ��������I�u�W�F�N�g
		for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
		{
			// ���ꂼ��̃R���C�_�[���擾
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip_image[i]->GetConvertedCollider();

			// �����蔻�茋��
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true �̂Ƃ��������Ă܂��B
			if (hit.is_hit) {
				g_chip_brush.mi_chip_id = static_cast<int>(i);
				g_chip_brush.mf_angle_z = 0.0f;
				g_chip_brush.m_tag = TAG_MAPCHIP;

				for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
				{
					this->mvp_cursor_image[j]->SetTexture(&gv_sprite_data[g_chip_brush.mi_chip_id]);
					this->mvp_cursor_image[j]->SetDefaultPosition(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);
				}

			}
		}
	}

	// ��������I�u�W�F�N�g
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// ���ꂼ��̃R���C�_�[���擾
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// �����蔻�茋��
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true �̂Ƃ��������Ă܂��B
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::MapChipUpdate()
{
	this->NormalUpdate();
}

void CStageEditor::MapChipColliderTest()
{
	if (this->mp_input->GetKeyPress(VK_LBUTTON)) {
		// ��������I�u�W�F�N�g
		for (size_t i = 0; i < this->mvp_mapchip.size(); i++)
		{
			// ���ꂼ��̃R���C�_�[���擾
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip[i]->GetConvertedCollider();

			// �����蔻�茋��
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true �̂Ƃ��������Ă܂��B
			if (hit.is_hit) {
				int x = static_cast<int>(i) % (MAPCHIP_NUM_WIDTH * AREA_NUM);
				int y = static_cast<int>(i) / (MAPCHIP_NUM_WIDTH * AREA_NUM);

				if (this->mvp_main_buttons[SEB_PAINT]->GetONOFF())
				{
					if (gb_is_front) {
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = g_chip_brush.mi_chip_id;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;
					}
					else {
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = g_chip_brush.mi_chip_id;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;
					}
				}
				else if (this->mvp_main_buttons[SEB_ERASE]->GetONOFF())
				{
					if (gb_is_front) {
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = 0.0f;
						ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = TAG_MAPCHIP;
					}
					else {
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = 0.0f;
						ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = TAG_MAPCHIP;
					}
				}
			}
		}
	}
	this->NormalColliderTest();
}

void CStageEditor::BackSelectUpdate()
{
	// �{�^���̍X�V
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// �߂�{�^��
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		g_edit_mode = EM_NONE;
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::BackSelectColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// ��������I�u�W�F�N�g
		for (size_t i = 0; i < this->mvp_mapchip_image.size(); i++)
		{
			// ���ꂼ��̃R���C�_�[���擾
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_mapchip_image[i]->GetConvertedCollider();

			// �����蔻�茋��
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true �̂Ƃ��������Ă܂��B
			if (hit.is_hit) {
				if (gb_is_front) ga_stage_data[gi_world_index][gi_stage_index].mi_front_back = static_cast<int>(i);
				else ga_stage_data[gi_world_index][gi_stage_index].mi_back_back = static_cast<int>(i);
			}
		}
	}

	// ��������I�u�W�F�N�g
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// ���ꂼ��̃R���C�_�[���擾
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// �����蔻�茋��
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true �̂Ƃ��������Ă܂��B
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::GimmikSelectUpdate()
{
	// �{�^���̍X�V
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Update();
	}

	// �J�[�\���X�V
	for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
	{
		this->mvp_cursor_image[j]->Update();
	}

	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// �߂�{�^��
	if (this->mvp_select_buttons[SSB_BACK]->GetONOFF()) {
		this->mvp_select_buttons[SSB_BACK]->SetONOFF(false);
		if (g_chip_brush.m_tag == TAG_MAPCHIP) {
			g_edit_mode = EM_NONE;
		}
		else {
			g_edit_mode = EM_GIMMIK;
		}
		g_view_mode = EV_NONE;
	}

	this->ColliderTest();
}

void CStageEditor::GimmikSelectColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// ��������I�u�W�F�N�g
		for (size_t i = 0; i < this->mvp_gimmik_image.size(); i++)
		{
			// ���ꂼ��̃R���C�_�[���擾
			Collider obj_collider = this->mp_cursor->GetConvertedCollider();
			Collider other_collider = this->mvp_gimmik_image[i]->GetConvertedCollider();

			// �����蔻�茋��
			COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

			// is_hit == true �̂Ƃ��������Ă܂��B
			if (hit.is_hit) {
				g_chip_brush.mi_chip_id = MAPCHIP_NONE;
				g_chip_brush.mf_angle_z = 0.0f;

				switch (i)
				{
				case GSI_GOAL:
					g_chip_brush.m_tag = TAG_GOAL;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_GOAL);
						this->mvp_cursor_image[j]->SetDefaultPosition(GOAL_WIDTH, GOAL_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_PIECE_1:
					g_chip_brush.m_tag = TAG_PIECE_1;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_PIECE_0);
						this->mvp_cursor_image[j]->SetDefaultPosition(PIECE_0_WIDTH, PIECE_0_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_PIECE_2:
					g_chip_brush.m_tag = TAG_PIECE_2;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_PIECE_0);
						this->mvp_cursor_image[j]->SetDefaultPosition(PIECE_0_WIDTH, PIECE_0_HEIGHT);

						UV new_uv = { 1.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_PIECE_3:
					g_chip_brush.m_tag = TAG_PIECE_3;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_PIECE_0);
						this->mvp_cursor_image[j]->SetDefaultPosition(PIECE_0_WIDTH, PIECE_0_HEIGHT);

						UV new_uv = { 2.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_RED_BLOCK:
					g_chip_brush.m_tag = TAG_RED_BLOCK;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_RED_BULE);
						this->mvp_cursor_image[j]->SetDefaultPosition(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_BLUE_BLOCK:
					g_chip_brush.m_tag = TAG_BLUE_BLOCK;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_RED_BULE);
						this->mvp_cursor_image[j]->SetDefaultPosition(MAPCHIP_WIDTH, MAPCHIP_HEIGHT);

						UV new_uv = { 1.0f, 1.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_TORNADO:
					g_chip_brush.m_tag = TAG_TORNADO;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_TORNADO);
						this->mvp_cursor_image[j]->SetDefaultPosition(TORNADE_WIDTH, TORNADE_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_DHARMA:
					g_chip_brush.m_tag = TAG_DHARMA;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_DHARMA);
						this->mvp_cursor_image[j]->SetDefaultPosition(DHARMA_WIDTH, DHARMA_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_CANNON:
					g_chip_brush.m_tag = TAG_CANNON;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_CANNON);
						this->mvp_cursor_image[j]->SetDefaultPosition(CANNON_WIDTH, CANNON_HEIGHT);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_LION_TOP:
					g_chip_brush.m_tag = TAG_LION_TOP;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_LION_TOP);
						this->mvp_cursor_image[j]->SetDefaultPosition(LION_WIDTH, LION_HEIGHT);

						//UV new_uv = { 0.0f, 0.0f };
						//this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				case GSI_LION_BOTTOM:
					g_chip_brush.m_tag = TAG_LION_BOTTOM;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->ChangeTexture(SPRITE_LION_BOTTOM);
						this->mvp_cursor_image[j]->SetDefaultPosition(LION_WIDTH, LION_HEIGHT);

						//UV new_uv = { 0.0f, 1.0f };
						//this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				default:
					g_chip_brush.m_tag = TAG_MAPCHIP;
					for (size_t j = 0; j < this->mvp_cursor_image.size(); j++)
					{
						this->mvp_cursor_image[j]->SetTexture(&g_default_chip);
						this->mvp_cursor_image[j]->SetDefaultPosition(4.0f, 4.0f);

						UV new_uv = { 0.0f, 0.0f };
						this->mvp_cursor_image[j]->SetAnim(&new_uv);
					}
					break;
				}
			}
		}
	}

	// ��������I�u�W�F�N�g
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		// ���ꂼ��̃R���C�_�[���擾
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_select_buttons[i]->GetConvertedCollider();

		// �����蔻�茋��
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true �̂Ƃ��������Ă܂��B
		if (hit.is_hit) {
			this->mvp_select_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}

void CStageEditor::GimmikSelectDraw()
{
	for (size_t i = 0; i < this->mvp_gimmik_image.size(); i++)
	{
		this->mvp_gimmik_image[i]->Draw();
	}
	for (size_t i = 0; i < this->mvp_select_buttons.size(); i++)
	{
		this->mvp_select_buttons[i]->Draw();
	}
	this->mp_cursor->Draw();
	for (size_t i = 0; i < this->mvp_cursor_image.size(); i++)
	{
		this->mvp_cursor_image[i]->Draw();
	}
}

void CStageEditor::GimmikUpdate()
{
	this->NormalUpdate();
}

void CStageEditor::GimmikColliderTest()
{
	if (this->mp_input->GetKeyTrigger(VK_LBUTTON)) {
		// �y�C���g���[�h�ƃC���C�Y���[�h�ŕς���
		if (this->mvp_main_buttons[SEB_PAINT]->GetONOFF())
		{
			// ��������I�u�W�F�N�g
			for (int i = 0; i < this->mvp_mapchip.size(); i++)
			{
				// ���ꂼ��̃R���C�_�[���擾
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_mapchip[i]->GetConvertedCollider();

				// �����蔻�茋��
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true �̂Ƃ��������Ă܂��B
				if (hit.is_hit) {
					int x = i % (MAPCHIP_NUM_WIDTH * AREA_NUM);
					int y = i / (MAPCHIP_NUM_WIDTH * AREA_NUM);

					switch (g_chip_brush.m_tag)
					{
					case TAG_GOAL:
					{
						GoalData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj.emplace_back(data);
						}
					}
					break;
					case TAG_PIECE_1:
					{
						PieceData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.i_piece_num = 0;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.emplace_back(data);
						}
					}
					break;
					case TAG_PIECE_2:
					{
						PieceData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.i_piece_num = 1;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.emplace_back(data);
						}
					}
					break;
					case TAG_PIECE_3:
					{
						PieceData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.i_piece_num = 2;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.emplace_back(data);
						}
					}
					break;
					case TAG_RED_BLOCK:
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						break;
					case TAG_BLUE_BLOCK:
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_FACTORY;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = g_chip_brush.mf_angle_z;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;
						}
						break;
					case TAG_TORNADO:
					{
						TornadeData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj.emplace_back(data);
						}
					}
					break;
					case TAG_DHARMA:
					{
						DharmaData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.mf_max_speed = DEFAULT_INPUT_VALUE;
						data.mb_is_left = true;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj.emplace_back(data);
						}
					}
					break;
					case TAG_CANNON:
					{
						CannonData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.mf_angle_z = g_chip_brush.mf_angle_z;
						data.mf_max_speed = DEFAULT_INPUT_VALUE;
						data.mf_wait = DEFAULT_INPUT_VALUE;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj.emplace_back(data);
						}
					}
					break;
					case TAG_LION_TOP:
					{
						if (x % MAPCHIP_NUM_WIDTH == 0 || x % MAPCHIP_NUM_WIDTH == MAPCHIP_NUM_WIDTH - 1) break;

						LionData data;

						data.position = this->mvp_mapchip[i]->GetTransform().position;
						data.mf_up_max_speed = DEFAULT_INPUT_VALUE;
						data.mf_down_max_speed = DEFAULT_INPUT_VALUE;
						data.mf_up_wait = DEFAULT_INPUT_VALUE;
						data.mf_down_wait = DEFAULT_INPUT_VALUE;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj.emplace_back(data);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj.emplace_back(data);
						}
					}
					break;
					case TAG_LION_BOTTOM:
						if (x % MAPCHIP_NUM_WIDTH == 0 || x % MAPCHIP_NUM_WIDTH == MAPCHIP_NUM_WIDTH - 1) break;

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x - 1][y].mi_chip_id = LEFT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x - 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x - 1][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = CENTER_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x + 1][y].mi_chip_id = RIGHT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x + 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x + 1][y].m_tag = g_chip_brush.m_tag;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x - 1][y].mi_chip_id = LEFT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x - 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x - 1][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = CENTER_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = g_chip_brush.m_tag;

							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x + 1][y].mi_chip_id = RIGHT_LION_BOTTOM;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x + 1][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x + 1][y].m_tag = g_chip_brush.m_tag;
						}
						break;
					default:
						break;
					}
				}

			}
		}
		else if (this->mvp_main_buttons[SEB_ERASE]->GetONOFF())
		{
			switch (g_chip_brush.m_tag)
			{
			case TAG_GOAL:
				// ��������I�u�W�F�N�g
				for (int i = 0; i < this->mvp_goal.size(); i++)
				{
					// ���ꂼ��̃R���C�_�[���擾
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_goal[i]->GetConvertedCollider();

					// �����蔻�茋��
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true �̂Ƃ��������Ă܂��B
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_goal_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_goal_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_PIECE_1:
			case TAG_PIECE_2:
			case TAG_PIECE_3:
				// ��������I�u�W�F�N�g
				for (int i = 0; i < this->mvp_piece.size(); i++)
				{
					// ���ꂼ��̃R���C�_�[���擾
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_piece[i]->GetConvertedCollider();

					// �����蔻�茋��
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true �̂Ƃ��������Ă܂��B
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_piece_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_piece_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_TORNADO:
				// ��������I�u�W�F�N�g
				for (int i = 0; i < this->mvp_tornado.size(); i++)
				{
					// ���ꂼ��̃R���C�_�[���擾
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_tornado[i]->GetConvertedCollider();

					// �����蔻�茋��
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true �̂Ƃ��������Ă܂��B
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_tornade_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_tornade_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_DHARMA:
				// ��������I�u�W�F�N�g
				for (int i = 0; i < this->mvp_dharma.size(); i++)
				{
					// ���ꂼ��̃R���C�_�[���擾
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_dharma[i]->GetConvertedCollider();

					// �����蔻�茋��
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true �̂Ƃ��������Ă܂��B
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_CANNON:
				// ��������I�u�W�F�N�g
				for (int i = 0; i < this->mvp_cannon.size(); i++)
				{
					// ���ꂼ��̃R���C�_�[���擾
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_cannon[i]->GetConvertedCollider();

					// �����蔻�茋��
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true �̂Ƃ��������Ă܂��B
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_LION_TOP:
				// ��������I�u�W�F�N�g
				for (int i = 0; i < this->mvp_lion.size(); i++)
				{
					// ���ꂼ��̃R���C�_�[���擾
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_lion[i]->GetConvertedCollider();

					// �����蔻�茋��
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true �̂Ƃ��������Ă܂��B
					if (hit.is_hit) {
						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj.begin() + i);
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj.erase(ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj.begin() + i);
						}
						break;
					}
				}
				break;
			case TAG_RED_BLOCK:
			case TAG_BLUE_BLOCK:
			case TAG_LION_BOTTOM:
				// ��������I�u�W�F�N�g
				for (int i = 0; i < this->mvp_mapchip.size(); i++)
				{
					// ���ꂼ��̃R���C�_�[���擾
					Collider obj_collider = this->mp_cursor->GetConvertedCollider();
					Collider other_collider = this->mvp_mapchip[i]->GetConvertedCollider();

					// �����蔻�茋��
					COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

					// is_hit == true �̂Ƃ��������Ă܂��B
					if (hit.is_hit) {
						int x = i % (MAPCHIP_NUM_WIDTH * AREA_NUM);
						int y = i / (MAPCHIP_NUM_WIDTH * AREA_NUM);

						if (gb_is_front) {
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_front_data[x][y].m_tag = TAG_MAPCHIP;
						}
						else {
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mi_chip_id = MAPCHIP_SPRITE_NONE;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].mf_angle_z = 0.0f;
							ga_stage_data[gi_world_index][gi_stage_index].mv_back_data[x][y].m_tag = TAG_MAPCHIP;
						}
					}
				}
				break;
			default:
				break;
			}
		}
	}
	if (this->mp_input->GetKeyTrigger(VK_RBUTTON)) {
		switch (g_chip_brush.m_tag)
		{
		case TAG_DHARMA:
			// ��������I�u�W�F�N�g
			for (int i = 0; i < this->mvp_dharma.size(); i++)
			{
				// ���ꂼ��̃R���C�_�[���擾
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_dharma[i]->GetConvertedCollider();

				// �����蔻�茋��
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true �̂Ƃ��������Ă܂��B
				if (hit.is_hit) {
					if (gb_is_front) {
						this->InputDharmaData(&ga_stage_data[gi_world_index][gi_stage_index].mv_front_dharma_obj[i]);
					}
					else {
						this->InputDharmaData(&ga_stage_data[gi_world_index][gi_stage_index].mv_back_dharma_obj[i]);
					}
					break;
				}
			}
			break;
		case TAG_CANNON:
			// ��������I�u�W�F�N�g
			for (int i = 0; i < this->mvp_cannon.size(); i++)
			{
				// ���ꂼ��̃R���C�_�[���擾
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_cannon[i]->GetConvertedCollider();

				// �����蔻�茋��
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true �̂Ƃ��������Ă܂��B
				if (hit.is_hit) {
					if (gb_is_front) {
						this->InputCannonData(&ga_stage_data[gi_world_index][gi_stage_index].mv_front_cannon_obj[i]);
					}
					else {
						this->InputCannonData(&ga_stage_data[gi_world_index][gi_stage_index].mv_back_cannon_obj[i]);
					}
					break;
				}
			}
			break;
		case TAG_LION_TOP:
			// ��������I�u�W�F�N�g
			for (int i = 0; i < this->mvp_lion.size(); i++)
			{
				// ���ꂼ��̃R���C�_�[���擾
				Collider obj_collider = this->mp_cursor->GetConvertedCollider();
				Collider other_collider = this->mvp_lion[i]->GetConvertedCollider();

				// �����蔻�茋��
				COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

				// is_hit == true �̂Ƃ��������Ă܂��B
				if (hit.is_hit) {
					if (gb_is_front) {
						this->InputLionTopData(&ga_stage_data[gi_world_index][gi_stage_index].mv_front_lion_obj[i]);
					}
					else {
						this->InputLionTopData(&ga_stage_data[gi_world_index][gi_stage_index].mv_back_lion_obj[i]);
					}
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	this->NormalColliderTest();
}

