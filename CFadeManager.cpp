#include "CFadeManager.h"

#include "CDirect3D.h"

#include "CGameObject.h"
#include "CImage.h"
#include "CPlayerRunImage.h"

#include "CSingleton.h"

// �t�F�[�h�Ɏg��
enum FADE_IMAGE
{
	FADE_IMAGE_NORMAL,	// �ʏ�I�u�W�F�N�g
	FADE_IMAGE_PLAYER,	// �v���C���[�摜
	FADE_IMAGE_MAX
};

CFadeManager::CFadeManager()
{
	// �t�F�[�h�̏󋵂�������
	this->m_data.m_condeition = FADE_OUT_END;
	this->m_data.m_next_condeition = FADE_OUT_END;
	this->m_data.m_type = FADE_TYPE_00;
	this->m_data.mi_fade_count = 0;

	// CDirect3D�擾
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// �X�N���[���T�C�Y�i�[
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	// �O�I�u�W�F�N�g���ʕ���
	this->m_object.mvp_image.resize(FADE_IMAGE_MAX);
	for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
	{
		CImage* new_image;
		switch (i)
		{
			// �v���C���[�摜
		case FADE_IMAGE_PLAYER:
			new_image = new CPlayerRunImage;
			break;
			// �ʏ�摜
		default:
			new_image = new CImage;
			break;
		}
		new_image->Initilize();


		// �I�u�W�F�N�g�̕`����~�߂�
		obj_flg flg = OBJ_FLG_ACTIVE;
		new_image->SetFlg(&flg);

		// �o�^
		this->m_object.mvp_image[i] = new_image;
		this->m_object.mvp_delete_obj.emplace_back(new_image);
	}

	// FADE_IMAGE_NORMAL �ɂ���
	{
		// �摜�ݒ�
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->AddTexture(SPRITE_TEST_2);

		// �F�ύX
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// �傫���ύX
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetDefaultPosAndColl(screen_width, screen_height);
	}

	{
		// �傫���ύX
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetDefaultPosAndColl(PLAYER_RUN_WIDTH * 2, PLAYER_RUN_HEIGHT * 2);
	}
}

CFadeManager::~CFadeManager()
{
	// �I�u�W�F�N�g�̊J��
	for (size_t i = 0; i < this->m_object.mvp_delete_obj.size(); i++)
	{
		this->m_object.mvp_delete_obj[i]->UnInitilize();
		delete this->m_object.mvp_delete_obj[i];
	}
}

void CFadeManager::Update()
{
	// ���̏�ԂɈڍs
	if (this->m_data.m_condeition != this->m_data.m_next_condeition) this->m_data.m_condeition = this->m_data.m_next_condeition;

	// ��ޕʃt�F�[�h�̍X�V������
	switch (this->m_data.m_type)
	{
	case FADE_TYPE_00:
		this->UpdateType00();
		break;
	case FADE_TYPE_01:
		this->UpdateType01();
		break;
	case FADE_TYPE_02:
		this->UpdateType02();
		break;
	}
}

void CFadeManager::Draw()
{
	// ��ޕʃt�F�[�h�̕`�揈����
	switch (this->m_data.m_type)
	{
	case FADE_TYPE_00:
		this->DrawType00();
		break;
	case FADE_TYPE_01:
		this->DrawType01();
		break;
	case FADE_TYPE_02:
		this->DrawType02();
		break;
	}
}

void CFadeManager::StartFadeIn()
{
	// �t�F�[�h�A�E�g���I����Ă��Ȃ���΃t�F�[�h�C���ł��Ȃ�
	if (this->m_data.m_condeition != FADE_OUT_END) return;

	// �t�F�[�h�C���̏�����
	this->m_data.m_next_condeition = FADE_IN_PREPARE;
}

void CFadeManager::StartFadeOut()
{
	// �t�F�[�h�C�����I����Ă��Ȃ���΃t�F�[�h�A�E�g�ł��Ȃ�
	if (this->m_data.m_condeition != FADE_IN_END) return;

	// �t�F�[�h�A�E�g�̏�����
	this->m_data.m_next_condeition = FADE_OUT_PREPARE;
}

void CFadeManager::StartFadeOut(FADE_TYPE type)
{
	// �t�F�[�h�C�����I����Ă��Ȃ���΃t�F�[�h�A�E�g�ł��Ȃ�
	if (this->m_data.m_condeition != FADE_IN_END) return;

	// �t�F�[�h�̎�ނ�ύX
	if (type >= 0 && type < FADE_TYPE_MAX) this->m_data.m_type = type;

	// �t�F�[�h�A�E�g�̏�����
	this->m_data.m_next_condeition = FADE_OUT_PREPARE;
}

bool CFadeManager::CheckFadeInEnd()
{
	// �t�F�[�h�C�����I��������ǂ���
	return this->m_data.m_condeition == FADE_IN_END;
}

bool CFadeManager::CheckFadeOutEnd()
{
	// �t�F�[�h�A�E�g���I��������ǂ���
	return this->m_data.m_condeition == FADE_OUT_END;
}

void CFadeManager::SetType(FADE_TYPE type)
{
	// �t�F�[�h�������Ă���Ԃ̓t�F�[�h�̎�ނ�ς����Ȃ�
	if (this->m_data.m_condeition != FADE_IN_END) return;

	// �t�F�[�h�̎�ނ�ύX
	if (type >= 0 && type < FADE_TYPE_MAX) this->m_data.m_type = type;
}

void CFadeManager::UpdateType00()
{
	// CDirect3D�擾
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// �X�N���[���T�C�Y�i�[
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	switch (this->m_data.m_condeition)
	{
		//==============================
		// �t�F�[�h�C������
		//==============================
	case FADE_IN_PREPARE:
	{
		// ���F�ɂ���
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// �^�񒆂ɂ���
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// ��x���ׂĂ̕`����~�߂�
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// �I�u�W�F�N�g�̕`�������悤�ɂ���
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// �t�F�[�h�C�����s��
		this->m_data.m_next_condeition = FADE_IN_ACT;
	}
	break;
	//==============================
	// �t�F�[�h�C���I���܂�
	//==============================
	case FADE_IN_ACT:
	{
		// ���F�ɂ���
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a -= 0.05f;

		if (color.a <= 0.0f) {
			color.a = 0.0f;

			// �t�F�[�h�C���I����
			this->m_data.m_next_condeition = FADE_IN_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// �t�F�[�h�C���I����
	//==============================
	case FADE_IN_END:
		break;
		//==============================
		// �t�F�[�h�A�E�g����
		//==============================
	case FADE_OUT_PREPARE:
	{
		// ���F�ɂ���
		Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// �^�񒆂ɂ���
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// ��x���ׂĂ̕`����~�߂�
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// �I�u�W�F�N�g�̕`�������悤�ɂ���
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// �t�F�[�h�A�E�g���s��
		this->m_data.m_next_condeition = FADE_OUT_ACT;
	}
	break;
	//==============================
	// �t�F�[�h�A�E�g�I���܂�
	//==============================
	case FADE_OUT_ACT:
	{
		// ���F�ɂ���
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a += 0.05f;

		if (color.a >= 1.0f) {
			color.a = 1.0f;

			// �t�F�[�h�C���I����
			this->m_data.m_next_condeition = FADE_OUT_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// �t�F�[�h�A�E�g�I����
	//==============================
	case FADE_OUT_END:
		break;
	}
}

void CFadeManager::DrawType00()
{
	this->m_object.mvp_image[FADE_IMAGE_NORMAL]->Draw();
}

void CFadeManager::UpdateType01()
{
	switch (this->m_data.m_condeition)
	{
		//==============================
		// �t�F�[�h�C������
		//==============================
	case FADE_IN_PREPARE:
	{
		// ���F�ɂ���
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// �^�񒆂ɂ���
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// ��x���ׂĂ̕`����~�߂�
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// �I�u�W�F�N�g�̕`�������悤�ɂ���
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// �t�F�[�h�C�����s��
		this->m_data.m_next_condeition = FADE_IN_ACT;
	}
	break;
	//==============================
	// �t�F�[�h�C���I���܂�
	//==============================
	case FADE_IN_ACT:
	{
		// ���߂���
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();
		color.a -= 0.25f;

		if (color.a <= 0.0f) {
			color.a = 0.0f;

			// �t�F�[�h�C���I����
			this->m_data.m_next_condeition = FADE_IN_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// �t�F�[�h�C���I����
	//==============================
	case FADE_IN_END:
		break;
		//==============================
		// �t�F�[�h�A�E�g����
		//==============================
	case FADE_OUT_PREPARE:
	{
		// ���F�ɂ���
		Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// �^�񒆂ɂ���
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// ��x���ׂĂ̕`����~�߂�
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// �I�u�W�F�N�g�̕`�������悤�ɂ���
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);

		// �t�F�[�h�A�E�g���s��
		this->m_data.m_next_condeition = FADE_OUT_ACT;
	}
	break;
	//==============================
	// �t�F�[�h�A�E�g�I���܂�
	//==============================
	case FADE_OUT_ACT:
	{
		// ���߂���
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();
		color.a += 0.25f;

		if (color.a >= 1.0f) {
			color.a = 1.0f;

			// �t�F�[�h�C���I����
			this->m_data.m_next_condeition = FADE_OUT_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);
	}
	break;
	//==============================
	// �t�F�[�h�A�E�g�I����
	//==============================
	case FADE_OUT_END:
		break;
	}
}

void CFadeManager::DrawType01()
{
	this->m_object.mvp_image[FADE_IMAGE_NORMAL]->Draw();
}

void CFadeManager::UpdateType02()
{
	// CDirect3D�擾
	CDirect3D* direct3d = CSingleton<CDirect3D>::GetInstance();

	// �X�N���[���T�C�Y�i�[
	float screen_width, screen_height;
	direct3d->GetScreenSize(&screen_width, &screen_height);

	this->m_object.mvp_image[FADE_IMAGE_PLAYER]->Update();

	switch (this->m_data.m_condeition)
	{
		//==============================
		// �t�F�[�h�C������
		//==============================
	case FADE_IN_PREPARE:
	{
		// ���F�ɂ���
		Color color = { 0.0f, 0.0f, 0.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// �^�񒆂ɂ���
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		// ���F�ɂ���
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);

		// �^�񒆂ɂ���
		transform = this->m_object.mvp_image[FADE_IMAGE_PLAYER]->GetTransform();
		transform.position.x = 500.0f;
		transform.position.y = -200.0f;
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetTransform(&transform);

		// ��x���ׂĂ̕`����~�߂�
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// �I�u�W�F�N�g�̕`�������悤�ɂ���
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetFlg(&flg);

		// �t�F�[�h�C�����s��
		this->m_data.m_next_condeition = FADE_IN_ACT;
	}
	break;
	//==============================
	// �t�F�[�h�C���I���܂�
	//==============================
	case FADE_IN_ACT:
	{
		// ���F�ɂ���
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a -= 0.05f;

		if (color.a <= 0.0f) {
			color.a = 0.0f;
			this->m_data.mi_fade_count++;

			// �t�F�[�h�C���I����
			this->m_data.m_next_condeition = FADE_IN_END;
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;

		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);
	}
	break;
	//==============================
	// �t�F�[�h�C���I����
	//==============================
	case FADE_IN_END:
		break;
		//==============================
		// �t�F�[�h�A�E�g����
		//==============================
	case FADE_OUT_PREPARE:
	{
		// ���F�ɂ���
		Color color = { 0.0f, 0.0f, 0.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		// �^�񒆂ɂ���
		Transform transform = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetTransform();
		transform.position.x = 0.0f;
		transform.position.y = 0.0f;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetTransform(&transform);

		color = { 1.0f, 1.0f, 1.0f, 0.0f };
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);

		// �^�񒆂ɂ���
		transform = this->m_object.mvp_image[FADE_IMAGE_PLAYER]->GetTransform();
		transform.position.x = 500.0f;
		transform.position.y = -200.0f;
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetTransform(&transform);

		// ��x���ׂĂ̕`����~�߂�
		for (size_t i = 0; i < this->m_object.mvp_image.size(); i++)
		{
			obj_flg flg = OBJ_FLG_ACTIVE;
			this->m_object.mvp_image[i]->SetFlg(&flg);
		}

		// �I�u�W�F�N�g�̕`�������悤�ɂ���
		obj_flg flg = OBJ_FLG_ACTIVE | OBJ_FLG_DRAW;
		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetFlg(&flg);
		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetFlg(&flg);

		this->m_data.mi_fade_count = 0;

		// �t�F�[�h�A�E�g���s��
		this->m_data.m_next_condeition = FADE_OUT_ACT;
	}
	break;
	//==============================
	// �t�F�[�h�A�E�g�I���܂�
	//==============================
	case FADE_OUT_ACT:
	{
		// ���F�ɂ���
		Color color = this->m_object.mvp_image[FADE_IMAGE_NORMAL]->GetColor();

		color.a += 0.05f;

		if (color.a >= 1.0f) {
			color.a = 1.0f;
			this->m_data.mi_fade_count++;

			if (this->m_data.mi_fade_count > 20) {
				this->m_data.mi_fade_count = 0;
				// �t�F�[�h�C���I����
				this->m_data.m_next_condeition = FADE_OUT_END;
			}
		}

		this->m_object.mvp_image[FADE_IMAGE_NORMAL]->SetColor(&color);

		color.r = 1.0f;
		color.g = 1.0f;
		color.b = 1.0f;

		this->m_object.mvp_image[FADE_IMAGE_PLAYER]->SetColor(&color);
	}
	break;
	//==============================
	// �t�F�[�h�A�E�g�I����
	//==============================
	case FADE_OUT_END:
		break;
	}
}

void CFadeManager::DrawType02()
{
	this->m_object.mvp_image[FADE_IMAGE_NORMAL]->Draw();
	this->m_object.mvp_image[FADE_IMAGE_PLAYER]->Draw();
}
