#pragma once

#include <vector>

class CDirect3D;
class CXAudio2;
class CInput;
class CTimer;

enum MENU_CONDITION
{
	MENU_FADE_IN_PREPARE,
	MENU_FADE_IN_ACT,
	MENU_FADE_IN_END,
	MENU_PREPARE,
	MENU_ACT,
	MENU_END,
	MENU_FADE_OUT_PREPARE,
	MENU_FADE_OUT_ACT,
	MENU_FADE_OUT_END,
};

class CMenuScene
{
public:
	// �R���X�g���N�^
	CMenuScene();

	// �f�X�g���N�^
	~CMenuScene();

	// ����������
	// �V�[���J�n���̏�����
	virtual void Initialize();

	// �I������
	// �V�[���I�����̏�����
	virtual void Finalize();

	// �X�V����
	virtual void Update();

	// �`�揈��
	virtual void Draw();

	void SetCondition(MENU_CONDITION next);

protected:
	// CDirectX�̃C���X�^���X
	CDirect3D* mp_directx;

	// CXAudio2�̃C���X�^���X
	CXAudio2* mp_xaudio2;

	// CInput�̃C���X�^���X
	CInput* mp_input;

	// CTimer�̃C���X�^���X
	CTimer* mp_timer;

	// ��
	MENU_CONDITION m_current_condition;
	MENU_CONDITION m_next_condition;
};

