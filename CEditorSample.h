#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// �G�f�B�^�[�p�̃T���v���V�[��
// ����ȍ~�ʏ�̃T���v���V�[���͎g�p���Ȃ��B
// �ʏ�ɔ�ׁA�㕔�ɂ��ǂ�{�^����ǉ��B

#include "CScene.h"

class CStageManager;
class CButton;

class CEditorSample : public CScene
{
public:
	// �R���X�g���N�^
	CEditorSample();

	// �f�X�g���N�^
	~CEditorSample();

	// ����������
	// �V�[���J�n���̏�����
	void Initialize() override final;

	// �I������
	// �V�[���I�����̏�����
	void Finalize() override final;

	// �X�V����
	void Update() override final;

	// �`�揈��
	void Draw() override final;

	// �|�[�Y���X�V����
	void PauseUpdate() override final;

	// �|�[�Y���`�揈��
	void PauseDraw() override final;

	// �����蔻��
	void ColliderTest();

private:
	// �X�e�[�W�}�l�[�W���[�C���X�^���X
	CStageManager* mp_stage_manager;

	// �{�^��
	std::vector<CButton*> mvp_button;
};

