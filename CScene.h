#pragma once

#include <vector>
#include <mutex>

class CDirect3D;
class CXAudio2;
class CInput;
class CTimer;

class CScene
{
public:
	// �R���X�g���N�^
	CScene();

	// �f�X�g���N�^
	~CScene();

	// ����������
	// �V�[���J�n���̏�����
	virtual void Initialize() = 0;

	// �I������
	// �V�[���I�����̏�����
	virtual void Finalize() = 0;

	// �X�V����
	virtual void Update() = 0;

	// �`�揈��
	virtual void Draw() = 0;

	// �|�[�Y���X�V����
	virtual void PauseUpdate() = 0;

	// �|�[�Y���`�揈��
	virtual void PauseDraw() = 0;

	//bool GetInitializedFlag();
	//void SetInitializedFlag(bool flg);

protected:
	// �������ς݃t���O
	bool mb_initialized_flg;

	// CDirectX�̃C���X�^���X
	CDirect3D* mp_directx;

	// CXAudio2�̃C���X�^���X
	CXAudio2* mp_xaudio2;

	// CInput�̃C���X�^���X
	CInput* mp_input;

	// CTimer�̃C���X�^���X
	CTimer* mp_timer;
};

