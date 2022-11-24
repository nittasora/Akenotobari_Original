#pragma once

#include "CScene.h"

class CStageManager;
class CButton;

class CStagePlay : public CScene
{
public:
	// �R���X�g���N�^
	CStagePlay();

	// �f�X�g���N�^
	~CStagePlay();

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

private:
	CStageManager* mp_stage_manager;

	float mf_time;
};

