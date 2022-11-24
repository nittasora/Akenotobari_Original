#include "CEditorSample.h"

#include <fstream>
#include <iostream>
#include <string>

#include "CStageManager.h"
#include "CInput.h"
#include "CButton.h"
#include "CCursor.h"

#include "CDirect3D.h"
#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"

#include "CSingleton.h"

CEditorSample::CEditorSample()
{
	this->mp_stage_manager = nullptr;
}

CEditorSample::~CEditorSample()
{
}

void CEditorSample::Initialize()
{
	FILE* ifp;
	FILE* ofp;

	// �R���\�[�����쐬
	AllocConsole();
	freopen_s(&ifp, "CONOUT$", "w", stdout);
	freopen_s(&ofp, "CONIN$", "r", stdin);

	// �ǂݍ��ރ��[���h�ԍ�����͂�����
	int world = 0;
	std::cout << "���[���h�ԍ�����͂��Ă������� : \n";
	std::cin >> world;
	std::cout << std::endl;
	world--;

	// �ǂݍ��ރX�e�[�W�ԍ�����͂�����
	int stage = 0;
	std::cout << "�X�e�[�W�ԍ�����͂��Ă������� : \n";
	std::cin >> stage;
	std::cout << std::endl;
	stage--;

	// ���[���h�ԍ��ƃX�e�[�W�ԍ���ݒ�
	CGameManager* game_manager = CSingleton<CGameManager>::GetInstance();
	game_manager->SetWorldIndex(world);
	game_manager->SetStageIndex(stage);

	// �R���\�[���p�E�B���h�E�̏I��
	fclose(ifp);
	fclose(ofp);
	FreeConsole();

	// �X�e�[�W�}�l�[�W���[�C���X�^���X�擾
	this->mp_stage_manager = new CStageManager;

	// CDirect3D�擾
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// �X�N���[���T�C�Y�i�[
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	// ���ǂ�{�^���̍쐬
	{
		CButton* new_image = new CButton;
		new_image->Initilize();

		// �{�^���̑傫����ς���
		float width = BUTTON_W_WIDTH;
		float height = BUTTON_W_HEIGHT;
		new_image->SetDefaultPosAndColl(width, height);

		// �{�^���̍��W��ς���
		Transform transform = new_image->GetTransform();
		transform.position.y = (screen_height * 0.5f) - (height * 1.5f);
		new_image->SetTransform(&transform);

		// �{�^���̉摜�Ǝ󂯕t������͂̐ݒ�
		new_image->SetButton(VK_LBUTTON, XI_BUTTON_B);
		new_image->SetSprite(SPRITE_TEST_BUTTON_W_0, { 2.0f, 0.0f }, true);

		// �{�^���o�^
		this->mvp_button.emplace_back(new_image);
	}
}

void CEditorSample::Finalize()
{
	// �X�e�[�W�}�l�[�W���[�̉��
	if (this->mp_stage_manager != nullptr) {
		delete this->mp_stage_manager;
		this->mp_stage_manager = nullptr;
	}

	// �{�^���̊J��
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		this->mvp_button[i]->UnInitilize();
		delete this->mvp_button[i];
	}
	this->mvp_button.clear();
}

void CEditorSample::Update()
{
	// �{�^���̍X�V
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		this->mvp_button[i]->Update();
	}

	this->ColliderTest();

	// �{�^����ON�Ȃ�΃G�f�B�^�[�̑I����ʂ�
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		if (this->mvp_button[i]->GetONOFF()) {
			this->mvp_button[i]->SetONOFF(false);
			CSingleton<CSceneManager>::GetInstance()->ChangeScene(SCENE_EDITOR_SELECT);
		}
	}

	// �X�e�[�W�̍X�V
	this->mp_stage_manager->Update();
}

void CEditorSample::Draw()
{
	// �X�e�[�W�̕`��
	this->mp_stage_manager->Draw();

	// �{�^���̕`��
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		this->mvp_button[i]->Draw();
	}
}

void CEditorSample::PauseUpdate()
{
	// Pause���͉������Ȃ�
}

void CEditorSample::PauseDraw()
{
	// Pause�����`��͂���
	this->Draw();
}

void CEditorSample::ColliderTest()
{
	// �J�[�\���̎擾
	CCursor* cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();

	// �{�^���ƃJ�[�\���̓����蔻��
	for (size_t i = 0; i < this->mvp_button.size(); i++)
	{
		// ���ꂼ��̃R���C�_�[���擾
		Collider obj_collider = cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_button[i]->GetConvertedCollider();

		// �����蔻�茋��
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true �̂Ƃ��������Ă܂��B
		if (hit.is_hit) {
			this->mvp_button[i]->OnCollision(&hit, cursor);
		}
	}
}
