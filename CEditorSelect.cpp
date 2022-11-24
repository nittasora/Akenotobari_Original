#include "CEditorSelect.h"

#include "CDirect3D.h"
#include"CInput.h"

#include "CImage.h"
#include "CButton.h"
#include "CCursor.h"

#include "CSceneManager.h"
#include "CGameManager.h"
#include "CMenuManager.h"

#include "CSingleton.h"

// ���ꂼ��̃{�^���̖���
enum EDITOR_SELECT_BUTTON
{
	ESB_STAGE,			// �X�e�[�W�G�f�B�^�\��
	ESB_EDITOR_SAMPLE,	// �T���v����
	ESB_FINISH,			// �I��
	ESB_MAX
};

CEditorSelect::CEditorSelect()
{
}

CEditorSelect::~CEditorSelect()
{
}

void CEditorSelect::Initialize()
{
	// �ꉞ
	this->mvp_images.clear();
	this->mvp_buttons.clear();

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

		// �摜�̑傫����ς���
		new_image->SetDefaultPosAndColl(screen_width, screen_height);

		// �摜�I�u�W�F�N�g�̓o�^
		this->mvp_images.emplace_back(new_image);
	}

	// �{�^�������炩���ߗp��
	this->mvp_buttons.resize(ESB_MAX);
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		// �{�^���I�u�W�F�N�g����
		this->mvp_buttons[i] = new CButton;
		this->mvp_buttons[i]->Initilize();

		// �{�^���̑傫����ς���
		float width = BUTTON_W_WIDTH;
		float height = BUTTON_W_HEIGHT;
		this->mvp_buttons[i]->SetDefaultPosAndColl(width, height);

		// �{�^���̍��W��ύX
		Transform transform = this->mvp_buttons[i]->GetTransform();
		transform.position.x = (-screen_width * 0.5f) + (width * 1.5f) * static_cast<float>(i % 8 + 1);
		transform.position.y = (screen_height * 0.5f) - (height * 1.5f) * static_cast<float>(i / 8 + 1);
		this->mvp_buttons[i]->SetTransform(&transform);

		// �{�^���̓o�^
		this->mvp_buttons[i]->SetButton(VK_LBUTTON, XI_BUTTON_B);
	}

	// �{�^����p�̃e�N�X�`���ݒ�֐�
	this->mvp_buttons[ESB_STAGE]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 1.0f, 0.0f }, true);
	this->mvp_buttons[ESB_EDITOR_SAMPLE]->SetSprite(SPRITE_TEST_BUTTON_W_1, { 4.0f, 2.0f }, true);
	this->mvp_buttons[ESB_FINISH]->SetSprite(SPRITE_TEST_BUTTON_W_0, { 2.0f, 0.0f }, true);

	// �J�[�\���擾
	this->mp_cursor = CSingleton<CGameManager>::GetInstance()->GetCursor();
}

void CEditorSelect::Finalize()
{
	// �摜�I�u�W�F�N�g�̊J��
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->UnInitilize();
		delete this->mvp_images[i];
	}
	this->mvp_images.clear();

	// �{�^���̉��
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->UnInitilize();
		delete this->mvp_buttons[i];
	}
	this->mvp_buttons.clear();

	// �J�[�\���������
	this->mp_cursor = nullptr;
}

void CEditorSelect::Update()
{
	// �{�^���̍X�V
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->Update();
	}

	// �C���X�^���X�擾
	CSceneManager* scene_manager = CSingleton<CSceneManager>::GetInstance();
	CMenuManager* menu_manager = CSingleton<CMenuManager>::GetInstance();

	// �X�e�[�W�쐬�{�^���������ꂽ
	if (this->mvp_buttons[ESB_STAGE]->GetONOFF()) {
		// �X�e�[�W�G�f�B�^�\��
		scene_manager->ChangeScene(SCENE_STAGE_EDITOR);
	}

	// �G�f�B�^�[�T���v���{�^���������ꂽ
	if (this->mvp_buttons[ESB_EDITOR_SAMPLE]->GetONOFF()) {
		// �T���v����
		scene_manager->ChangeScene(SCENE_EDITOR_SAMPLE);
	}

	// �I���{�^���������ꂽ
	if (this->mvp_buttons[ESB_FINISH]->GetONOFF()) {
		// �Q�[�����[�v�I��
		scene_manager->FinishLoop();
	}

	this->ColliderTest();
}

void CEditorSelect::Draw()
{
	// �摜�̕`��
	for (size_t i = 0; i < this->mvp_images.size(); i++)
	{
		this->mvp_images[i]->Draw();
	}

	// �{�^���̕`��
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->Draw();
	}
}

void CEditorSelect::PauseUpdate()
{
	// Pause���Ƀ{�^����߂�
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		this->mvp_buttons[i]->SetONOFF(false);
	}
}

void CEditorSelect::PauseDraw()
{
	this->Draw();
}

void CEditorSelect::ColliderTest()
{
	// �{�^���ƃJ�[�\���̓����蔻��
	for (size_t i = 0; i < this->mvp_buttons.size(); i++)
	{
		// ���ꂼ��̃R���C�_�[���擾
		Collider obj_collider = this->mp_cursor->GetConvertedCollider();
		Collider other_collider = this->mvp_buttons[i]->GetConvertedCollider();

		// �����蔻�茋��
		COLLIDER_RESULT hit = Collider_Test(&obj_collider, &other_collider);

		// is_hit == true �̂Ƃ��������Ă܂��B
		if (hit.is_hit) {
			this->mvp_buttons[i]->OnCollision(&hit, this->mp_cursor);
		}
	}
}
