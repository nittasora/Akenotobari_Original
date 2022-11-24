#pragma once

// �쐬 : �V�c

//=============================
// �T�v�Ȃ�
//=============================
// �G�f�B�^�[�I���V�[��
// �G�f�B�^�[�ɍs�����A�T���v����ʂɍs�����A��߂邩�̑I��

#include "CScene.h" 

class CImage;
class CButton;
class CCursor;

class CEditorSelect : public CScene
{
public:
	// �R���X�g���N�^
	CEditorSelect();

	// �f�X�g���N�^
	~CEditorSelect();

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
	// �摜
	std::vector<CImage*> mvp_images;

	// �{�^��
	std::vector<CButton*> mvp_buttons;

	// �J�[�\��
	CCursor* mp_cursor;
};

