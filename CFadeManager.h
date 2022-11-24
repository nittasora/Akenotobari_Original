#pragma once

// �쐬 : �V�c
//		  ����

//=============================
// �T�v�Ȃ�
//=============================
// ��ʑJ�ڎ��̃t�F�[�h���Ǘ�

#include <vector>
#include "FadeData.h"

class CGameObject;
class CImage;

class CFadeManager
{
public:
	CFadeManager();
	~CFadeManager();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw();

	// �t�F�[�h�C�����J�n
	void StartFadeIn();

	// �t�F�[�h�C�����I��
	void StartFadeOut();
	void StartFadeOut(FADE_TYPE  type);

	// �t�F�[�h���I��������ǂ���
	bool CheckFadeInEnd();
	bool CheckFadeOutEnd();

	// �t�F�[�h�̎�ނ�ς���
	void SetType(FADE_TYPE  type);

private:

	// FADE_TYPE_00�p�̍X�V�E�`�揈��
	void UpdateType00();
	void DrawType00();

	// FADE_TYPE_01�p�̍X�V�E�`�揈�� (�����ς�G�f�B�^�\�p)
	void UpdateType01();
	void DrawType01();

	// FADE_TYPE_02�p�̍X�V�E�`�揈�� (�����ς�{�ԗp)
	void UpdateType02();
	void DrawType02();

	// �t�F�[�h�֌W�̃f�[�^
	struct
	{
		// ���݂̃t�F�[�h�̏�
		FADE_CONDITION m_condeition;

		// ���݂̃t�F�[�h�̃^�C�v
		FADE_TYPE  m_type;

		// ���̃t�F�[�h�̏�
		FADE_CONDITION m_next_condeition;

		// �t�F�[�h�Ɏg�����t���[����
		int mi_fade_count;
	} m_data;

	// �I�u�W�F�N�g�֌W
	struct
	{
		// delete�p
		std::vector<CGameObject*> mvp_delete_obj;

		// �\������I�u�W�F�N�g
		std::vector<CImage*> mvp_image;
	} m_object;
};

